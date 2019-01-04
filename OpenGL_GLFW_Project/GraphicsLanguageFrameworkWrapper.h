//File:                 GraphicsLanguageFrameworkWrapper.h
//
//  
//  Notes on GLFW behavior:     
//                            -If a monitor with one or more fullscreen windows on it gets disconnected,
//                             those windows are forced into windowed mode. 
//
//
//-----------------------------------------------------------------------------------------------------------//
// (OUT OF DATE DESCRIPTION) [I really need to hold off on writing these until I have finalized my design]   //
//-----------------------------------------------------------------------------------------------------------//
//Description:         Defines a wrapper class for the GLFW (Graphics Language FrameWork) library.
//                     Is intended to be used as a convient object for OS-Independent Window and GL Context 
//                     creation and usage (in other words, provide a C++-based object-oriented encapsulation for GLFW,
//                     which is itself a C API). 
//                     All GLFW-related function calls can be handled through this class. 
//                     In the event of a GL context reset, this class will be able to use its stored internal state to 
//                     provide a way to recover.
//
//  (NOTE FOR DEVELOPMENT, DELETE WHEN COMPLETED:)      This class is intended to replace the older GLFW_Init class and related 
//                                                      GLFW functionality. If this note is still here, than chances are this class 
//                                                      and the related sub-classes are not yet fully functional.
//
//
//  References:            
//          [GLFW Documentation]                 https://www.glfw.org/docs/3.3/intro_guide.html#init_hints
//
//
//
// Programmer:          Forrest Miller
// Date:                December 2018 -  January 2019

#pragma once

#ifndef GRAPHICS_LANGUAGE_FRAMEWORK_WRAPPER_H_
#define GRAPHICS_LANGUAGE_FRAMEWORK_WRAPPER_H_

#include <vector>
#include <string>
#include <sstream>

#include "ProjectSetup.h" //For GLFW library

#include "GraphicsLanguageFrameworkContext.h"
#include "GraphicsLanguageFrameworkMonitor.h"
#include "GraphicsLanguageFrameworkWindow.h"

#include "ConnectedMonitors.h"

//#include "GLFrameworkCallbackInitializer.h"  //Provides functions which handle assigning GLFW callback functions.


//I am trying out this thing where I have moved the implementation-dependent-only "#include" statements over to the .cpp file
//#include "LoggingMessageTargets.h"

class GraphicsLanguageFrameworkWrapper final {
public:
	//                  /////////////////////////////////////////////////////////
	//                  //////////            Constructor(s)           //////////
	//                  /////////////////////////////////////////////////////////

	//Creates an instance of the GraphicsLanguageFrameworkWrapper class. This will perform all steps necessary
	//for the member functions to create a graphics context and window to be called. Default OpenGL version is
	//4.5, but can set to a different version through parameters glVersionMajor and glVersionMinor. It is 
	//recommended to stick with version 4.5 or later and to not use the compatability profile. 
	GraphicsLanguageFrameworkWrapper(const int glVersionMajor = 4,
		                             const int glVersionMinor = 5,
		                             const bool useCompatabilityProfile = false);


	//                  //////////////////////////////////////////////////////
	//                  //////////            Destructor            //////////
	//                  //////////////////////////////////////////////////////
	~GraphicsLanguageFrameworkWrapper();


	//                  //////////////////////////////////////////////////////
	//                  //////////      Public Member Functions      /////////
	//                  //////////////////////////////////////////////////////
	
	//    //--------------------------------------------------\\
	//    ||            Safe to call at any time              ||
	//    \\--------------------------------------------------//    

	//Sets parameters to match the version of OpenGL that contexts created with this class will use. These values can only 
	//be specified as parameters during object construction. 
	void getRequestedGLVersion(int& versionMajor, int& versionMinor, bool& usesCompatMode) const;
	//Returns the OpenGL version major that contexts created through this object will use. This value is set 
	//at object construction and can't be modified once set.
	int getRequestedGLVersionMajor() const;
	//Returns the OpenGL version minor that contexts created through this object will use. This value is set 
	//at object construction and can't be modified once set.
	int getRequestedGLVersionMinor() const;
	//Returns a string outlining the version of OpenGL that will be used when creating GL Contexts. These
	//values can only be specified as parameters during object construction. The returned string will 
	//look like (with the sections in-between the single quotes being replaced by their respective
	//integer values): 
	//    "OpenGL 'versionMajor'.'versionMinor' Core"                     if not using compatibility mode
	//  or
	//     "OpenGL 'versionMajor'.'versionMinor' Compatability"           if using compatibility mode
	std::string getRequestedGLVersionString() const;
	//Retrieves the Major, Minor and Revision numbers corrosponding to the version of GLFW used 
	//during compile time. These values can be compared with the shared-library GLFW version loaded
	//at runtime (which could be helpful for debugging).
	void getGLFWCompileTimeVersion(int& compileVersionMajor, int& compileVersionMinor, int& compileVersionRevision) const;
	//Returns a string that looks like: "Compiled against GLFW 'major'.'minor'.'revision'", with the
	//'major', 'minor' and 'revision' replaced with their corrosponding integer values. 
	std::string getGLFWCompileTimeVersionString() const;
	//Retrieves the Major, Minor and Revision numbers corrosponding to the version of GLFW linked with 
	//during run time. These values can be compared with the compile-time GLFW version (which could be 
	//helpful for debugging purposes).
	void getGLFWRuntimeVersion(int& runtimeVersionMajor, int& runtimeVersionMinor, int& runtimeVersionRevision) const;
	//Returns a string that looks like: "Running against GLFW 'major'.'minor'.'revision'", with the
	//'major', 'minor' and 'revision' replaced with the corrosponding integer values retrieved from
	//the GLFW library which gets linked at run-time. 
	std::string getGLFWRuntimeVersionString() const;

	//Checks to see if this object is ready for use
	inline bool ready() const { return mGLFWWasInitialized_; }
    //Allows for the object to be treated as a bool in certain expressions. Will take on the value
	//of 'true' if object is ready and will take on the value 'false' if not ready 
	explicit operator bool() const { return ready(); }


	//    //---------------------------------------------------------\\
	//    ||       Call only after confirming object is ready        ||    
	//    \\---------------------------------------------------------//
	//Calling any of the following functions while the member function 'ready()'
	//has not returned true will cause the function to return immediatly





	//                 //////////////////////////////////////////////////////
	//                 //////////    Copying/Moving are disabled    /////////
	//                 //////////////////////////////////////////////////////
	GraphicsLanguageFrameworkWrapper(const GraphicsLanguageFrameworkWrapper&) = delete;
	GraphicsLanguageFrameworkWrapper(GraphicsLanguageFrameworkWrapper &&) = delete;
	GraphicsLanguageFrameworkWrapper& operator=(const GraphicsLanguageFrameworkWrapper&) = delete;
	GraphicsLanguageFrameworkWrapper& operator=(GraphicsLanguageFrameworkWrapper&&) = delete;

	

private:
	bool mGLFWWasInitialized_;
	int mGLVersionMajor_, mGLVersionMinor_, mGLCompatabilityMode_;

	ConnectedMonitors mAvailableMonitors_;
	


	//////////////////////
	// Helper functions //
	//////////////////////

	//Call the following only from a constructor
	void giveMemberVariablesInitialValues(); 
	void setGLVersion(const int glVersionMajor, const int glVersionMinor, const bool useCompatProfile);
	bool validateRuntimeGLFWVersion() const; //Compares runtime-linked GLFW version with version used during compile time 
	void setInitializationHints() const;
	bool initializeGLFW(); 
	void aquireGLFrameworkComponents();


	

	
};


#endif //GRAPHICS_LANGUAGE_FRAMEWORK_WRAPPER_H_


