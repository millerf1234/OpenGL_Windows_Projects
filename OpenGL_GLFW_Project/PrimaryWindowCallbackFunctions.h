//File:  PrimaryWindowCallbackFunctions.h
//
// Description:   Defines callback functions that are provided to GLFW to process events
//
// Reference:     Many of the available GLFW Callback functions (for input) are documented
//                 at: https://www.glfw.org/docs/latest/group__input.html
//
// Programmer:   Forrest Miller
// Date:         November 5, 2018

#pragma once

#ifndef PRIMARY_WINDOW_CALLBACK_FUNCTIONS_H_
#define PRIMARY_WINDOW_CALLBACK_FUNCTIONS_H_

#include <atomic>
#include <mutex>

#include "ProjectSetup.h"    //To get GLFWwindow*
#include "LoggingMessageTargets.h"
#include "PrimaryWindowState.h"

namespace WindowCallbackInternal {

	

	//There is a global variable here for dropped filepaths. It is unfortunate that this is global, but
	//it seems a less-worse than any other alternatives (such as performing a memory allocation to store
	//the filepaths without knowning when/where the deallocation will occur)
	namespace {
		std::mutex filepathListWriteMutex;
		static std::list<std::string> droppedFilepaths;
	}

	//---------------------------------------------------------------------------------
	//   Window Position Moved Callback               
	//---------------------------------------------------------------------------------
	void windowPositionCallback(GLFWwindow* window, int xPos, int yPos);

	//---------------------------------------------------------------------------------
	//   Window Refresh-Required Callback               
	//---------------------------------------------------------------------------------
	void windowRefreshCallback(GLFWwindow* window);

	//NOTE: When the window gets resized, both the windowSizeCallback and framebufferSizeCallback functions are called
	//---------------------------------------------------------------------------------
	//   Window Resize Callback               (this will be called when the window gets resized)
	//---------------------------------------------------------------------------------
	void windowSizeCallback(GLFWwindow* window, int width, int height);

	//---------------------------------------------------------------------------------
	//    Framebuffer Size Callback           (this will be called if the window is resized or is moved between different resolution monitors)
	//---------------------------------------------------------------------------------
	void framebufferSizeCallback(GLFWwindow* window, int width, int height);

	//---------------------------------------------------------------------------------
	//   Window Focus Gained/Lost Callback               
	//---------------------------------------------------------------------------------
	void windowFocusChangeCallback(GLFWwindow* window, int focused);

	//---------------------------------------------------------------------------------
	//   Joystick Connection Discovered/Lost Callback               
	//---------------------------------------------------------------------------------
	void joystickConnectionCallback(GLFWwindow * window, int joyID, int connected);  //Second parameter is either GLFW_CONNECTED or GLFW_DISCONNECTED


	//Input callbacks:

	void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

	void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

	void mouseScrollCallback(GLFWwindow* window, double x, double y);

	void curserEnterCallback(GLFWwindow* window, int entered);

	void curserPositionCallback(GLFWwindow* window, double x, double y);


	//Callback function for when files are dragged and dropped onto the window. Note that 
	//memory for filepaths is allocated by GLFW and is only guarenteed to exist for the 
	//duration of this function, so a deep copy should be performed. Filepaths are UTF-8 encoded
	void filedropCallback(GLFWwindow* window, int count, const char ** filePaths);





} //namespace WindowCallbackInternal


#endif //PRIMARY_WINDOW_CALLBACK_FUNCTIONS_H_
