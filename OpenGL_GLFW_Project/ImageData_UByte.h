//                                                                             
// File:                                         ImageData_UByte.h             
// Class:                                        ImageData_UByte               
//                                                                             
// Designation:                                  Data Wrapper                  
//                                                                             
// QuickInfo:           OPERATION                          Performance Cost    
//                  Default Constructor      ENABLED             [LOW]         
//                  Copy Operations          ENABLED           [EXTREME]       
//                  Move Operations          ENABLED             [LOW]         
//                                                                             
//                                                                             
// Description:                Designed to serve as an easy-to-use data wrapper
//                         for data when working with textures and image files.
//                            Each instance of this class contains internally a
//                             valid block of image data in the format of 8-bit
//                                 unsigned bytes (internally represented using
//                                                    the data type 'uint8_t').
//                                                                             
//                                                                             
// Invariants:                                                                 
//                                                                             
//     [INTERNAL DATA REPRESENTATION]      ---> Internal Data Always Is Of Type 
//                                             8-bit Unsigned Byte [Represented
//                                                Internally As Type 'uint8_t'] 
//                                                                             
//          [ALWAYS VALID]          ---> Objects of this type are guaranteed to
//                                        always be in a valid state with image
//                                         data available for use upon request.
//                                                                             
//   Requiring these objects to always maintain usable data in a valid state   
//   necessitates providing them with some reliable method of regaining valid  
//   data in the face of catastrophe. Thus for ensuring these objects have in  
//   place a highly-reliable path to recovery is what this next invariant      
//   provides.                                                                 
//                                                                             
// [Error Contingency Response Invariant]     --->  All instances of this class
//                                              share internally a small static
//                                               image of checkerboard pattern.
//
//   If an operation were to occur at any point in this objects lifetime [well,
//        maybe besides when the destructor is running] that would render this 
//       object's state invalid, rather than allow control to return with this 
//      object invalid these objects will simply flush all of current data and 
//     instead sets itself from this small static default image. These objects 
//        provides a method for querying whether their data has been corrupted 
//             causing them to instead take on this backup image data with the 
//         function                                                            
//                                bool IsDefaultImage() const noexcept;        
//                                                                             
//                                                                             
// Dependencies:             Due to the close proximity between this class and 
//                         the OpenGL API, this class depends on having access 
//                          to OpenGL types and thus requires an OpenGL loader 
//                               library (such as 'glad') in order to function.
//                                                                             
//                           The implementation file relies upon the C-library 
//                          'stb_image' behind the scenes to parse model files.
//                                                                             
//                                                                             
//                                                                             
// Supported Image Formats:                                                    
//                              '.jpg'                                         
//                              '.png'                                         
//                              '.tga'         [UNTESTED]                      
//                              '.bmp'         [UNTESTED]                      
//                              '.psd'         [UNTESTED]                      
//                              '.gif'         [UNTESTED]                      
//                              '.hdr'         [UNTESTED]                      
//                              '.pic'         [UNTESTED]                      
//                           '.ppm'/'.pgm'     [UNTESTED]   (Not sure if these 
//                                                             file extensions 
//                                                                 are correct)
//      [Consult "stb_imaghe.h" for limitations                                
//         within each of these supported formats]                             
//                                                                             
//                                                                             
//  References:                                                                
// https://software.intel.com/en-us/articles/opengl-performance-tips-use-native-formats-for-best-rendering-performance
//                                                                             
//   https://www.opengl.org/wiki/Image_Format                                  
//                                                                             
// Programmer:                                   Forrest Miller                
// Date:                                         July 2019                     
//                                                                             

#pragma once

#ifndef IMAGE_DATA_UBYTE_H_
#define IMAGE_DATA_UBYTE_H_

#include <memory>
#include <filesystem>
#include <functional>
#include "GlobalIncludes.h"    //For including OpenGL libraries


///////////////////////////
//       Constants       //
///////////////////////////

//Image Dimensions of dynamically generated image data to be used if 
//custom dimensions were not specified to constructor
static constexpr const GLsizei DEFAULT_GENERATED_IMAGE_HEIGHT = 64;
static constexpr const GLsizei DEFAULT_GENERATED_IMAGE_WIDTH = 64;

//The ImageData_UByte class is guaranteed to always maintain valid state with,
//usable data, even when attempts are made to construct it with bogus data.
//To fulfill this guarantee, an internal default image is generated to be 
//shared internally by objects of this type. These constants control what the 
//default image will be. Note that these values are still validated and will 
//be ignored if they violate the restrictions placed upon all images.
static constexpr const GLsizei DEFAULT_IMAGE_DIMENSIONS = 4;

//Specify default image colors that comprise the checkerboard pattern
static const glm::u8vec4 DEFAULT_IMAGE_COLOR_1 = { 145u, 18u, 138u, 255u/201u };
static const glm::u8vec4 DEFAULT_IMAGE_COLOR_2 = { 165u, 195u, 248u, 225u };


// DESCRIPTION
//     To specify a custom ImageData generation function simply write your own 
//     and then pass it in as a callback.
//  INPUT  
//     Parameters 'y' and 'x' represent the coordinates of the pixel currently 
//     being written to in the image and will range from 0 to image height-1 or
//     width-1, respectively. The third parameter 'compIndx' represents which  
//     color component is being assigned by the function call. While the actual
//     range of 'compIndx' depends on the number of components per pixel being 
//     written, it's range is guaranteed to always be a value found within the 
//     set {0, 1, 2, 3}. By default pixel components will be ordered as BGRA
//  OUTPUT
//     Output must be an 8-bit unsigned char ('uint8_t' works great) value.    
//     This means if writing values by converting from decimal values (as I 
//     suggest you should do), the range of the output must satisfy the 
//     condition:
//                0 <= output < 255
typedef std::function<uint8_t(GLsizei x,
                              GLsizei y,
                              GLsizei compIndx)> generateImgDataCallbackFunc;



class ImageData_UByte final {
public:

    //  +---------------------------------------------------------------------+
    //  |                         PUBLIC INTERFACE                            |
    //  +---------------------------------------------------------------------+

    //~~~~~~~~~~~~~~~~~~
    //  CONSCTRUCTORS
    //~~~~~~~~~~~~~~~~

    //DEFAULT CONSTRUCTOR -- LOAD STATIC TEST IMAGE
    //Creates an ImageData object from a static image 
    //of a basic test checkerboard pattern. This image
    //will always have 4 data components.
    ImageData_UByte();

    //DYNAMIC CONSTRUCTOR -- GENERATE IMAGE INTERNALLY
    //This object is able to generate itself dynamically 
    //by calling its constructor with a value to use as the
    //random seed for the data generation. Optionally custom
    //image widths and heights can be specified too, with 
    //each dimension being at least 2 pixels. 
    //
    //While implementing the internal data generation algorithm
    //used by this constructor I was curious about how the 
    //breakdown of the various data paths looked, so I 
    //created a way to print details reporting how the 
    //image data is assigned out to a file. I decided this 
    //feature was interesting enough to leave it optional
    //for public usage. PLEASE UPDATE DOCUMENTATION HERE ONCE
    //IT BECOMES CLEAR WHERE THIS REPORT FILE WILL GET SAVED AND
    //WHAT NAME IT WILL HAVE.
    ImageData_UByte(float randSeed,
                    GLsizei width = DEFAULT_GENERATED_IMAGE_WIDTH,
                    GLsizei height = DEFAULT_GENERATED_IMAGE_HEIGHT,
                    bool writeAnalysisOfGeneratedDataToFile = false);

    //  DYNAMIC CONSTRUCTOR  --  CUSTOM IMAGE GENERATION 
    //Allows for a custom generation function to be 
    //specified which is then used by this object to 
    //dynamically create itself. Optionally can pass
    //in custom image dimensions to allow for increased
    //control over the resulting image. Both dimensions
    //are minimally required to be at least 2, 
    //specifying smaller values will be ignored and their
    //dimension set to 2.
    ImageData_UByte(generateImgDataCallbackFunc generator,
                    GLsizei width = DEFAULT_GENERATED_IMAGE_WIDTH,
                    GLsizei height = DEFAULT_GENERATED_IMAGE_HEIGHT);

    //  DYNAMIC CONSTRUCTOR  --  LOAD IMAGE DATA FROM IMAGE FILE
    //Please make sure the filepath exists. Failure to load the image
    //will result in this class falling back to using the static test
    //image pattern which would be assigned by calling the default 
    //constructor.
    ImageData_UByte(const std::filesystem::path& imageFile);



    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //  PUBLIC MEMBER DATA TYPES
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~
  
    //Public Member Data Type [This class describes key
    //information for how to interpret stored image data]
    class ImageAttributes final {
    public:
        GLsizei width; //Must be 2 or greater
        GLsizei height; //Must be 2 or greater
        GLsizei comp; //Number of components per pixel
        //Returns the number of bytes in total are expected
        //in an image matching these attributes
        GLsizei sizeInBytes() const noexcept {
            return (width * height * comp);
        }
        //Construct image attributes
        ImageAttributes(GLsizei width = 0,
                        GLsizei height = 0,
                        GLsizei comp = 0) : width(width),
                                            height(height),
                                            comp(comp) { /*assert(comp <= 4);*/}
        ~ImageAttributes() noexcept = default;
        ImageAttributes(const ImageAttributes& that) noexcept = default;
        ImageAttributes(ImageAttributes&& that) noexcept = default;
        ImageAttributes& operator=(const ImageAttributes& that) noexcept = default;
        ImageAttributes& operator=(ImageAttributes&& that) noexcept = default;
    };
    


    //~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //  PUBLIC MEMBER FUNCTIONS
    //~~~~~~~~~~~~~~~~~~~~~~~~~


    //                      //                             //
    //                      //  Internal State Accessors   //
    //                      //                             //
    
    //Checks to see if this object had to reset itself with
    //its default universal back-up image
    bool isDefaultImage() const noexcept;
    
    //Returns the attributes describing how to interpret this objects
    //stored array of data as an image
    ImageAttributes getAttributes() const noexcept;

    //Get Each ImageAttribute Individually

    //Returns the width of this object's internal image data
    GLsizei width() const noexcept;

    //Returns the height of this object's internal image data
    GLsizei height() const noexcept;

    //Returns the number of components comprise each pixel
    //in this image
    GLsizei components() const noexcept;


    //Returns the OpenGL enum type corresponding to the
    //internal format the image data is in. This value will 
    //need to be provided to the OpenGL API when allocating 
    //storage for this imageData so it can be used as a 
    //texture. These GLenum values are determined based 
    //upon the number of components the image's pixels use
    //as follows:
    //    [Components]      |       [Internal Format]     
    //         1            |            GL_R8            
    //         2            |            GL_RG8           
    //         3            |           GL_RGB8    (and maybe GL_SRGB8 ?)   
    //         4            |           GL_RGBA8  (and maybe GL_SRGB8_ALPHA8 ?)
    GLenum internalFormat() const noexcept;               

    //Returns the expected color-component ordering of this
    //objects data so that OpenGL's functions which allow 
    //texture data to be supplied to the GPU know how they
    //should interpret the data.
    GLenum externalFormat() const noexcept;
    
    //Will return the type used internally to represent the
    //data, which will always be GL_UNSIGNED_BYTE.
    GLenum dataRepresentation() const noexcept;
    

    //Returns a pointer to the start of the stored internal data.
    //This pointer is necessary for sending data to GPU with
    //OpenGL's C API. As such it is impossible for this object to
    //know if you modify the data in any way through this pointer.
    //So please don't do that.
    uint8_t* data() noexcept;


    //Provides the safest way to have this object transfer its internal
    //data to OpenGL. This object handles all the tricky parameter matching
    //while also attempts to match the preferred format types preferred by 
    //the implementation to automatically achieve optimum performance
    void uploadDataTo2DTexture(GLuint textureName, 
                               GLint level = 0) const noexcept;


    //                      //                             //
    //                      //  Internal State Modifiers   //
    //                      //                             //

    //Modifies the way this object reports its internal data 
    //with regard to whether the first byte is assigned to the
    //red or blue channel. While this object will try to get this
    //correct on its own, it is possible to swap the red and 
    //blue channels using this function.
    //Calling this function on an image with less than 3 components
    //has no effect.
    //
    // Return Value Meanings
    //
    //    TRUE   -->  Blue channel is represented by the first byte
    //                and Red channel is represented by the third byte.
    //
    //   FALSE   -->  Red channel is represented by the first byte
    //                and Blue channel is represented by the third byte.
    bool swapRedAndBlueChannels() noexcept;

    


    //  +---------------------------------------------------------------------+
    //  |                     BOILERPLATE HOUSE KEEPING                       |
    //  +---------------------------------------------------------------------+
    
    //~~~~~~~~~~~~~~~~
    //  DESCTRUCTOR
    //~~~~~~~~~~~~~~
    
    ~ImageData_UByte() noexcept;
    
    //~~~~~~~~~~~~~~~~~~~
    //  Copy Operations
    //~~~~~~~~~~~~~~~~~
    
    ImageData_UByte(const ImageData_UByte& that) noexcept;
    ImageData_UByte& operator=(const ImageData_UByte& that) noexcept;
    
    //~~~~~~~~~~~~~~~~~~~
    //  Move Operations
    //~~~~~~~~~~~~~~~~~
    
    ImageData_UByte(ImageData_UByte&& that) noexcept;
    ImageData_UByte& operator=(ImageData_UByte&& that) noexcept;
    
private:
    //The implementation is hidden
    class ImageDataImpl;
    std::unique_ptr<ImageDataImpl> pImpl_;



public:  
    //      ~~~~~ SSSHH THIS IS SUPPOSED TO BE A SECRET ~~~~~
    //  UTILITY CONSTRUCTOR  --  TAP INTO THE TEXTURING DATAPATH 
    //                           DIRECTLY WITH YOUR OWN CUSTOM DATA.
    //       [Please Use This Wisely]
    //  Ensure that the data you provide meets the
    //  constraints listed for each parameter. 
    //  Failure to satisfy the constraints of any
    //  parameter will result in none of your data being
    //  used and the object reverting to constructing 
    //  itself from the static test pattern assigned 
    //  to these objects when they are default constructed.
    //
    //  To check for failures, an optional extra parameter 
    //  can be passed to this function which is a pointer 
    //  to a std::string object that an error message can 
    //  be written to. 
    ImageData_UByte(GLsizei width,       //Requires  width >= 2
                    GLsizei height,      //Requires height >= 2
                    GLsizei comp,        //Requires 1 <= comp <= 4
                    GLenum internalFormat,     //Must be a valid internal format for 'comp'
                    GLenum externalFormat,     //Must be a valid external format
                    std::vector<uint8_t> data, //Must contain enough data to match attributes
                    std::string* errMsg = nullptr); 
};

#endif //IMAGE_DATA_UBYTE_H_