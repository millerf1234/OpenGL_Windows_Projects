//
//  File:                                                            TGAImage.h
//
//  Description:                    Inputs a TGA file at construction and wraps
//                                                        the files image data
//
//                                           Written to meet the C++17 standard
//
//  Dependencies:    TGA SDK  --  "a free and small library to read TGA images"
//                    https://github.com/paulusmas/TGA
//
//  Forrest Miller
//  July 20, 2019
//

#pragma once

#ifndef TGA_IMAGE_H_
#define TGA_IMAGE_H_

#include <memory>
#include <string_view>
#include <filesystem>

class TGAImage final {
public:
    
    //~~~~~~~~~~~~~~~~~~
    //  CONSCTRUCTORS
    //~~~~~~~~~~~~~~~~
    
    //Constructs object to contain an image of size 2x2 with a 
    //test pattern of red-green-blue corners and full alpha
    TGAImage();
    //Loads the data from the tgaFile and stores it internally.
    //Object will have an image of size 2x2 if data
    //acquisition fails for any reason. This 2x2 image has a 
    //test pattern of:
    //                    B  G      [Assuming data is in BGRA8 format]
    //                    G  R  
    //plus full alpha at each value.
    explicit TGAImage(const std::filesystem::path& tgaFilepath);
    
    
    
    //~~~~~~~~~~~~~~~~
    //  DESCTRUCTOR
    //~~~~~~~~~~~~~~
    
    ~TGAImage() noexcept;
    
    
    
    //~~~~~~~~~~~~~~~~~~~
    //  Move Operations
    //~~~~~~~~~~~~~~~~~

    TGAImage(TGAImage&& that) noexcept;
    TGAImage& operator=(TGAImage&& that) noexcept;



    //~~~~~~~~~~~~~~
    //  Interface
    //~~~~~~~~~~~~
    
    //Returns a string_view to this object's internal error log that 
    //provides details regarding any possible issues or failures 
    //this object has experienced. An empty string will be returned 
    //if no issues or errors have occurred.
    std::string_view errorReportString() const noexcept;
    
    //Returns the image's width
    int width() const noexcept;

    //Returns the image's height
    int height() const noexcept;

    //Returns the number of components per pixel the data uses.
    int components() const noexcept;

    //Gives access to the loaded image data
    std::vector<uint8_t>& dataVector() noexcept;
    
private:
    class TGAImageImpl;
    std::unique_ptr<TGAImageImpl> pImpl_;
};

#endif //TGA_IMAGE_H_

