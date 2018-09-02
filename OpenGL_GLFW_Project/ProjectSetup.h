//This file contains headers that are meant to be included globally only once. 
//There is also some functions which are just Pre-Processor checking

//Created by Forrest Miller on July 16, 2018

#pragma once

#ifndef PROJECT_SETUP_H_
#define PROJECT_SETUP_H_

//If building for debug
#define USE_DEBUG 

//Glad version link: http://glad.dav1d.de/#profile=core&specification=gl&api=gl%3D4.5&api=gles1%3Dnone&api=gles2%3Dnone&api=glsc2%3Dnone&language=c&loader=on
#include "glad.h"


#include "glfw_config.h"  //Not sure if this is explicitly necessary
#include "glfw3.h"


//Use the gl math library 
#include "glm/glm/glm.hpp"  //Well... It works so I am not going to fix it  //see: https://stackoverflow.com/questions/17905794/how-to-setup-the-glm-library-in-visual-studio-2012
//Explicitly include the following as well
#include "glm/glm/gtc/quaternion.hpp"
//#include "glm/glm/gtx/quaternion.hpp"


//FreeType is required for HarfBuzz
#include "ft2build.h"

//HarfBuzz text library  //see: https://harfbuzz.github.io/
#include "HarfBuzz/hb.h"
#include "HarfBuzz/hb-ft.h"  //Font


//Move this to .cpp file?
//#ifndef STB_IMAGE_IMPLEMENTATION
//#define STB_IMAGE_IMPLEMENTATION
//#include "stb_image.h"
//#endif 


#include "LoggingMessageTargets.h"
#include "OS_And_Compiler_Configuration_Check.h"

#ifdef USE_DEBUG 
#include "GL_Context_Debug_Message_Callback_Function.h"
#endif //USE_DEBUG






#endif //PROJECT_SETUP_H_



