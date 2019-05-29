//File:       RenderDemoBase.h
//Class:      RenderDemoBase
//Namespace:  N/A 
//Programmer: Forrest Miller
//Date(s):    10/4/2018
//
//  Description:    This file contains the definition for the abstract base 
//                  class RenderDemoBase. RenderDemoBase provides an interface 
//					that can be called by an application to reduce the amount of
//					code duplication within an application that may run multi RenderDemos.
//					There are 3 key pieces of functionality used within every render demo 
//					project, which are:
//										(i)   Construction 
//									    (ii)  Loading of Assets (including shader compilation)
//										(iii) Run Demo  
//					
//			
//Notes:			-All Render Projects inheriting from this class must implement the two required
//					 functions. It is meant to be assumed that the calling application will construct 
//				     an instance of an object derived from this class, will check for error, then will
//                   call load(), check for errors and then will call run(). It is up to objects derived
//					 from this class to handle all of the user's input (except for maybe window resizing)
//                   and to provide an exit condition for the program. It is not yet known whether this 
//				     exit will constitute the termination of the entire application or if the application
//					 will provide a menu system to switch between Render Demos.  
//					 
//
//
//Errors to watch for:  Derived classes should play things safe and check to make sure the Application
//						holding them provides them with valid parameters (this mostly means make sure 
//						the pointer to the render target window is valid/not-NULL). 
//
//

#pragma once

#ifndef RENDER_DEMO_BASE_H_
#define RENDER_DEMO_BASE_H_

#include <iostream>  //fprintf 

#include "GlobalIncludes.h"         //For GLFWwindow*
#include "GLFW_Init.h"              //For the MonitorData struct
//#include "MissingAsset.h"         //For assets that are missing

#include "JoystickStatePrinter.h"

enum class PIPELINE_PRIMITIVE_INPUT_TYPE {POINTS, DISCRETE_TRIANGLES, TRIANGLE_STRIP, TRIANGLE_FAN, LINE, TRIANGLE_OUTLINE, LINE_STRIP }; 

class RenderDemoBase {
public:

	//Deleted Functionality 
	RenderDemoBase() /*= delete */;
	RenderDemoBase(const RenderDemoBase&) = delete;
	RenderDemoBase& operator=(const RenderDemoBase&) = delete;

	//Constructor and Destructor
	//RenderDemoBase(GLFWwindow * renderWindow);
	virtual ~RenderDemoBase() noexcept;
	

	//-----------------------------------
	//		Abstract Functions to override
	//----------------------------------- 

	virtual void loadAssets() = 0;
	virtual void run() = 0;

	//------------------------------------
	//  Useful Universal RenderDemo Functions
	//------------------------------------

	//(Based off advice in OpenGL Super Bible 7e Chapter 15: Debugging and Stability) [Pages 737-740]
	//Current GPUs and graphics drivers are not yet at the level of robustness comapred with
	//modern CPUs and operating systems when it comes to dealing with rouge/misbehaving processes.
	//In the event that an errant task begins hogging all of the GPUs resources, about the best that
	//can be done is to kick everything off the GPU and reset everything. Since the GPU is a shared-resource,
	//this means that other processes can cause the GPU to hang, requiring a context reset. Since a context
	//reset erases all state from the GPU, it is up to all applications that were using the GPU to recreate all
	//of their lost data. 
	//In the event a context reset occurs, all rendering will cease and all assets that were stored within the 
	//GPU's memory are lost. None of the OpenGL functions will work properly except for the functions ... (to be further researched...)
	//  http://discourse.glfw.org/t/glfw-context-robustness-window-attribute/643
	//
	//This function should be called around once a frame.
	bool checkForContextReset();


	/*  This whole part is unnecessary
	//-----------------------------------
	//		Some utility functions
	//-----------------------------------
	//Checks to see if an error has occured within the RenderDemo object. Returns true
	//if an error has occured and false otherwise.
	inline bool checkIfRenderDemoError() const { return mRenderDemoError; }

	//Checks to see if the Render Demo object has successfully loaded all of the 
	//pieces it needs to have its 'run()' member function called.
	inline bool checkIfRenderDemoLoaded() const { return mRenderDemoLoaded; }
	*/

protected:

    float counter;

	//std::vector<MissingAsset> missingAssets;
	/*
	bool mRenderDemoError;
	bool mRenderDemoLoaded;
	*/

	GLFWwindow * mainRenderWindow; //Pointer to target renderable window (Application must provide this)

	//Performs input-handling and logic configuration on the shared members of every RenderDemo object.
	//This function should be called as part of the render loop.
	void performRenderDemoSharedInputLogic(); 
	
	// Marks the close flag for the RenderDemo's Main Window. The actual closing of the window
	//must be handled elsewhere.
	//  [Class Implementation Detail]  (i.e. expectation on how this function impacts derived classes)   
	//     If this function is called before the render loop is first
	//       initiated, the render loop will just instantly exit once
	//       reached. Otherwise, if this function is called from within a
	//       render loop iteration, the remaining parts of the current
	//       iteration will be performed.
	void markMainRenderWindowAsReadyToClose() const { glfwSetWindowShouldClose(mainRenderWindow, true); }


private:
	bool mJoystickStatePrintingEnabled_;
	unsigned long long mIterationsSinceLastJoystickStatePrintingLastModified_; //Please rename this variable when less tired and can think...
	JoystickStatePrinter joystickPrinter;

	void doJoystickPrinterLoopLogic();
};



#endif //RENDER_DEMO_BASE_H_