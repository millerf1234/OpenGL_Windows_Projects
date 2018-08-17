//This class represents a texture to be used within OpenGL.
//Very simple currently, not a whole lot of image formats supported
//and not designed for maximum streamlined efficiency.
// or....
// Could have this class just be a wrapper for the data (and the ImageFileLoader) and
// have all of the OpenGL textures created and managed by 1 Object

//Currently only supports loading JPEG and PNG files

//Created by Forrest Miller on 8/17/2018


#pragma once

#ifndef TEXTURE_H_
#define TEXTURE_H_

#include "ImageFileLoader.h"
#include "ProjectConstants.h"
#include "ProjectParameters.h"

class Texture {
public:



private:


};

#endif //TEXTURE_H_
