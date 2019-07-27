//
//  FILE:                                                           ImageData.h
//
//    (This class is very straightforward though, the only
//     thing to be aware of is that this class supports 
//     construction with either loading image data from a file
//     or generating its own data programmatically. See constructors
//    for details)
//
//  Ideally in the future this class will have its responsibilities
//  divided up into several distinct classes since it currently 
//  wears multiple hats.
//
//      It is perhaps needless to say that the number of different
//  options available when dealing with images, image-file-formats,
//  and OpenGL's (and perhaps other 3D API's) texture API, it is 
//  quite easy to get overwhelmed. Fortunately it is quite possible
//  to accomplish most tasks through a small subset of the data. This
//  is accomplished through class invariants.
//  
//  
//  
//  
//   
//  
//  July 24, 2019

#pragma once

#ifndef IMAGE_DATA_H_
#define IMAGE_DATA_H_

#include <memory>
#include <vector>
#include <filesystem>
#include "GlobalIncludes.h"   //glad.h for OpenGL 

static constexpr const GLsizei DEFAULT_STATIC_IMAGE_HEIGHT = 8;
static constexpr const GLsizei DEFAULT_STATIC_IMAGE_WIDTH = 8;


static constexpr const GLsizei DEFAULT_GENERATED_IMAGE_HEIGHT = 64;
static constexpr const GLsizei DEFAULT_GENERATED_IMAGE_WIDTH = 64;

class ImageData final {
public:
    //Creates an ImageData object containing a basic  image 
    //of a static test checkerboard pattern [unless during
    //development the size is changed
    ImageData();



    //Creates an 
    //Image Generation algorithm that creates a texture of
    //size DEFAULT_GENERATED_IMAGE_HEIGHT by DEFAULT_GENERATED_IMAGE_WIDTH
    //(defined above). This algorithm is seeded with values
    //that are updated as the algorithm runs, causing it to 
    //produce different outcomes each time it is called.


    //Upload your own image data. Or just specify a width and
    //height to get this object to use its generator to generate
    //itself with texture data of  size
    ImageData(std::string name,
              GLsizei width = DEFAULT_GENERATED_IMAGE_WIDTH,
              GLsizei height = DEFAULT_GENERATED_IMAGE_HEIGHT,
              GLenum format = GL_BGRA,
              GLenum internalFormat = GL_RGBA8,
              std::vector<uint8_t>* data = nullptr);

    //This Constructor Takes parameters corresponding directly 
    //to each member field in this object. Basically the idea is
    //to have a constructor that allows for every field of this
    //object to be set directly. 
    // NOTE: This is not meant to serve as part of the primary
    //       constructor interface for this object. This is more
    //       for allowing an easy access point straight into the
    //       texturing functionality of the renderer. 
    //[todo] (maybe)  Validate all parameters to make sure they
    //                make sense (i.e. GLenums are of the 
    //                appropriate type, the vector size matches 
    //                width*height*components, 
    ImageData(bool valid,
        std::string errorMessage,
        std::string name,
        GLsizei width,
        GLsizei height,
        GLint components,
        GLenum dataType,
        GLenum format,
        GLenum internalFormat,
        std::vector<uint8_t> data);

    //Please ensure the image file has a proper file extension which matches 
    //one of the expected formats.Accepted formats currently are: [None] 
    ImageData(const std::filesystem::path& imageFile);

    ~ImageData();
    ImageData(const ImageData& that) noexcept;
    ImageData(ImageData&& that) noexcept;
    ImageData& operator=(const ImageData& that) noexcept;
    ImageData& operator=(ImageData&& that) noexcept;


    //Interface

    //Check to see if the data contained in this object is acceptable for use
    bool valid() const noexcept { return mValid_; }

    //Will possibly contain some information regarding any error that may have
    //gone down while this object was in use. Chances are high that this will 
    //contain some information in the event that the image data fails to be 
    //valid.
    std::string errorMessage() const noexcept { return mErrorMessage_; }

    //Returns the assigned name of this imageData. This name will be internally
    //generated if not specified at construction.
    std::string name() const noexcept { return mName_; }

    GLsizei width() const noexcept { return mFields_.mWidth_; }
    GLsizei height() const noexcept { return mFields_.mHeight_; }
    GLint components() const noexcept { return mFields_.mComponents_; }
    //Returns the type the image data represents
    //GLenum dataType() const noexcept { return mDataType_; }
    //Returns the format of the image data
    //GLenum format() const noexcept { return mFormat_; }
    ////Returns the internal format of the image data
    //GLenum internalFormat() const noexcept { return mInternalFormat_; }
    //Returns a reference to this object's internal data vector
    //std::vector<uint8_t>& dataVector() noexcept { return mData_; }
    //Returns a pointer to the first element of this object's data vector. Please
    //check 
    //void* dataPtr() noexcept;


private:

    typedef struct GL_FIELDS {
        GLsizei mWidth_;
        GLsizei mHeight_;
        GLint mComponents_;
        GLenum mDataType_;
        GLenum mFormat_;
        GLenum mInternalFormat_;
    } GLFields;
    GLFields mFields_;

    bool mValid_;
    std::string mErrorMessage_;
    std::string mName_;
    ///ImgDataFormat mDataFormat_;
    //This may be a bad idea, but... 
    typedef union AvailableImgDataFormats {
        std::vector<uint8_t> ui8;
        std::vector<uint16_t> ui16;
        std::vector<uint32_t> ui32;

    } ImgData;

    ImgData mData_;
};


#endif //IMAGE_DATA_H_


























#if defined OLD_INCOMPLETE_VERSION_FROM_BEFORE_I_KNEW_DATA_WOULD_ALWAYS_BE_8BIT_UCHAR








#ifndef IMAGE_DATA_H_
#define IMAGE_DATA_H_

#include <memory>
#include <vector>
#include <filesystem>
#include "GlobalIncludes.h"   //glad.h for OpenGL 

static constexpr const GLsizei DEFAULT_GENERATED_IMAGE_HEIGHT = 64;
static constexpr const GLsizei DEFAULT_GENERATED_IMAGE_WIDTH = 64;

class ImageData {
public:

    enum class ImgDataFormat { ui8, ui16, ui32, ui64,  };

    //The default constructor uses an internally-defined
    //Image Generation algorithm that creates a texture of
    //size DEFAULT_GENERATED_IMAGE_HEIGHT by DEFAULT_GENERATED_IMAGE_WIDTH
    //(defined above). This algorithm is seeded with values
    //that are updated as the algorithm runs, causing it to 
    //produce different outcomes each time it is called.
    //Please note that this means it is probably not a good
    //idea to default construct a large number of these objects
    //
    ImageData();

    //Upload your own image data. Or just specify a width and height to get
    //this object to 
    ImageData(std::string name,
              GLsizei width = DEFAULT_GENERATED_IMAGE_WIDTH,
              GLsizei height = DEFAULT_GENERATED_IMAGE_HEIGHT,
              GLenum format = GL_BGRA,
              GLenum internalFormat = GL_RGBA8,
              std::vector<uint8_t>* data = nullptr);
    
    //This Constructor Takes parameters corresponding directly 
    //to each member field in this object. Basically the idea is
    //to have a constructor that allows for every field of this
    //object to be set directly. 
    // NOTE: This is not meant to serve as part of the primary
    //       constructor interface for this object. This is more
    //       for allowing an easy access point straight into the
    //       texturing functionality of the renderer. 
    //[todo] (maybe)  Validate all parameters to make sure they
    //                make sense (i.e. GLenums are of the 
    //                appropriate type, the vector size matches 
    //                width*height*components, 
    ImageData(bool valid,
              std::string errorMessage,
              std::string name,
              GLsizei width,
              GLsizei height,
              GLint components,
              GLenum dataType,
              GLenum format,
              GLenum internalFormat,
              std::vector<uint8_t> data);

    //Please ensure the image file has a proper file extension which matches 
    //one of the expected formats.Accepted formats currently are: [None] 
    ImageData(const std::filesystem::path& imageFile);

    ~ImageData();
    ImageData(const ImageData& that) noexcept;
    ImageData(ImageData&& that) noexcept;
    ImageData& operator=(const ImageData& that) noexcept;
    ImageData& operator=(ImageData&& that) noexcept;


    //Interface

    //Check to see if the data contained in this object is acceptable for use
    bool valid() const noexcept { return mValid_; }

    //Will possibly contain some information regarding any error that may have
    //gone down while this object was in use. Chances are high that this will 
    //contain some information in the event that the image data fails to be 
    //valid.
    std::string errorMessage() const noexcept { return mErrorMessage_; }

    //Returns the assigned name of this imageData. This name will be internally
    //generated if not specified at construction.
    std::string name() const noexcept { return mName_; }

    GLsizei width() const noexcept { return mWidth_; }
    GLsizei height() const noexcept { return mHeight_; }
    GLint components() const noexcept { return mComponents_; }
    //Returns the type the image data represents
    GLenum dataType() const noexcept { return mDataType_; }
    //Returns the format of the image data
    GLenum format() const noexcept { return mFormat_; }
    //Returns the internal format of the image data
    GLenum internalFormat() const noexcept { return mInternalFormat_; }
    //Returns a reference to this object's internal data vector
    std::vector<uint8_t>& dataVector() noexcept { return mData_; }
    //Returns a pointer to the first element of this object's data vector. Please
    //check 
    void* dataPtr() noexcept;


private:
    
    typedef struct GL_FIELDS {
        GLsizei mWidth_;
        GLsizei mHeight_;
        GLint mComponents_;
        GLenum mDataType_;
        GLenum mFormat_;
        GLenum mInternalFormat_;
    } GLFields;


    bool mValid_;
    std::string mErrorMessage_;
    std::string mName_;
    ImgDataFormat mDataFormat_;
    //This may be a bad idea, but... 
    typedef union AvailableImgDataFormats {
        std::vector<uint8_t> ui8;
        std::vector<uint16_t> ui16;
        std::vector<uint32_t> ui32;

    } ImgData;

    ImgData mData_;
};





#endif //IMAGE_DATA_H_

#endif //OLD_INCOMPLETE_VERSION_YADA_YADA