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
//                              for working with textures and image files. Each
//                                 instance of this class contains internally a
//                             valid block of image data in the format of 8-bit
//                                 unsigned bytes (internally represented using
//                             the data type 'uint8_t').                       
//                                                                             
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
// Programmer:                                   Forrest Miller                
// Date:                                         July 2019                     
//                                                                             

#pragma once

#ifndef IMAGE_DATA_UBYTE_H_
#define IMAGE_DATA_UBYTE_H_

#include <memory>
#include <filesystem>
#include <functional>
#include "GlobalIncludes.h"    //For including 'glad.h' 


///////////////////////////
//       Constants       //
///////////////////////////

//Image Dimensions of static image assigned to objects constructed by
//calling the default constructor
static constexpr const GLsizei DEFAULT_STATIC_IMAGE_HEIGHT = 8;
static constexpr const GLsizei DEFAULT_STATIC_IMAGE_WIDTH = 8;
//Image Dimensions of dynamically generated image data
static constexpr const GLsizei DEFAULT_GENERATED_IMAGE_HEIGHT = 64;
static constexpr const GLsizei DEFAULT_GENERATED_IMAGE_WIDTH = 64;


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
    //breakdown of the various data paths looked like, so I 
    //created a way to print details looking into how the 
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
    //information for how to interpret the stored image data]
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
                                            comp(comp) { assert(comp <= 4); }
        ~ImageAttributes() = default;
        ImageAttributes(const ImageAttributes& that) noexcept = default;
        ImageAttributes(ImageAttributes&& that) noexcept = default;
        ImageAttributes& operator=(const ImageAttributes& that) noexcept = default;
        ImageAttributes& operator=(ImageAttributes&& that) noexcept = default;
    };
    
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //  PUBLIC MEMBER FUNCTIONS
    //~~~~~~~~~~~~~~~~~~~~~~~~~
    //Returns the attributes describing how to interpret this objects
    //stored array of data as an image
    ImageAttributes getAttributes() const noexcept;

    
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
    ImageData_UByte(GLsizei width,
                    GLsizei height,
                    GLsizei comp,
                    std::string* errMsg = nullptr) 
};

#endif //IMAGE_DATA_UBYTE_H_