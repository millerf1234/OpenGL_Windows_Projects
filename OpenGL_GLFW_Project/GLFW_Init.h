//  See .cpp file for more info
//
//  GLFW_Init.h
//
//  Created by Forrest Miller on 2/12/18.
//  Repurposed for Windows project on 7/16/2018
//

#pragma once

#ifndef GLFW_INIT_H_
#define GLFW_INIT_H_

#include <iostream>
#include <sstream>

#include <cstdlib>  //For a std::atexit function that ensures glfwTerminate is called

#include "GLFW_Init_Report.h"

#include "GlobalConstants.h"
#include "PrimaryWindowCallbackFunctions.h"

#include "WindowConfiguration.h"
#include "ApplicationConstantSettings.h"


namespace GLFW_INIT_INTERNAL {
    static bool GLFW_IS_INIT = false;
    inline void atExitFuncToEnsureGLFWTerminateIsCalled() noexcept {
        if (GLFW_IS_INIT) {
            glfwTerminate();
            GLFW_IS_INIT = false;
        }
    }
} //namespace GLFW_INIT_INTERNAL


class GLFW_Init {
public:
	GLFW_Init(); 
	~GLFW_Init() noexcept ;

	void openWindowed() { this->openFullScreen = false; }
	void setDefaultMonitor(int monitorNumber) { this->defaultMonitor = monitorNumber; }

	std::unique_ptr<InitReport> initialize(); //Sets up the window
	void specifyWindowCallbackFunctions(); //Sets up callback functions for the window
	void setWindowUserPointer(void * userPointer); //Sets a custom user pointer for the window

	void terminate(); //Ends the window

	//Here is a public function for detecting information about a connected display (undefined behavior if displayNum is not a connected display)
	void detectDisplayResolution(int displayNum, int& width, int& height, int& refreshRate);


private:
	int connectedDisplayCount; //Number of detected connected displays
	GLFWmonitor** monitors;  //Array of pointers to connected displays
	GLFWwindow * mWindow; //The display to be rendering to (might add ability to change displays in the future
	bool decoratedBorder, resizeable, forwardCompatible; //Configuration option booleans
    int contextResetStrategy;
	int contextVersionMajor, contextVersionMinor, aaSamples, vSyncInterval; //Configuration option integers
	bool openFullScreen; 
	int defaultMonitor;
	int width, height, refreshRate; 
	//int pixelWidth, pixelHeight;

	//Window attachment is created and has its allocation owned by this class
	//std::unique_ptr<WindowCallbackInternal::CallbackStreamBuffer> windowAttachment;

    void assignAtExitTerminationFunction() noexcept;

	std::unique_ptr<InitReport> generateDetectedMonitorsStruct();  //Private function to be called to generate the return struct of system data at the end of initialize().

};



#endif // GLFW_INIT_H_ 