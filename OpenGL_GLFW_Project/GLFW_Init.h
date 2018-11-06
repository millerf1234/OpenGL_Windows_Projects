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

#include "ProjectParameters.h"
#include "ProjectConstants.h"
#include "GLFW_CallbackFunctions.h"


typedef struct MonitorData {
	int numDetected, activeMonitorNum, width, height, refreshRate;
	GLFWmonitor** monitorArray; 
	GLFWwindow * activeMonitor;
	bool validContext;
} MonitorData;

class GLFW_Init {
public:

	//void glfwGetFramebufferSize(GLFWwindow* window, int* width, int* height);

	GLFW_Init(); 
	~GLFW_Init();

	void openWindowed() { this->openFullScreen = false; }
	void setDefaultMonitor(int monitorNumber) { this->defaultMonitor = monitorNumber; }

	std::shared_ptr<MonitorData> initialize(); //Sets up the window
	void specifyWindowCallbackFunctions(); //Sets up callback functions for the window

	void terminate(); //Ends the window

	//Here is a public function for detecting information about a connected display (undefined behavior if displayNum is not a connected display)
	void detectDisplayResolution(int displayNum, int& width, int& height, int& refreshRate);


private:
	int connectedDisplayCount; //Number of detected connected displays
	GLFWmonitor** monitors;  //Array of pointers to connected displays
	GLFWwindow * mWindow; //The display to be rendering to (might add ability to change displays in the future
	bool resizeable, forwardCompatible; //Configuration option booleans
	int contextVersionMajor, contextVersionMinor, aaSamples, vSyncInterval; //Configuration option integers
	bool contextIsValid; //Will be false if an initialization step fails
	bool openFullScreen; 
	int defaultMonitor;
	int width, height, refreshRate; 
	int pixelWidth, pixelHeight;

	std::shared_ptr<MonitorData> generateDetectedMonitorsStruct();  //Private function to be called to generate the return struct of monitor data at the end of initialize().

};



#endif // GLFW_INIT_H_ 