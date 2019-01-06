//File:                     FSMErrorCallback.h
//
//  Defines the callback function to be used with GLFW for reporting errors. This function is
//  to be assigned once per application and will be used for every window/context. Note that this
//  behavior is different from every other available GLFW callback, which are assigned on a per-context
//  or per-window basis. This is the only callback function that does not require GLFW to be initialized 
//  by the function glfwInit() having called.
//
//  Important Note!: The description message parameter will be UTF-8 encoded.
//                     A UTF-8 sequence can only start with values 0xC0 or greater,
//                     so there is no need to worry about possible interference
//                     with ASCII.
//  For more information on UTF-8, see: https://www.cprogramming.com/tutorial/unicode.html
//  
//
// Programmer:              Forrest Miller
// Date:                    December 13, 2018


#pragma once

#ifndef FSM_ERROR_CALLBACK_H_
#define FSM_ERROR_CALLBACK_H_

#include "LoggingMessageTargets.h" 
#include <sstream>

//Callback function for GLFW error reporting. Unlike [almost*] every other GLFW callback function,             
//this function is set globally for the entire library as opposed to on a per-window basis.                               *Joystick and Monitor callbacks are exceptions
namespace FSMEngineInternal {
	inline void FSMErrorCallbackFunction(int error, const char* description) {

		//Build a formated error message printout
		std::stringstream errorMessage;
		errorMessage << "\n\n";
		for (int i = 0; i < 80; i++) {
			errorMessage << "^";
		}
		
		errorMessage << "\n\tGLFW Error " << error << "!\n";
		errorMessage << "\tERROR DESCRIPTION: " << description << "\n";

		for (int i = 0; i < 80; i++) {
			errorMessage << "v";
		}
		errorMessage << "\n\n";

		//Print the message to the ERRLOG
		fprintf(ERRLOG, "%s", errorMessage.str().c_str());
	}
} //namespace FSMEngineInternal



#endif //GL_FRAMEWORK_ERROR_CALLBACK_H_