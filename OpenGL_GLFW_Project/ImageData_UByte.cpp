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
GLenum getPreferedImageFormatForImplementation(GLenum textureTarget,
                                               GLenum internalFormat);


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
    GLenum dataRepresentation() const noexcept { mDataType_; }

    bool swapRedAndBlueChannels() noexcept;

private:
    ImageAttributes mAttributes_;
    std::vector<uint8_t> mImgData_;
    bool mWasResetToDefault_;
    bool mFlipRedAndBlueEnabled_;
    GLenum mInternalFormat_; //Used with 'glTextureStorage()' 
    GLenum mExternalFormat_; //Used With 'glTextureSubImage2D()'
    const GLenum mDataType_;

    //Sets this objects internal format to the correct type
    //based off the number of components are listed in the 
    //ImageAttributes member struct
    void setInternalFormatFromAttributes() noexcept;
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
}

ImageData_UByte::ImageDataImpl::ImageDataImpl(generateImgDataCallbackFunc generator,
                                              GLsizei width,
                                              GLsizei height)
    : mDataType_(GL_UNSIGNED_BYTE) {
    //todo
}

ImageData_UByte::ImageDataImpl::ImageDataImpl(const std::filesystem::path& imageFile)
    : mDataType_(GL_UNSIGNED_BYTE) {
    //todo
}

ImageData_UByte::ImageDataImpl::ImageDataImpl(GLsizei width,       
                                              GLsizei height,     
                                              GLsizei comp,        
                                              GLenum internalFormat, 
                                              GLenum externalFormat, 
                                              std::vector<uint8_t> data,
                                              std::string* errMsg = nullptr) 
    : mDataType_(GL_UNSIGNED_BYTE) {
    //todo

}

ImageData_UByte::ImageDataImpl::~ImageDataImpl() {
    fprintf(MSGLOG, "\nDestroying Image Data!\n");
}


void ImageData_UByte::ImageDataImpl::resetSelfFromInternalDefaultImage() noexcept {
    getDefaultImage(&mImgData_, &mAttributes_);
    mWasResetToDefault_ = true;
    setInternalFormatFromAttributes();
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
        parametersAgree == ((GL_RGB8 == internalFormat) ||
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
GLenum getPreferedImageFormatForImplementation(GLenum textureTarget,
                                               GLenum internalFormat) noexcept {

    assert(verifyIsInternalFormat(internalFormat));

    /*void glGetInternalFormativ(GLenum target​,
                                 GLenum internalformat​,
                                 GLenum pname​,
                                 GLsizei bufSize​,
                                 GLint *params​);*/

    if (textureTarget = GL_TEXTURE_1D) {
        GLint preferedFormat = 0;
        glGetInternalformativ(textureTarget,
                              internalFormat,
                              GL_INTERNALFORMAT_PREFERRED,
                              1,
                              &preferedFormat);

        const GLenum preferedFormatEnum = static_cast<GLenum>(preferedFormat);
        printf("\nThe preferred image format by this platform for using \'%s\'\n"
            "textures with data in the format \'%s\' is: \'%s\'\n",
            "GL_TEXTURE_1D",
            convertGLEnumToString(internalFormat).c_str(),
            convertGLEnumToString(preferedFormatEnum));
        return preferedFormatEnum;

    }
    else if (textureTarget == GL_TEXTURE_2D) {
        GLint preferedFormat = 0;
        glGetInternalformativ(textureTarget,
                              internalFormat,
                              GL_INTERNALFORMAT_PREFERRED,
                              1,
                              &preferedFormat);

        const GLenum preferedFormatEnum = static_cast<GLenum>(preferedFormat);
        printf("\nThe preferred image format by this platform for using \'%s\'\n"
            "textures with data in the format \'%s\' is: \'%s\'\n",
            "GL_TEXTURE_2D",
            convertGLEnumToString(internalFormat).c_str(),
            convertGLEnumToString(preferedFormatEnum));
        return preferedFormatEnum;
    }
    else if (textureTarget == GL_TEXTURE_2D_ARRAY) {
        GLint preferedFormat = 0;
        glGetInternalformativ(textureTarget,
            internalFormat,
            GL_INTERNALFORMAT_PREFERRED,
            1,
            &preferedFormat);

        const GLenum preferedFormatEnum = static_cast<GLenum>(preferedFormat);
        printf("\nThe preferred image format by this platform for using \'%s\'\n"
            "textures with data in the format \'%s\' is: \'%s\'\n",
            "GL_TEXTURE_2D_ARRAY",
            convertGLEnumToString(internalFormat).c_str(),
            convertGLEnumToString(preferedFormatEnum));
        return preferedFormatEnum;
    }
    else if (textureTarget = GL_TEXTURE_3D) {
        GLint preferedFormat = 0;
        glGetInternalformativ(textureTarget,
                              internalFormat,
                              GL_INTERNALFORMAT_PREFERRED,
                              1,
                              &preferedFormat);

        const GLenum preferedFormatEnum = static_cast<GLenum>(preferedFormat);
        printf("\nThe preferred image format by this platform for using \'%s\'\n"
            "textures with data in the format \'%s\' is: \'%s\'\n",
            "GL_TEXTURE_3D",
            convertGLEnumToString(internalFormat).c_str(),
            convertGLEnumToString(preferedFormatEnum));
        return preferedFormatEnum;
    }

    else {
        assert(false); //Querying this format isn't supported yet. You should add it...
    }

    return 0u;
}