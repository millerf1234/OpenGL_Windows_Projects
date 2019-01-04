//File:                            ApplicationConstantSettings.h
//
//Preamble to description:         Really at this point the number of Constant Settings/Parameter Settings/Global Constants/FilepathFolderLocations
//                                  has gotten quite disorganized. The goal it to eventually have something like XML parsing (or even just .ini parsing)
//                                  set up so that configuration and settings can be loaded from a file rather than requiring direct compilation into the
//                                  project. 
// 
//Description:                     This file defines some Application specific constants. The difference between this file and the other header with constant
//                                  values is this file sets Application-specific configuration constants (for example, what OpenGL version to use) while the
//                                  other file defines project-code related constants (like the value of PI).  
//
// Intended Usage:                This file should only need to be included with Application.h 
//                                  Any constant value needed beyond Application.h should go in a different constants header file. 
//
// Programmer:                    Forrest Miller
// Date:                          December 13, 2018


#pragma once

#ifndef APPLICATION_CONSTANT_SETTINGS_H_
#define APPLICATION_CONSTANT_SETTINGS_H_

#include "LoggingMessageTargets.h"
#include "GlobalIncludes.h"



#define USE_DEBUG_
#define FORCE_DEBUG_CONTEXT_APP_SYNCHRONIZATION_


static constexpr const char * NAME_OF_APPLICATION = "OpenGL Render Demo";


//OpenGL Version:
static constexpr const int DEFAULT_OPENGL_VERSION_MAJOR = 4;
static constexpr const int DEFAULT_OPENGL_VERSION_MINOR = 5;



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
														   const void* userParam) {
	//If not in debug mode, then this function can just have an empty definition.
	//This function shouldn't be called when not debugging, but there is some set-up code that
	//complains when this function is not defined... 
}
#endif //USE_DEBUG_


#endif //APPLICATION_CONSTANT_SETTINGS_H_