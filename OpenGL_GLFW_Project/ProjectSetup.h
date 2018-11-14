//This file contains headers that are meant to be included globally only once. 
//There is also some functions which are just Pre-Processor checking
//This is also where some 

//Created by Forrest Miller on July 16, 2018
// Reorganized document on November 8, 2018. It is still pretty disorganized...

#pragma once

#ifndef PROJECT_SETUP_H_
#define PROJECT_SETUP_H_

#include "ProjectParameters.h"  //This header contains project configuration which will affect setup




//Glad version link: http://glad.dav1d.de/#profile=core&specification=gl&api=gl%3D4.5&api=gles1%3Dnone&api=gles2%3Dnone&api=glsc2%3Dnone&language=c&loader=on
#include "glad.h" //This one header file handles loading the entire graphics language. I am 'glad' it exists (lol)





//#include "glfw_config.h"  //Not sure if this is explicitly necessary
#include "glfw3.h"




//////////////////////////////
//  Set up the GLM library  //
//////////////////////////////
#ifndef GLM_FORCE_SSE2
#define GLM_FORCE_SSE2
#endif //GLM_FORCE_SSE2
#ifndef GLM_FORCE_ALIGNED
#define GLM_FORCE_ALIGNED
#endif //GLM_FORCE_ALIGNED

//Include the main GLM library
#include "glm/glm.hpp"  

//After including the main GLM header, it is still necessary to include
//some additional GLM extensions that are used in this project:

//'type_ptr' is a conversion function that allows GLM vector and GLM 
// matrix types to be accessed as though they are just arrays through
// a GLfloat* to their first element. This is necessary for sending them
// as uniforms to shader programs.
#include "glm/gtc/type_ptr.hpp"   

//'quaternion' is a special data type used to efficently and cleanly express
// rotations in 3D. They are both cheaper and more robust than relying on Euler angles.
#include "glm/gtc/quaternion.hpp" 

//'matrix_transform' provides functions for creating several useful matrices that 
// are used within the course of rendering. 
#include "glm/gtc/matrix_transform.hpp"



//Freetype will be useful for when I get around to implementing a kick-butt glpyh-based text rendering system.
#include "ft2build.h"


#include "AL/al.h"


//Move this to .cpp file?
//#ifndef STB_IMAGE_IMPLEMENTATION
//#define STB_IMAGE_IMPLEMENTATION
//#include "stb_image.h"
//#endif 


#include "LoggingMessageTargets.h"


#if defined USE_DEBUG_ 
#include "GL_Context_Debug_Message_Callback_Function.h"
static constexpr bool USE_DEBUG = true;
#if defined FORCE_DEBUG_CONTEXT_APP_SYNCHRONIZATION_
static constexpr bool FORCE_SYNC_BETWEEN_CONTEXT_AND_APP = true;
#else 
static constexpr bool FORCE_SYNC_BETWEEN_CONTEXT_AND_APP = false;
#endif //FORCE_DEBUG_CONTEXT_APP_SYNCHRONIZATION_
#else 
static constexpr bool USE_DEBUG = false;
static constexpr bool FORCE_SYNC_BETWEEN_CONTEXT_AND_APP = false;
//This is the function that would be defined in "GL_Context_Debug_Message_Callback_Function.h"
static void GLAPIENTRY printGraphicsContextMessageCallback(GLenum source, 
															GLenum type,
															GLuint id,
															GLenum severity,
															GLsizei length,
															const GLchar* message,
															const void* userParam)  {
	//If not in debug mode, then this function can just have an empty definition. It shouldn't be called anywhere, but there's some set-up code
	//that complains when this function is not defined... 
	}
#endif //USE_DEBUG_



#endif //PROJECT_SETUP_H_



