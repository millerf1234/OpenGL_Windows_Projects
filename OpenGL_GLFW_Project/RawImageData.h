//
//  FILE:                                                            RawImageData.h
//
//  I am not sure yet if I am going to proceed with an implementation of the public
//  class ImageData based around this at its core or if I will simply restrict the
//  possible ImageData formats to only unsigned bytes (i.e. 'uint8_t'). It looks like
//  traditionally stb_image would automatically convert all images to 8-bit, but in
//  the past few years support has been added for newer image types.
//  
//  The overarching idea is as follows:
//  (i)  Have the class 'ImageData' be the public facing High-Level interface
//         that all other classes must go through to load, store, or copy images.
//         This class will simply encompass some ImageData, actually uploading the 
//         data to OpenGL is beyond the scope of ImageData.
//  (ii) Since Images can be really complicated internally due to many different 
//         possible combinations of various components, data-representations and color
//       blah blah blah
//         basically the plan is to stick all of the tricky difficult code in here in 
//         its own class which will be much less likely to change over time than the
//         public interface class.
//
//  
//  Forrest Miller
//  July 26, 2019
//

#pragma once

#ifndef RAW_IMAGE_DATA_H_
#define RAW_IMAGE_DATA_H_

#include <vector>
#include "GlobalIncludes.h" //for 'glad.h' etc

namespace ImageDataInternal {

    enum class ImageDataFormat { ui8, ui16 };
    union RawData {
        mutable std::vector<uint8_t> uint8Data;
        mutable std::vector<uint16_t> uint16Data;
    };

    struct RawImgData {
        const ImageDataFormat format;
        const RawData data;
    };



    class RawImageData final {
    public:
        RawImageData() = delete;
        RawImageData(ImageDataFormat format);

        ~RawImageData() noexcept = default;
        

        const RawImgData mData_;
    };


}
#endif //RAW_IMAGE_DATA_H_

