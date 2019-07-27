//
//  File:                                    ImageDataLoader.h
//
//  Meant to serve as an extra layer of abstraction between 
//  how image loading is actually implemented and how the data
//  is acquired by the App.
//
//  Basically the idea is the Application should only ever have
//  to deal with the 'ImageData' class, and all of the loading 
//  happens behind the scenes. The implementation for the 
//  ImageData class uses a local instance of this class to 
//  interface with a data loader.
//

#pragma once

#ifdef IMAGE_DATA_LOADER_H_
#define IMAGE_DATA_LOADER_H_

namespace ImageLoadingInternal {

    class ImageDataLoader final {


    };


} //namespace ImageLoadingInternal

#endif //IMAGE_DATA_LOADER_H_
