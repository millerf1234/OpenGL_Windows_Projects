
//
//  FILE:                                                  ImageLoadingStrategy.h
//
//  Description:                               Abstract base class that serves as 
//                                       the interface that loaders for specific 
//                                      formats can derive themselves from this.
//

#pragma once

#ifndef IMAGE_LOADING_STRATEGY_H_
#define IMAGE_LOADING_STRATEGY_H_

#include <string>

namespace ImageLoadingInternal {

    class ImageLoadingStrategy {
    public:
        ImageLoadingStrategy();
        ~ImageLoadingStrategy() noexcept;


    protected:
        bool mValid_;
        std::string mErrorMessage_;

    };

} //namespace ImageLoadingInternal

#endif IMAGE_LOADING_STRATEGY_H_