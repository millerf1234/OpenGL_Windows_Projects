//File:  GLFW_CallbackFunctions.h
//
// Description:   Defines callback functions that are provided to GLFW to process events
//
// Programmer:   Forrest Miller
// Date:         November 5, 2018

#pragma once

#ifndef GLFW_CALLBACK_FUNCTIONS_H_
#define GLFW_CALLBACK_FUNCTIONS_H_

#include "ProjectSetup.h"    //To get GLFWwindow*
#include "LoggingMessageTargets.h"

//NOTE: When the window gets resized, both the windowSizeCallback and framebufferSizeCallback functions are called

//---------------------------------------------------------------------------------
//   Window Resize Callback               (this will be called when the window gets resized)
//---------------------------------------------------------------------------------
inline void windowSizeCallback(GLFWwindow* window, int width, int height) {
	fprintf(MSGLOG, "\nGLFW_CALLBACK::Detected window size update! New screen coordinates are %dx%d\n", width, height);
	//OpenGL operates using pixels instead of screen coordinates, which means there is nothing else to do here. GL_Viewport will be updated with framebufferSizeCallback()
}

//---------------------------------------------------------------------------------
//    Framebuffer Size Callback           (this will be called if the window is resized or is moved between different resolution monitors)
//---------------------------------------------------------------------------------
inline void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
	fprintf(MSGLOG, "\nGLFW_CALLBACK::Detected framebuffer size update! New fb is %dx%d pixels\n", width, height);
	glViewport(0, 0, width, height);
}


#endif //GLFW_CALLBACK_FUNCTIONS_H_