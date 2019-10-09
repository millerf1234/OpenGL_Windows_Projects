//File:                                                        RenderDemoBase.h
//Class:                                                         RenderDemoBase
//Namespace:                                                                N/A 
//Programmer:                                                    Forrest Miller
//Dates:                                                                       
//             File Created                               10/4/2018            
//          Under Development                       10/4/2018 - Present        
//                                                                           
//  Description:    This file contains the definition for the abstract base    
//                  class RenderDemoBase. RenderDemoBase provides a common     
//                  public interface built to allow for a standardized 3-step  
//                  invocation process for loading and running a RenderDemo    
//                  project. This allows for any Application using RenderDemo  
//                  objects to be able to invoke any demo without requiring any
//                  modification to the invocation (well, aside from selecting 
//                  the name of the RenderDemo object to construct). 
//                                                            
//                  There are 3 key pieces of functionality used within every  
//                  render demo project, which are:
//                          (i)  Construction 
//                         (ii)  Loading of Assets (including building shaders)
//                         (iii) Run Demo  
//  
//           [But Wait, There is More!]
//                  This class also contains some core shared functionality that 
//                  is general purpose enough to be worth sharing between every 
//                  RenderDemo project.
//
//Notes:			-All Render Projects inheriting from this class must implement the two required
//					 functions. It is meant to be assumed that the calling application will construct 
//				     an instance of an object derived from this class, will check for error, then will
//                   call load(), check for errors and then will call run(). It is up to objects derived
//					 from this class to handle all of the user's input (except for maybe window resizing?)
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

//Global RenderDemo utilities
#include "ScreenCaptureAssistant.h"
#include "JoystickStatePrinter.h"
#include "FramebufferPreferredUsage.h"

enum class PIPELINE_PRIMITIVE_INPUT_TYPE {POINTS,
                                          DISCRETE_TRIANGLES,
                                          TRIANGLE_STRIP,
                                          TRIANGLE_FAN,
                                          LINE,
                                          TRIANGLE_OUTLINE,
                                          LINE_STRIP }; 


class RenderDemoBase {
public:

	//Deleted Functionality 
	RenderDemoBase() /*= delete */; //No Longer Deleted
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
	//Current GPUs and graphics drivers are not yet at the level of robustness compared with
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
	//Checks to see if an error has occurred within the RenderDemo object. Returns true
	//if an error has occurred and false otherwise.
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

	GLFWwindow* mainRenderWindow; //Pointer to target rendering window (Application's running this demo
    //                              must provide this)

    //                  ------------[NOT YET IMPLEMENTED]------------
    //Is able to take screenshots of the current render window output. Only call from a
    //thread that has the active context first.
    ScreenCaptureAssistant screenshotAssistant;
    //


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
    void markMainRenderWindowAsReadyToClose() const noexcept; 

private:

	bool mJoystickStatePrintingEnabled_;
	uint64_t mIterationsSinceLastJoystickStatePrintingLastModified_; //Please rename this variable when less tired and can think...
    float mWindowOpaqueness_; //Window Opaqueness disabled for Fullscreen windows
	JoystickStatePrinter joystickPrinter;

    std::unique_ptr<FramebufferPreferredUsage> mFBInfo_;

	void doJoystickPrinterLoopLogic();

    //Return value signals what should happen. 0 means nothing, positive means increase,
    //and negative means decrease
    int checkIfShouldModifyWindowOpacity() const noexcept;
    void increaseWindowOpacity() noexcept;
    void decreaseWindowOpacity() noexcept;
};



#endif //RENDER_DEMO_BASE_H_