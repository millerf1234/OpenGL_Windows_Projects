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
// UPDATES/Changes:    None (currently)
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

#include "LoggingMessageTargets.h"  //provides MSGLOG, WRNLOG and ERRLOG targets for fprintf
#include "ProjectSetup.h"           //For GLFWwindow*
#include "GLFW_Init.h"              //For the MonitorData struct
#include "MissingAsset.h"           //For assets that are missing

enum class PIPELINE_PRIMATIVE_INPUT_TYPE {POINTS, DISCRETE_TRIANGLES, TRIANGLE_STRIP, TRIANGLE_FAN, LINE, LINE_STRIP };  //Technically LINE and LINE_STRIP are not triangles

class RenderDemoBase {
public:

	//Deleted Functionality 
	RenderDemoBase() /*= delete */;
	RenderDemoBase(const RenderDemoBase&) = delete;
	RenderDemoBase& operator=(const RenderDemoBase&) = delete;

	//Constructor and Destructor
	//RenderDemoBase(GLFWwindow * renderWindow);
	virtual ~RenderDemoBase();
	

	//-----------------------------------
	//		Abstract Functions to override
	//----------------------------------- 

	virtual void loadAssets() = 0;
	virtual void run() = 0;


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

	//Returns true iff the RenderDemo is loaded with no errors. Will return
	//false if the demo is not loaded, or if an error has occured.
	explicit operator bool() const { return (mRenderDemoLoaded && !mRenderDemoError); }
	*/
protected:

	std::vector<MissingAsset> missingAssets;
	/*
	bool mRenderDemoError;
	bool mRenderDemoLoaded;
	*/

	GLFWwindow * window; //Pointer to target renderable window (Application must provide this)


};



#endif //RENDER_DEMO_BASE_H_