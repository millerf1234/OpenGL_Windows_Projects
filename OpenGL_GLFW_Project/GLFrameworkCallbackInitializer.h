//  File:                      GLFrameworkCallbackInitializer.h
//
//  Description:               Manages setting up callback functions for the 
//                             GLFW library
//
//  
//
// Defines several functions which manage ensuring that all of
//                             the necessary callback functions have each of their requirements
//                             initialized and that each callback gets assigned to GLFW. 
//
//  Programmer:                Forrest Miller
//  Date:                      January 2, 2019


#pragma once

#ifndef GL_FRAMEWORK_CALLBACK_INITIALIZER_H_
#define GL_FRAMEWORK_CALLBACK_INITIALIZER_H_

#include "GLFrameworkErrorCallback.h"
#include "GLFrameworkMonitorEventCallback.h"

namespace FSMEngineInternal {

	//To be called before GLFW is initialized
	//Expected Callbacks functions to set:
	//   -) Error callback function
	void configurePreInitializationCallbackFunctions();


	//To be called soon after GLFW has returned from a successful 
	//initialization.
	//Expected Callback functions to be set:
	//   -) Monitor Connection Event Callback
	//   -) Joystick Connected Event Callback 
	void configurePostInitializationCallbackFunctions();


	//Once a render window has been created, it almost certainly will  
	//be desired to have both the necessary and unnecessary callbacks 
	//assigned to the window so that it behaves properly.
	//These are the callbacks that will be assigned to the window:

	void configureWindowSpecificCallbackFunctions(GLFWwindow*);
	





} //namespace FSMEngineInternal

#endif //GL_FRAMEWORK_CALLBACK_INITIALIZER_H_
