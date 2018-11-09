//This file contains headers that are meant to be included globally only once. 
//There is also some functions which are just Pre-Processor checking
//This is also where some 

//Created by Forrest Miller on July 16, 2018

#pragma once

#ifndef PROJECT_SETUP_H_
#define PROJECT_SETUP_H_

#define USE_DEBUG_ 
#define FORCE_DEBUG_CONTEXT_APP_SYNCHRONIZATION_  //Defining this will 

//Glad version link: http://glad.dav1d.de/#profile=core&specification=gl&api=gl%3D4.5&api=gles1%3Dnone&api=gles2%3Dnone&api=glsc2%3Dnone&language=c&loader=on
#include "glad.h" //This one header file handles loading the entire graphics language. I am 'glad' it exists (lol)


#include "glfw_config.h"  //Not sure if this is explicitly necessary
#include "glfw3.h"

   
//Use the gl math library 
#include "glm/glm/glm.hpp"  //Well... It works so I am not going to fix it  //see: https://stackoverflow.com/questions/17905794/how-to-setup-the-glm-library-in-visual-studio-2012
//Explicitly include the following as well
#include "glm/glm/gtc/quaternion.hpp"
//#include "glm/glm/gtx/quaternion.hpp"



//Freetype will be useful for when I get around to implementing a kick-butt glpyh-based text rendering system.
#include "ft2build.h"


//Move this to .cpp file?
//#ifndef STB_IMAGE_IMPLEMENTATION
//#define STB_IMAGE_IMPLEMENTATION
//#include "stb_image.h"
//#endif 


#include "LoggingMessageTargets.h"

#if defined USE_DEBUG_ 
#include "GL_Context_Debug_Message_Callback_Function.h"
static constexpr bool USE_DEBUG = true;
#undef USE_DEBUG_ //To prevent confusion down the road with having 2 'USE_DEBUGs'
#else 
static constexpr bool USE_DEBUG = false;
//This is the function that would be defined in "GL_Context_Debug_Message_Callback_Function.h"
static void GLAPIENTRY printGraphicsContextMessageCallback(GLenum source, 
															GLenum type,
															GLuint id,
															GLenum severity,
															GLsizei length,
															const GLchar* message,
															const void* userParam)  {
	//If not in debug mode, then this function can just have an empty definition. It shouldn't be called anywhere, but there's some set-up code
	//that complains when it is not defined... 
	}
#endif //USE_DEBUG_






#endif //PROJECT_SETUP_H_



