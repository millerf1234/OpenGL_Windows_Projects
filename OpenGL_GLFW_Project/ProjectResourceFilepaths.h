//
// This is just a seperate header file containing all of the filepaths to resources that are used by this project.
// Since this project is currently intended for Windows only, these filepaths can be pretty simple.
//
//  Created by Forrest Miller on July 25, 2018 


#pragma once

#ifndef PROJECT_RESOURCE_FILEPATHS_H_
#define PROJECT_RESOURCE_FILEPATHS_H_


#include "ProjectSetup.h" //To get access to GL types

///////////////////////////////
//  MODELS 
///////////////////////////////

#include "ModelFilepaths.h"  //Models get their own file


///////////////////////////////
//  FONTS
///////////////////////////////

static const char * font_path = "Fonts\\Roboto-Black.ttf";




///////////////////////////////
/////  Shaders
///////////////////////////////

static const GLchar * VERT_PASSTHROUGH2D = "passthrough2D.vert";
static const GLchar * VERT_PASSTHROUGH3D = "passthrough3D.vert";
static const GLchar * FRAG_CONSTANTCOLOR = "constantcolor.frag";





#endif //PROJECT_RESOURCE_FILEPATHS_H_
