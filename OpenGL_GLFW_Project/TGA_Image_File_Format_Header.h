
//Based off the example found in the OpenGL Super Bible 7e 
//in Listing 9.37 on pages 460-461.


#pragma once

#ifndef TGA_IMAGE_FILE_FORMAT_HEADER_H_
#define TGA_IMAGE_FILE_FORMAT_HEADER_H_


namespace TGA_INTERNAL {

#pragma pack (push, 1)

    //Based off listing 9.37 of the OpenGL Super Bible 7e textbook
    struct TGA_HEADER {
        unsigned char identsize;     //  Size of the following ID field  
        unsigned char cmaptype;      //  Color map type 0 = none
        unsigned char imagetype;     //  Image type 2 == rgb
        short cmapstart;             //  First entry in palette
        short cmapsize;              //  Number of entries in the palette
        unsigned char cmapbpp;       //  Number of bits per palette entry
        short xorigin;               //  X origin
        short yorigin;               //  Y origin   
        short width;                 //  Width (in pixels)
        short height;                //  Height (in pixels)
        unsigned char bpp;           //  Bits per Pixel
        unsigned char descriptor;    //  Descriptor bits
    };

#pragma pack (pop)

} //namespace TGA_INTERNAL

#endif //TGA_IMAGE_FILE_FORMAT_HEADER_H_