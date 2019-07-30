//
// File:                               ImageData_UByte.cpp
//
// Implementation File for the class ImageData_UByte.
// Forrest Miller
// July 23-27, 2019

//                            SECTION [0]
///////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//        //       //      File Description Preamble      //       //        //
///////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
//  This file contains a fairly complex implementation                         
//  to a complicated class. It may prove helpful to                            
//  know that the layout of this source file is broken                         
//  down into these following sections:                                        
//                                                                             
// SECTION                      TITLE                                           
//                                                                             
//  [0]                File Description Preamble               [You Are Here]  
//                                                                             
//  [1]      Ordinary Include Statements/Constants/Macros                      
//                                                                             
//  [2]             Configure and Include 'stb_image'                          
//                                                                             
//  [3]    Function Prototypes For Internal Utility Functions                  
//                                                                             
//  [4]    Definition For The Static Default Checkerboard Test Image           
//                                                                             
//  [5]                  class ImageDataImpl                                   
//                                                                             
//  [6]   Implementation of Functions For class ImageData_UByte                
//                                                                             
//  [7]   Implementation of the Internal Utility Functions Declared in [3]    
//                                                                             
//                               [END]                                         


//-----------------------------------------------------------------------------


//                            SECTION [1]
///////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//      //       Ordinary Include Statements/Constants/Macros       //       //
///////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

#include "ImageData_UByte.h"
#include <fstream>

#include "LoggingMessageTargets.h"
#include "OpenGLEnumToString.h"  //Helps with tracking the meaning of hex 
//                               //GLenum data

typedef ImageData_UByte::ImageAttributes ImgAttrib;

//This is a class invariant discussed multiple times in the documentation.
//Please do not change it from 2. This is the minimum cutoff allowed for
//when specifying custom image dimensions
static constexpr const GLsizei MINIMUM_IMAGE_DIMENSION_SPAN = 2;

//This is a class invariant discussed multiple times in the documentation.
//Please do not change it from 4. Doing so wouldn't even make sense really,
//unless one day colors are done differently from the ubiquitous RGBA 
//channels.
static constexpr const GLsizei MAXIMUM_NUMBER_OF_COMPONENTS = 4;

//This is a class invariant that the implementation assumes will
//always be 4. Please do not change this value as that would most likely 
//break the implementation in multiple locations.
static constexpr const GLsizei DEFAULT_NUMBER_OF_COMPONENTS = 4;


//-----------------------------------------------------------------------------


//                            SECTION [2]
///////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//      //       //    Configure and Include 'stb_image'    //       //      //
///////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////


////////////////////////////////////////
//////  Error Message Verboseness
////////////////////////////////////////
//
// Option A
//Disable Failure Messages in stb_image with the macro
//#define STBI_NO_FAILURE_STRINGS
//
// Option B
//To Get Normal Failure Messages, don't define the macros
//for options 'A' and 'C'
//
// Option C
//To Get Increased Verbosity in failure messages, use the macro
#define STBI_FAILURE_USERMSG

#ifdef _MSC_VER 
#define STBI_WINDOWS_UTF8
#else 
#pragma warning("\n\nWARNING! The ImageData_UByte class has not "             \
           "been tested on this\n\t\tplatform EVER! Chances are it will not\n"\
           "\t\tfunction correctly!\n\n")
#endif //_MSC_VER

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"



//-----------------------------------------------------------------------------



//                            SECTION [3]
///////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//    //      Function Prototypes For Internal Utility Functions       //    //
///////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

//Checks to see if the specified GLenum refers to a valid 
//internal texture data storage format type. Returns true if
//internalFormat is an actual internal format, and false otherwise.
bool verifyIsInternalFormat(GLenum internalFormat) noexcept;

//Not every InternalFormat will make sense based off the image's attributes.
//For example a GL_R8 internal format doesn't make sense if the number of 
//components per pixel is not 1. This function returns true if the specified
//internalFormat makes sense with respect to the provided image attributes, 
//and false if there is a mismatch between them
bool verifyInternalFormatMatchesImageAttributes(GLenum internalFormat,
                                                ImgAttrib attributes) noexcept;

//Gets the preferred internal format for images and textures from the 
//OpenGL implementation. 
GLenum checkIfInternalFormatIsPreferredByImplementationForTextureTarget(GLenum textureTarget,
                                                                        GLenum internalFormat) noexcept;
 
//Returns the implementation-reported maximum width for the specified internalFormat 
//and texture binding target
GLsizei getMaximumWidthForInternalFormat(GLenum textureTarget, 
                                         GLenum internalFormat) noexcept;

//Returns the implementation-reported maximum height for the specified internalFormat 
//and texture binding target
GLsizei getMaximumHeightForInternalFormat(GLenum textureTarget,
                                          GLenum internalFormat) noexcept;

//Returns the implementation-reported maximum layers for the specified internalFormat 
//and texture binding target
GLsizei getMaximumLayersForInternalFormat(GLenum textureTarget,
                                          GLenum internalFormat) noexcept;

//Returns the implementation-reported maximum depth for the specified internalFormat 
//and texture binding target
GLsizei getMaximumDepthForInternalFormat(GLenum textureTarget,
                                         GLenum internalFormat) noexcept;

//"the maximum combined dimensions of a texture of the specified internal format" is 
//what the OpenGL standard says the query performed by this function returns. Do with
//that as you will... 
GLsizei getMaximumCombinedForInternalFormat(GLenum textureTarget,
                                            GLenum internalFormat) noexcept;


//THIS ONE IS JUST FOR EXPERIMENTAL PURPOSES
//Returns the Image Pixel Format preferred for the given texture target 
//and internal format 
GLenum getPreferredImagePixelFormatForInternalFormat(GLenum textureTarget,
                                                     GLenum internalFormat) noexcept;

//THIS ONE IS JUST FOR EXPERIMENTAL PURPOSES
//Returns the Image Pixel Type preferred for the given texture target 
//and internal format  
GLenum getPreferredImagePixelTypeForInternalFormat(GLenum textureTarget,
                                                   GLenum internalFormat) noexcept;

//-----------------------------------------------------------------------------


//                            SECTION [4]
///////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//    //     Definition For The Static Default Checkerboard Image     //     //
///////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////



void getDefaultImage(std::vector<uint8_t>* dataVecPtr,
                     ImgAttrib* attributesPtr) noexcept {
    //To make sure we are setting a valid dimension, we enforce our declared minimum
    //supported dimension size on the Default Image. [DEF_IMG_DIM == default image dimension]
    constexpr auto DEF_IMG_DIM = (DEFAULT_IMAGE_DIMENSIONS >= MINIMUM_IMAGE_DIMENSION_SPAN) ?
        DEFAULT_IMAGE_DIMENSIONS : MINIMUM_IMAGE_DIMENSION_SPAN;
    
    assert(dataVecPtr); 
    assert(attributesPtr);
    

    //These following static variables will be initialized the first time this function is
    //called after which they will be persistent to allow for subsequent function calls to
    //gain access to them quickly.
    
    //------------------
    //IMAGE ATTRIBUTES
    //-----------------
    const static ImgAttrib defaultImgAttribs(DEF_IMG_DIM, DEF_IMG_DIM, DEFAULT_NUMBER_OF_COMPONENTS);
    //We are finished initializing the default Image Attributes data so we 
    //can assign it to its corresponding pointer right away
    *attributesPtr = defaultImgAttribs;

    //------------------
    //Image Data 
    //------------------
    static bool defaultImageHasBeenSet = false;
    static uint8_t defaultImage[DEF_IMG_DIM][DEF_IMG_DIM][DEFAULT_NUMBER_OF_COMPONENTS] = { { { 0u } } };


    //Create the default image if it hasn't yet been created
    if (!defaultImageHasBeenSet) {
        for (GLsizei i = 0; i < DEF_IMG_DIM; i++) {
            for (GLsizei j = 0; j < DEF_IMG_DIM; j++) {
                for (GLsizei component = 0; component < DEFAULT_NUMBER_OF_COMPONENTS; component++) {
                    if ((i + j) % 2) //Create the checkerboard pattern
                        defaultImage[i][j][component] = DEFAULT_IMAGE_COLOR_1[component];
                    else 
                        defaultImage[i][j][component] = DEFAULT_IMAGE_COLOR_2[component];
                }
            }
        }
        defaultImageHasBeenSet = true;
    }


    //Finish Assigning the image to the vector pointer
    std::vector<uint8_t> defaultImageData;
    defaultImageData.reserve(DEF_IMG_DIM * DEF_IMG_DIM * DEFAULT_NUMBER_OF_COMPONENTS);
    //Copy the default image over into the vector
    for (GLsizei i = 0; i < DEF_IMG_DIM; i++) {
        for (GLsizei j = 0; j < DEF_IMG_DIM; j++) {
            for (GLsizei component = 0; component < DEFAULT_NUMBER_OF_COMPONENTS; component++) {
                defaultImageData.push_back(defaultImage[i][j][component]);
            }
        }
    }
    //Assign the vector we copied to the vector pointer
    std::swap(*dataVecPtr, defaultImageData);

}



//-----------------------------------------------------------------------------


//                            SECTION [5]
///////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//        //       //         class ImageDataImpl         //       //        //
///////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

class ImageData_UByte::ImageDataImpl {
public:
    ImageDataImpl();
    ImageDataImpl(float randSeed,
                  GLsizei width,
                  GLsizei height,
                  bool writeAnalysisOfGeneratedDataToFile);
    ImageDataImpl(generateImgDataCallbackFunc generator,
                  GLsizei width,
                  GLsizei height);
    ImageDataImpl(const std::filesystem::path& imageFile);

    ImageDataImpl(GLsizei width,       
                  GLsizei height,     
                  GLsizei comp,        
                  GLenum internalFormat, 
                  GLenum externalFormat, 
                  std::vector<uint8_t> data,
                  std::string* errMsg = nullptr);

    ~ImageDataImpl() noexcept;

    //If something is about to cause this object to enter into an 
    //invalid state, call this function to drop all current object state,
    //then create a valid state again from the backup data. Finally make
    //sure to mark the flag that this object is no longer holding the data
    //users might have been expecting to hold.
    void resetSelfFromInternalDefaultImage() noexcept;

    ImageDataImpl(const ImageDataImpl& that) noexcept;
    ImageDataImpl(ImageDataImpl&& that) noexcept;
    ImageDataImpl& operator=(const ImageDataImpl& that) noexcept;
    ImageDataImpl& operator=(ImageDataImpl&& that) noexcept;



    bool isDefaultImage() const noexcept { return mWasResetToDefault_; }
    ImageAttributes getAttributes() const noexcept { return mAttributes_; }
    GLsizei width() const noexcept { return mAttributes_.width; }
    GLsizei height() const noexcept { return mAttributes_.height; }
    GLsizei components() const noexcept { return mAttributes_.comp; }
    GLenum internalFormat() const noexcept { return mInternalFormat_; }
    GLenum externalFormat() const noexcept { return mExternalFormat_; }
    GLenum dataRepresentation() const noexcept { return mDataType_; }
    uint8_t* data() noexcept { return mImgData_.data(); }
    void uploadDataTo2DTexture(GLuint textureName,
                               GLint level) const noexcept;

    bool swapRedAndBlueChannels() noexcept;

private:
    ImageAttributes mAttributes_;
    std::vector<uint8_t> mImgData_;
    bool mWasResetToDefault_;
    bool mFlipRedAndBlueEnabled_;
    GLenum mInternalFormat_; //Used with 'glTextureStorage()' 
    GLenum mExternalFormat_; //Used With 'glTextureSubImage2D()'
    const GLenum mDataType_;

    //Utility Functions

    //Checks with the implementation to see if any of this object's current image 
    //attributes exceed the maximums supported by the implementation. 
    bool checkIfImageDimensionsExceedImplementationMaximum() const noexcept;

    //Sets this objects internal format to the correct type
    //based off the number of components are listed in the 
    //ImageAttributes member struct
    void setInternalFormatFromAttributes() noexcept;
    //Call This After calling setInternalFormatFromAttributes() only.
    //Queries the implementation to see if there is a preference, then
    //decides on the External Format the use.
    void selectAnExternalFormat() noexcept; 
};





ImageData_UByte::ImageDataImpl::ImageDataImpl()
    : mDataType_(GL_UNSIGNED_BYTE),
      mFlipRedAndBlueEnabled_(false) {
    resetSelfFromInternalDefaultImage(); //Simply reset self to the backup image
}

ImageData_UByte::ImageDataImpl::ImageDataImpl(float randSeed,
                                              GLsizei width,
                                              GLsizei height,
                                              bool writeAnalysisOfGeneratedDataToFile)
    : mDataType_(GL_UNSIGNED_BYTE) {
    
    //todo

    printf("\nConstructor not yet implemented! Reverting to\n"
        "default construction!\n");
    resetSelfFromInternalDefaultImage();
}

ImageData_UByte::ImageDataImpl::ImageDataImpl(generateImgDataCallbackFunc generator,
                                              GLsizei width,
                                              GLsizei height)
    : mDataType_(GL_UNSIGNED_BYTE) {
    
    //todo

    printf("\nConstructor not yet implemented! Reverting to\n"
        "default construction!\n");
    resetSelfFromInternalDefaultImage();
}

ImageData_UByte::ImageDataImpl::ImageDataImpl(const std::filesystem::path& imageFile)
    : mDataType_(GL_UNSIGNED_BYTE) {
   
    assert(!imageFile.empty());
    
    try {    
        if (!std::filesystem::exists(imageFile)) {
            throw std::exception("Dude what are you doing!\n"
                "That file doesn't exist!\n");
        }

        
#if defined(_MSC_VER) && _MSC_VER >= 1400

        const wchar_t* cstrFilenameWide = imageFile.c_str();
        static constexpr const size_t FILENAME_CONVERSION_BUFFER_SIZE = 2048;
        char cstrFilename[FILENAME_CONVERSION_BUFFER_SIZE] = { '\0' };
        int conversionResult = stbi_convert_wchar_to_utf8(cstrFilename,                   
                                                          FILENAME_CONVERSION_BUFFER_SIZE,
                                                          cstrFilenameWide);


#else 
#pragma error("Open File Functionality Not Implemented On Non-Windows Platforms Yet...\n")
        //Currently not implemented so what you will need to do if you
        //are not on Windows is quickly write in here your platforms C
        //API function for opening a FILE* from a filepath
#endif 


//This implementation that varies depending on a number of different circumstances 
//was getting pretty complicated and there is no reason for it to need to be. Thus
//rather than implementing multiple different paths through this function, now the
//most general implementation is taken in pretty much all cases
#if 0
        //'stb_image' has a maximum filepath length it supports for parsing files.
        //If the requested filepath this function gets called with exceeds this 
        //length, then we will need to adjust our implementation by handling opening
        //and closing the file handle ourselves
        if (imageFile.string().length() >= FILENAME_MAX) {
            FILE* imageFileHandle = nullptr; 
            //An inner try-catch block is used here to ensure
            //file handle gets closed
            try { 
#if defined(_MSC_VER) && _MSC_VER >= 1400
                if (0 != fopen_s(&imageFileHandle, imageFile.string().c_str(), "r")) {
                    imageFileHandle = nullptr;
                    throw std::exception("Error! Unable to open image file!\n");
                }
                
#else 

#endif 
            }
            catch (const std::exception& e) {
                if (imageFileHandle) {
                    fclose(imageFileHandle);
                    imageFileHandle = nullptr;
                } //Throw e out to the next layer of catch statements
                throw e;
            }
        }


        //See if we need to worry about wide character filepaths since we will be 
        //interacting with 'stb_image' to load the data, which is a C library and 
        //thus requires us to do more work on our end to make sure we are interacting
        //with it appropriately.
#ifdef _MSC_VER 
#ifdef STBI_WINDOWS_UTF8
        //This is way plenty
        static constexpr const size_t BUFFER_FOR_CONVERTED_FILEPATH_SIZE = 2048; 
#else

#endif //STBI_WINDOWS_UTF8
#else 
        //Platform is not windows, so no need to worry about wide strings

#endif 
#endif //IF 0
    }
    catch (const std::exception& e) {
        fprintf(WRNLOG, "\nCaught an exception while loading the "
            "image file\n\t\"%s\"\n"
            "All Dependent TextureS will be loaded using the ugly"
            " default image!\n"
            "Exception Message: %s\n\n",
            imageFile.string().c_str());
        resetSelfFromInternalDefaultImage();
    }
    catch (...) {
        fprintf(WRNLOG, "\nCaught an unanticipated exception while loading the "
            "image file\n\t\"%s\"\n"
            "All Dependent TextureS will be loaded using the ugly"
            " default image!\n\n",
            imageFile.string().c_str());
        resetSelfFromInternalDefaultImage();
    }
}

ImageData_UByte::ImageDataImpl::ImageDataImpl(GLsizei width,       
                                              GLsizei height,     
                                              GLsizei comp,        
                                              GLenum internalFormat, 
                                              GLenum externalFormat, 
                                              std::vector<uint8_t> data,
                                              std::string* errMsg) 
    : mDataType_(GL_UNSIGNED_BYTE) {
    
    //todo

    printf("\nConstructor not yet implemented! Reverting to\n"
        "default construction!\n");
    resetSelfFromInternalDefaultImage();
}

ImageData_UByte::ImageDataImpl::~ImageDataImpl() {
    fprintf(MSGLOG, "\nDestroying Image Data!\n");
}


void ImageData_UByte::ImageDataImpl::resetSelfFromInternalDefaultImage() noexcept {
    getDefaultImage(&mImgData_, &mAttributes_);
    mWasResetToDefault_ = true;
    setInternalFormatFromAttributes();
    selectAnExternalFormat();
}

ImageData_UByte::ImageDataImpl::ImageDataImpl(const ImageDataImpl& that) noexcept 
    : mDataType_(GL_UNSIGNED_BYTE) {
    //todo
}
ImageData_UByte::ImageDataImpl::ImageDataImpl(ImageDataImpl&& that) noexcept
    : mDataType_(GL_UNSIGNED_BYTE) {
    //todo
}
ImageData_UByte::ImageDataImpl& ImageData_UByte::ImageDataImpl::operator=(const ImageDataImpl& that) noexcept {
    if (this != &that) {
        //todo
    }
    return *this;
}
ImageData_UByte::ImageDataImpl& ImageData_UByte::ImageDataImpl::operator=(ImageDataImpl&& that) noexcept {
    if (this != &that) {
        //todo
    }
    return *this;
}

void ImageData_UByte::ImageDataImpl::uploadDataTo2DTexture(GLuint textureName,
                                                           GLint level) const noexcept {
    assert(textureName != 0u);
    
    glTextureSubImage2D(textureName,              //GLuint textureHandle
                        level,                    //GLint layers
                        0,                        //GLsizei xOffset  
                        0,                        //GLsizei yOffset
                        mAttributes_.width,       //Image Data Width
                        mAttributes_.height,      //Image Data Height
                        mExternalFormat_,         //How OpenGL Should Interpret pixel data
                        mDataType_,               //GLenum data type
                        mImgData_.data());        //Pointer to array of image data
}

bool ImageData_UByte::ImageDataImpl::swapRedAndBlueChannels() noexcept {
    
    if (mAttributes_.comp < 3)  //Has no impact if there are less than 3 components
        return mFlipRedAndBlueEnabled_;

    mFlipRedAndBlueEnabled_ = !mFlipRedAndBlueEnabled_;

    switch (mExternalFormat_) {
    default:
        assert(false); //unrecognized external format
        break;
    case (GL_RGB):
        mExternalFormat_ = GL_BGR;
        break;
    case (GL_RGBA):
        mExternalFormat_ = GL_BGRA;
        break;
    case (GL_BGR):
        mExternalFormat_ = GL_RGB;
        break;
    case (GL_BGRA):
        mExternalFormat_ = GL_RGBA;
        break;
    //Repeat the same pattern except with the INTEGER external formats
    case (GL_RGB_INTEGER):
        mExternalFormat_ = GL_BGR_INTEGER;
        break;
    case (GL_RGBA_INTEGER):
        mExternalFormat_ = GL_BGRA_INTEGER;
        break;
    case (GL_BGR_INTEGER):
        mExternalFormat_ = GL_RGB_INTEGER;
        break;
    case (GL_BGRA_INTEGER):
        mExternalFormat_ = GL_RGBA_INTEGER;
        break;
    }
    return mFlipRedAndBlueEnabled_;
}

//Checks with the implementation to see if any of this object's current image 
//attributes exceed the maximums supported by the implementation. 
bool ImageData_UByte::ImageDataImpl::checkIfImageDimensionsExceedImplementationMaximum() const noexcept {

    //Check Width
    static const auto MaxSupportedWidth =
        getMaximumWidthForInternalFormat(GL_TEXTURE_2D,
                                         mInternalFormat_);
    if (MaxSupportedWidth > mAttributes_.width) {
        fprintf(WRNLOG, "\nWarning! The Requested Image\n"
            "exceeds this implementation's reported maximum supported width\n"
            "for 2D textures!\n"
            "One day this Application may be able to break your image down\n"
            "into more manageable chunks, but for now it must simply fail at\n"
            "loading thus reseting itself using the default image...\n");
        return false;
    }
    
    //Check Height
    static const auto MaxSupportedHeight =
        getMaximumHeightForInternalFormat(GL_TEXTURE_2D, 
                                          mInternalFormat_);
    if (MaxSupportedHeight > mAttributes_.height) {
        fprintf(WRNLOG, "\nWarning! The Requested Image\n"
            "exceeds this implementation's reported maximum supported height\n"
            "for 2D textures!\n"
            "One day this Application may be able to break your image down\n"
            "into more manageable chunks, but for now it must simply fail at\n"
            "loading thus reseting itself using the default image...\n");
        return false;
    }

    //Check Depth          //IT TURNS OUT THAT THIS CLASS WON'T BE CONCERNED WITH DEPTH
    static const auto MaxSupportedDepth =
        getMaximumDepthForInternalFormat(GL_TEXTURE_2D,
                                         mInternalFormat_);
    //if (MaxSupportedDepth > mAttributes_.depth) {
    //    fprintf(WRNLOG, "\nWarning! The Requested Image\n"
    //        "exceeds this implementation's reported maximum supported depth\n"
    //        "for 2D textures!\n"
    //        "One day this Application may be able to break your image down\n"
    //        "into more manageable chunks, but for now it must simply fail at\n"
    //        "loading thus reseting itself using the default image...\n");
    //    return false;
    //}


    //Check Layers          //IT TURNS OUT THAT THIS CLASS WON'T BE CONCERNED WITH DEPTH
    static const auto MaxSupportedLayers =
        getMaximumLayersForInternalFormat(GL_TEXTURE_2D,
                                          mInternalFormat_);
    //if (MaxSupportedLayers > mAttributes_.layers) {
    //    fprintf(WRNLOG, "\nWarning! The Requested Image\n"
    //        "exceeds this implementation's reported maximum supported height\n"
    //        "for 2D textures!\n"
    //        "One day this Application may be able to break your image down\n"
    //        "into more manageable chunks, but for now it must simply fail at\n"
    //        "loading thus reseting itself using the default image...\n");
    //    return false;
    //}

    return true;
}

void ImageData_UByte::ImageDataImpl::setInternalFormatFromAttributes() noexcept {
    assert(mAttributes_.comp != 0);
    switch (mAttributes_.comp) {
    default:
        assert(false); //This case should never occur
        break;
    case (1):
        mInternalFormat_ = GL_R8;
        break;
    case (2):
        mInternalFormat_ = GL_RG8;
        break;
    case (3):
        mInternalFormat_ = GL_RGB8;
        break;
    case (4):
        mInternalFormat_ = GL_RGBA8;
        break;
    }
}


void ImageData_UByte::ImageDataImpl::selectAnExternalFormat() noexcept {
        //const GLenum recommendedByImplementation =
        //   checkIfInternalFormatIsPreferredByImplementationForTextureTarget(GL_TEXTURE_2D,
        //                                                                    mInternalFormat_);
        //fprintf(MSGLOG, "\nPreferred External Format for TEXTURE_2D is %s\n",
        //    convertGLEnumToString(recommendedByImplementation).c_str());

        const GLenum preferredImagePixelFormat =
            getPreferredImagePixelFormatForInternalFormat(GL_TEXTURE_2D, 
                                                          mInternalFormat_);
        const GLenum preferredImagePixelType = 
        getPreferredImagePixelTypeForInternalFormat(GL_TEXTURE_2D, 
                                                    mInternalFormat_);
        
        if (preferredImagePixelFormat != preferredImagePixelType) {
            fprintf(MSGLOG, "\n\n"
                "FYI! Alert! For internal image data format %s,\n"
                "the following has occurred: \n"
                "\tThe preferred image pixel type and\n"
                "\tpreferred image pixel format obtained\n"
                "from querying the implementation are\n"
                "different!\n"
                "\t\tPreferred Image Pixel Format is %#x [%s]\n"
                "\t\tPreferred Image Pixel Type is %#x [%s]\n"
                "Investigate further if interested!\n\n)",
                convertGLEnumToString(mInternalFormat_).c_str(),
                preferredImagePixelFormat,
                convertGLEnumToString(preferredImagePixelFormat).c_str(),
                preferredImagePixelType,
                convertGLEnumToString(preferredImagePixelType).c_str());
        }
        else {
            fprintf(MSGLOG, "\nDEBUG: Choosing External Format \"%s\" for\n"
                "Texture2D downloads of data in internal format \"%s\"\n\n",
                convertGLEnumToString(mExternalFormat_).c_str(),
                convertGLEnumToString(mInternalFormat_).c_str());
        }

        if (mAttributes_.comp < 3)
            return;
        else {
            
            //NEED TO BE AWARE OF SWAPPED RED AND BLUE CHANNELS 
            //Since the public interface function 'swapRedAndBlueChannels' 
            //affects what external format is to be used, it may be necessary
            //to disable this feature to set the next ExternalFormat. This way 
            //turning the Red and Blue swap back on will keep a consistent state
            //across all objects. 
            const bool redAndBlueFlipActive = mFlipRedAndBlueEnabled_;
            if (redAndBlueFlipActive) 
                swapRedAndBlueChannels(); //Disable the flip 
            

            mExternalFormat_ = preferredImagePixelFormat;
 
            //If we turned off the Red/Blue flip, turn it back on
            if (redAndBlueFlipActive) 
                swapRedAndBlueChannels();
            
        }
    }



//-----------------------------------------------------------------------------



//                            SECTION [6]
///////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//    //     Implementation of Functions For class ImageData_UByte     //    //
///////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

ImageData_UByte::ImageData_UByte() : pImpl_(nullptr) {
    try {
        pImpl_ = std::make_unique<ImageDataImpl>();
        assert(pImpl_);
        return;
    }
    catch (const std::bad_alloc& badAlloc) {
        fprintf(ERRLOG, "A Bad Allocation has occurred while creating an\n"
            "ImageData_UByte class!\nMessage: %s\n\n", badAlloc.what());
        std::exit(EXIT_FAILURE);
    }
}


ImageData_UByte::ImageData_UByte(float randSeed,
                                 GLsizei width,
                                 GLsizei height,
                                 bool writeAnalysisOfGeneratedDataToFile)  
    : pImpl_(nullptr) {
    
    try {
        pImpl_ = std::make_unique<ImageDataImpl>(randSeed,
                                                 width,
                                                 height,
                                                 writeAnalysisOfGeneratedDataToFile);
        assert(pImpl_);
    }
    catch (const std::bad_alloc& badAlloc) {
        fprintf(ERRLOG, "A Bad Allocation has occurred while creating an\n"
            "ImageData_UByte class!\nMessage: %s\n\n", badAlloc.what());
        std::exit(EXIT_FAILURE);
    }
}


ImageData_UByte::ImageData_UByte(generateImgDataCallbackFunc generator,
                                 GLsizei width,
                                 GLsizei height)  
    : pImpl_(nullptr) {
    
    try {
        pImpl_ = std::make_unique<ImageDataImpl>(generator, width, height);
        assert(pImpl_);
    }
    catch (const std::bad_alloc& badAlloc) {
        fprintf(ERRLOG, "A Bad Allocation has occurred while creating an\n"
            "ImageData_UByte class!\nMessage: %s\n\n", badAlloc.what());
        std::exit(EXIT_FAILURE);
    }
}


ImageData_UByte::ImageData_UByte(const std::filesystem::path& imageFile)
    : pImpl_(nullptr) {

    try {
        pImpl_ = std::make_unique<ImageDataImpl>(imageFile);
        assert(pImpl_);
    }
    catch (const std::bad_alloc& badAlloc) {
        fprintf(ERRLOG, "A Bad Allocation has occurred while creating an\n"
            "ImageData_UByte class!\nMessage: %s\n\n", badAlloc.what());
        std::exit(EXIT_FAILURE);
    }
}



ImageData_UByte::ImageData_UByte(GLsizei width,       
                                 GLsizei height,     
                                 GLsizei comp,        
                                 GLenum internalFormat, 
                                 GLenum externalFormat, 
                                 std::vector<uint8_t> data,
                                 std::string* errMsg) 
    : pImpl_(nullptr) {
    try {
        pImpl_ = std::make_unique<ImageDataImpl>(width, height, comp,
            internalFormat, externalFormat, data, errMsg);
        assert(pImpl_);
    }
    catch (const std::bad_alloc& badAlloc) {
        fprintf(ERRLOG, "A Bad Allocation has occurred while creating an\n"
            "ImageData_UByte class!\nMessage: %s\n\n", badAlloc.what());
        std::exit(EXIT_FAILURE);
    }
}

bool ImageData_UByte::isDefaultImage() const noexcept {
    return pImpl_->isDefaultImage();
}

ImageData_UByte::ImageAttributes ImageData_UByte::getAttributes() const noexcept {
    return pImpl_->getAttributes(); 
}
GLsizei ImageData_UByte::width() const noexcept {
    return pImpl_->width();
}
GLsizei ImageData_UByte::height() const noexcept {
    return pImpl_->height();
}
GLsizei ImageData_UByte::components() const noexcept {
    return pImpl_->components();
}
GLenum ImageData_UByte::internalFormat() const noexcept {
    return pImpl_->internalFormat();
}
GLenum ImageData_UByte::externalFormat() const noexcept {
    return pImpl_->externalFormat();
}
GLenum ImageData_UByte::dataRepresentation() const noexcept {
    return pImpl_->dataRepresentation();
}
uint8_t* ImageData_UByte::data() noexcept {
    return pImpl_->data();
}
void ImageData_UByte::uploadDataTo2DTexture(GLuint textureName,
                                            GLint level) const noexcept {
    return pImpl_->uploadDataTo2DTexture(textureName, level);
}
bool ImageData_UByte::swapRedAndBlueChannels() noexcept {
    return pImpl_->swapRedAndBlueChannels();
}

ImageData_UByte::~ImageData_UByte() noexcept { ; }

ImageData_UByte::ImageData_UByte(const ImageData_UByte& that) noexcept {
    assert(that.pImpl_);
    fprintf(WRNLOG, "\nWarning! Calling copy constructor on an ImageData_UByte\n"
        "object. This operation is potentially very expensive. See if it\n"
        "is possible to use a move operation instead?\n\n");
    pImpl_ = std::make_unique<ImageDataImpl>(*(that.pImpl_));
}
ImageData_UByte& ImageData_UByte::operator=(const ImageData_UByte& that) noexcept {
    assert(that.pImpl_);
    if (this != &that) {
        fprintf(WRNLOG, "\nWarning! Calling copy operator on an ImageData_UByte\n"
            "object. This operation is potentially very expensive. See if it\n"
            "is possible to use a move operation instead?\n\n");
        pImpl_ = std::make_unique<ImageDataImpl>(*(that.pImpl_));
    }
    return *this;
}
ImageData_UByte::ImageData_UByte(ImageData_UByte&& that) noexcept {
    assert(that.pImpl_);
    pImpl_ = std::move(that.pImpl_);
}
ImageData_UByte& ImageData_UByte::operator=(ImageData_UByte&& that) noexcept {
    assert(that.pImpl_);
    if (this != &that)
        pImpl_ = std::move(that.pImpl_);
    return *this;
}


//-----------------------------------------------------------------------------


//                            SECTION [7]
///////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//      Implementation of the Internal Utility Functions Declared in [3]     //
///////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

//Checks to see if the specified GLenum refers to a valid 
//internal texture data storage format type. Returns true if
//internalFormat is an actual internal format, and false otherwise.
bool verifyIsInternalFormat(GLenum internalFormat) noexcept {
   
    //OPTION A
    //std::string name("");
    //END OPTION A
    
    //OPTION B
#define name if constexpr (true) {return true;} neverset
    std::string neverset; //This is a decoy string that will never be set to 
    //a value
    //END OPTION B

    switch (internalFormat) {
    default:
        break;
    case GL_R8:
        name = "GL_R8";
        break;
    case GL_R8_SNORM:
        name = "GL_R8_SNORM";
        break;
    case GL_R16:
        name = "GL_R16";
        break;
    case GL_R16_SNORM:
        name = "GL_R16_SNORM";
        break;
    case GL_R3_G3_B2:
        name = "GL_R3_G3_B2";
        break;
    case GL_RGB4:
        name = "GL_RGB4";
        break;
    case GL_RGB5:
        name = "GL_RGB5";
        break;
    case GL_RGB565:
        name = "GL_RGB565";
        break;
    case GL_RGB8:
        name = "GL_RGB8";
        break;
    case GL_RGB8_SNORM:
        name = "GL_RGB8_SNORM";
        break;
    case GL_RGB10:
        name = "GL_RGB10";
        break;
    case GL_RGB12:
        name = "GL_RGB12";
        break;
    case GL_RGB16:
        name = "GL_RGB16";
        break;
    case GL_RGB16_SNORM:
        name = "GL_RGB16_SNORM";
        break;
    case GL_RGBA2:
        name = "GL_RGBA2";
        break;
    case GL_RGBA4:
        name = "GL_RGBA4";
        break;
    case GL_RGB5_A1:
        name = "GL_RGB5_A1";
        break;
    case GL_RGBA8:
        name = "GL_RGBA8";
        break;
    case GL_RGBA8_SNORM:
        name = "GL_RGBA8_SNORM";
        break;
    case GL_RGB10_A2:
        name = "GL_RGB10_A2";
        break;
    case GL_RGB10_A2UI:
        name = "GL_RGB10_A2UI";
        break;
    case GL_RGBA12:
        name = "GL_RGBA12";
        break;
    case GL_RGBA16:
        name = "GL_RGBA16";
        break;
    case GL_RGBA16_SNORM:
        name = "GL_RGBA16_SNORM";
        break;
    case GL_SRGB8:
        name = "GL_SRGB8";
        break;
    case GL_SRGB8_ALPHA8:
        name = "GL_SRGB8_ALPHA8";
        break;
    case GL_R16F:
        name = "GL_R16F";
        break;
    case GL_RG16F:
        name = "GL_RG16F";
        break;
    case GL_RGB16F:
        name = "GL_RGB16F";
        break;
    case GL_RGBA16F:
        name = "GL_RGBA16F";
        break;
    case GL_R32F:
        name = "GL_R32F";
        break;
    case GL_RG32F:
        name = "GL_RG32F";
        break;
    case GL_RGB32F:
        name = "GL_RGB32F";
        break;
    case GL_RGBA32F:
        name = "GL_RGBA32F";
        break;
    case GL_R11F_G11F_B10F:
        name = "GL_R11F_G11F_B10F";
        break;
    case GL_RGB9_E5:
        name = "GL_RGB9_E5";
        break;
    case GL_R8I:
        name = "GL_R8I";
        break;
    case GL_R8UI:
        name = "GL_R8UI";
        break;
    case GL_R16I:
        name = "GL_R16I";
        break;
    case GL_R16UI:
        name = "GL_R16UI";
        break;
    case GL_R32I:
        name = "GL_R32I";
        break;
    case GL_R32UI:
        name = "GL_R32UI";
        break;
    case GL_RG8I:
        name = "GL_RG8I";
        break;
    case GL_RG8UI:
        name = "GL_RG8UI";
        break;
    case GL_RG16I:
        name = "GL_RG16I";
        break;
    case GL_RG16UI:
        name = "GL_RG16UI";
        break;
    case GL_RG32I:
        name = "GL_RG32I";
        break;
    case GL_RG32UI:
        name = "GL_RG32UI";
        break;
    case GL_RGB8I:
        name = "GL_RGB8I";
        break;
    case GL_RGB8UI:
        name = "GL_RGB8UI";
        break;
    case GL_RGB16I:
        name = "GL_RGB16I";
        break;
    case GL_RGB16UI:
        name = "GL_RGB16UI";
        break;
    case GL_RGB32I:
        name = "GL_RGB32I";
        break;
    case GL_RGB32UI:
        name = "GL_RGB32UI";
        break;
    case GL_RGBA8I:
        name = "GL_RGBA8I";
        break;
    case GL_RGBA8UI:
        name = "GL_RGBA8UI";
        break;
    case GL_RGBA16I:
        name = "GL_RGBA16I";
        break;
    case GL_RGBA16UI:
        name = "GL_RGBA16UI";
        break;
    case GL_RGBA32I:
        name = "GL_RGBA32I";
        break;
    case GL_RGBA32UI:
        name = "GL_RGBA32UI";
        break;
    }

    // OPTION A
    ////This isn't the most efficient implementation but it does save me 
    ////a fair amount of typing for now...
    //return (name.empty());
    //END OPETION A

    //OPTION B
    return false;
#ifdef name
#undef name
#endif
    //End OPTION B
}


//I think this covers all the possible internal data formats possible
//for using unsigned byte data, but there could be some I left out in
//which case this function will need to be updated to reflect that.
bool verifyInternalFormatMatchesImageAttributes(GLenum internalFormat,
                                                ImgAttrib attributes) noexcept {
    bool parametersAgree = false; //Parameters are by default assumed to disagree
    switch (attributes.comp) {
    default: 
        assert(false); //This case shouldn't happen ever for any reason
        break;
    case (1):
        parametersAgree = (GL_R8 == internalFormat);
        break;
    case (2):
        parametersAgree = (GL_RG8 == internalFormat);
        break;
    case (3):
        parametersAgree = ((GL_RGB8 == internalFormat) ||
                           (GL_RGB8_SNORM == internalFormat) ||
                           (GL_SRGB8 == internalFormat));
        break;
    case (4):
        parametersAgree = ((GL_RGBA8 == internalFormat) ||
                           (GL_RGBA8_SNORM == internalFormat) ||
                           (GL_SRGB8_ALPHA8 == internalFormat));
        break;
    }
    return parametersAgree;
}

//Gets the preferred internal format for images and textures from the 
//OpenGL implementation. 
GLenum checkIfInternalFormatIsPreferredByImplementationForTextureTarget(GLenum textureTarget,
                                                                        GLenum internalFormat) noexcept {

    assert(verifyIsInternalFormat(internalFormat));

    /*void glGetInternalFormativ(GLenum target​,
                                 GLenum internalformat​,
                                 GLenum pname​,
                                 GLsizei bufSize​,
                                 GLint *params​);*/

    if (textureTarget == GL_TEXTURE_1D) {
        GLint preferredFormat = 0;
        glGetInternalformativ(textureTarget,
                              internalFormat,
                              GL_INTERNALFORMAT_PREFERRED,
                              1,
                              &preferredFormat);

        const GLenum preferredFormatEnum = static_cast<GLenum>(preferredFormat);
        printf("\nThe preferred image format by this platform for using \'%s\'\n"
            "textures with data in the format \'%s\' is: \'%s\'\n",
            "GL_TEXTURE_1D",
            convertGLEnumToString(internalFormat).c_str(),
            convertGLEnumToString(preferredFormatEnum).c_str());
        return preferredFormatEnum;

    }
    else if (textureTarget == GL_TEXTURE_2D) {
        GLint preferredFormat = 0;
        glGetInternalformativ(textureTarget,
                              internalFormat,
                              GL_INTERNALFORMAT_PREFERRED,
                              1,
                              &preferredFormat);

        const GLenum preferredFormatEnum = static_cast<GLenum>(preferredFormat);
        printf("\nThe preferred image format by this platform for using \'%s\'\n"
            "textures with data in the format \'%s\' is: \'%s\'\n",
            "GL_TEXTURE_2D",
            convertGLEnumToString(internalFormat).c_str(),
            convertGLEnumToString(preferredFormatEnum).c_str());
        return preferredFormatEnum;
    }
    else if (textureTarget == GL_TEXTURE_2D_ARRAY) {
        GLint preferredFormat = 0;
        glGetInternalformativ(textureTarget,
                              internalFormat,
                              GL_INTERNALFORMAT_PREFERRED,
                              1,
                              &preferredFormat);

        const GLenum preferredFormatEnum = static_cast<GLenum>(preferredFormat);
        printf("\nThe preferred image format by this platform for using \'%s\'\n"
            "textures with data in the format \'%s\' is: \'%s\'\n",
            "GL_TEXTURE_2D_ARRAY",
            convertGLEnumToString(internalFormat).c_str(),
            convertGLEnumToString(preferredFormatEnum).c_str());
        return preferredFormatEnum;
    }
    else if (textureTarget == GL_TEXTURE_3D) {
        GLint preferredFormat = 0;
        glGetInternalformativ(textureTarget,
                              internalFormat,
                              GL_INTERNALFORMAT_PREFERRED,
                              1,
                              &preferredFormat);

        const GLenum preferredFormatEnum = static_cast<GLenum>(preferredFormat);
        printf("\nThe preferred image format by this platform for using \'%s\'\n"
            "textures with data in the format \'%s\' is: \'%s\'\n",
            "GL_TEXTURE_3D",
            convertGLEnumToString(internalFormat).c_str(),
            convertGLEnumToString(preferredFormatEnum).c_str());
        return preferredFormatEnum;
    }

    else {
        assert(false); //Querying this format isn't supported yet. You should add it...
    }

    return 0u;
}




GLsizei getMaximumWidthForInternalFormat(GLenum textureTarget, 
                                         GLenum internalFormat) noexcept {
    assert(verifyIsInternalFormat(internalFormat));
    int32_t maxWidth = -1;
    glGetInternalformativ(textureTarget, 
                          internalFormat,
                          GL_MAX_WIDTH,
                          1,
                          &maxWidth);
    return maxWidth;
}



GLsizei getMaximumHeightForInternalFormat(GLenum textureTarget,
                                          GLenum internalFormat) noexcept {
    assert(verifyIsInternalFormat(internalFormat));
    int32_t maxHeight = -1;
    glGetInternalformativ(textureTarget, 
                          internalFormat,
                          GL_MAX_HEIGHT,
                          1,
                          &maxHeight);
    return maxHeight;
}


GLsizei getMaximumLayersForInternalFormat(GLenum textureTarget,
                                          GLenum internalFormat) noexcept {
    assert(verifyIsInternalFormat(internalFormat));
    int32_t maxLayers = -1;
    glGetInternalformativ(textureTarget,
                          internalFormat,
                          GL_MAX_HEIGHT,
                          1,
                          &maxLayers);
    return maxLayers;
}


GLsizei getMaximumDepthForInternalFormat(GLenum textureTarget,
                                         GLenum internalFormat) noexcept {
    assert(verifyIsInternalFormat(internalFormat));
    int32_t maxDepth = -1;
    glGetInternalformativ(textureTarget, 
                          internalFormat,
                          GL_MAX_HEIGHT,
                          1,
                          &maxDepth);
    return maxDepth;
}


GLsizei getMaximumCombinedForInternalFormat(GLenum textureTarget,
                                            GLenum internalFormat) noexcept {
    assert(verifyIsInternalFormat(internalFormat));
    int32_t maxDimensions[10] = { -1 };
    glGetInternalformativ(textureTarget, 
                          internalFormat,
                          GL_MAX_HEIGHT,
                          10,
                          &maxDimensions[0]);
    for (auto i = 0; i < 10; i++)
        printf("\nmaxDimensions[%d] == %d", i, maxDimensions[i]);
    printf("\n");
    return maxDimensions[0];
}


GLenum getPreferredImagePixelFormatForInternalFormat(GLenum textureTarget,
                                                     GLenum internalFormat) noexcept {
    assert(verifyIsInternalFormat(internalFormat));
    GLenum preferredFormat = GL_NONE;

    glGetInternalformativ(textureTarget,
                          internalFormat,
                          GL_IMAGE_PIXEL_FORMAT,
                          1,
                          (GLint*)&preferredFormat);
    if (GL_NONE == preferredFormat) {
        printf("\nQuery to get preferred image pixel format for internal\n"
            "format %s from implementation has returned GL_NONE!\n"
            "This means that this internal format and texture target\n"
            "not supported by this implementation!\n\n",
            convertGLEnumToString(internalFormat).c_str());
    } 
    printf("\nPreferred Format for %s is %#x!\n",
        convertGLEnumToString(internalFormat).c_str(), preferredFormat);
    return preferredFormat;
}

//THIS ONE IS JUST FOR EXPERIMENTAL PURPOSES
//Returns the Image Pixel Type preferred for the given texture target 
//and internal format  
GLenum getPreferredImagePixelTypeForInternalFormat(GLenum textureTarget,
                                                    GLenum internalFormat) noexcept {
    assert(verifyIsInternalFormat(internalFormat));
    GLenum preferredType = GL_NONE;

    glGetInternalformativ(textureTarget,
                          internalFormat,
                          GL_IMAGE_PIXEL_FORMAT,
                          1,
                          (GLint*)& preferredType);
    if (GL_NONE == preferredType) {
        printf("\nQuery to get preferred image pixel type for internal\n"
            "format %s from implementation has returned GL_NONE!\n"
            "This means that this internal format and texture target\n"
            "not supported by this implementation!\n\n", convertGLEnumToString(internalFormat).c_str());
    }
    printf("\nPreferred Type for %s is %#x!\n",
        convertGLEnumToString(internalFormat).c_str(), preferredType);
    return preferredType;

}
