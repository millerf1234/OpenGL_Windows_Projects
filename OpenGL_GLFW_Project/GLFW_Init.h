//  See .cpp file for more info
//
//  GLFW_Init.h
//
//  Created by Forrest Miller on 2/12/18.
//  Re-purposed for Windows project on 7/16/2018
//  Many updates made over the next year or so

#pragma once

#ifndef GLFW_INIT_H_
#define GLFW_INIT_H_

#include <iostream>
#include <sstream>
#include <stack>

//Need <cstdlib> for assigning 'std::atexit()' callback functions which 
//are needed to ensure proper cleanup procedures are executed when the
//Application exits. 
#include <cstdlib> 

#include "GLFW_Init_Report.h"

#include "GlobalConstants.h"
#include "PrimaryWindowCallbackFunctions.h"

#include "WindowConfiguration.h"
#include "ApplicationConstantSettings.h"


//Internal Namespace
//  The following namespace is for allowing 'atexit' callback functions to
//    be communicated with and used to ensure proper clean-up of GLFW handles
//    when the program exits. 
//                                       NOTE ON REENTRANCY 
//                      The GLFW documentation warns that it is illegal to
//                         call 'GLFWterminate()' from a callback function
//                         since otherwise there is a high risk of reentrancy.
//                      However the 'atexit()' callbacks are a special type 
//                         of callback in that they are thread safe. The only 
//                         way this could lead to any problems is if one thread
//                         calls 'std::exit()' while a window/cursor is still 
//                         active on another thread. 
namespace GLFW_INIT_INTERNAL {

    //              +------------------------------------+
    //              |  GLFWinit()   +   GLFWterminate()  |
    //              +------------------------------------+
    //Singleton boolean value representing whether 'GLFWinit()' has been
    //called and returned success within this processes lifetime. 
    inline bool& GLFW_IS_INIT() noexcept {
        static bool glfwIsInit = false;
        return glfwIsInit;
    }
    //The 'atexit' callback function for calling 'GLFWterminate()' as
    //part of the program's exit procedure 
    inline void atExitFuncToEnsureGLFWTerminateIsCalled() noexcept {
        fprintf(ERRLOG, "\nCalling GLFW TERMINATE!\n");
        if (GLFW_IS_INIT()) {
            glfwTerminate();
            GLFW_IS_INIT() = false;
        }
    }
    
    //        +---------------------------------------------------+
    //        | Active GLFWwindow Handle -- Tracking + Releasing  |
    //        +---------------------------------------------------+
    using WindowStack = std::stack<GLFWwindow*>;
    //Singleton stack of Active Window Handles
    inline WindowStack& GLFW_ACTIVE_WINDOW_HANDLES() noexcept {
        static WindowStack activeWindowHandles;
        return activeWindowHandles;
    }

    inline void atExitFuncToReleaseAllActiveWindowHandles() noexcept {
        try {
            WindowStack activeWindowHandles = GLFW_ACTIVE_WINDOW_HANDLES();
            while (!activeWindowHandles.empty()) {
                //'top()' returns the top element of the stack but leaves it in place
                const GLFWwindow* winHandle = activeWindowHandles.top();
                //'pop()' returns void but is needed to remove the top element from 
                //the stack
                activeWindowHandles.pop();
                assert(winHandle);
                //Release the window handle with GLFW

            }
        } catch (...) { return; }
    }

    //  [In The Future]
    //        +---------------------------------------------------+
    //        | Active GLFWcursor Handle -- Tracking + Releasing  |
    //        +---------------------------------------------------+
    

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


    //'GLFWterminate()' is too important of a step to be left up to an external source
    //for reliably ensuring gets called under the wide variety of possible exit 
    //conditions. A much better solution has replaced it which instead assigns an 'atexit' 
    //callback function for calling 'GLFWterminate()' that gets assigned as soon as
    //'GLFWinitialize' returns success. This way it only will be called when required and
    //will not be missed during most exit conditions.
	//void terminate(); //Ends the window

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
    



    ////////////////////////////////
    //     Utility Functions     //
    //////////////////////////////
    
     //Private function to be called to generate the return struct
    //of system data at the end of public member function 'initialize()'.
    std::unique_ptr<InitReport> generateDetectedMonitorsStruct(); 

    //VSync is separate from all of other window configuration settings in
    //that it is to be set only after an OpenGL context has been created and 
    //made current
    void setVsync() const noexcept;


    //                                                      //
    //           ASSIGN ATEXIT CALLBACK FUNCTIONS           //
    //                                                      //
    
    // CALL THIS FUNCTION TO ASSIGN CALLBCKS
    //Due to the slight complexity behind ensuring that all of the
    //GLFW-related 'atexit()' callback functions get assigned in 
    //their required order to function properly, the task of assigning
    //them is accomplished all through a single function call. 
    void assignGLFWRelatedAtExitCallbackFunctions() noexcept {
        assert(GLFW_INIT_INTERNAL::GLFW_IS_INIT());
        assignAtExitTerminationFunction();
        assignAtExitFunctionForReleasingAllActiveWindowHandles();
        //assignAtExitFunctionForReleasingAllActiveCursors(); //Not yet implemented
    }

    
    //      DO NOT CALL THIS FUNCTION DIRECTLY. INSTEAD USE THE ABOVE FUNCTION
    //            WHICH SETS ALL OF THE 'atexit()' CALLBACKS AT ONCE!
    //
    // DESCRIPTION  --  Assigns the 'atexit()' callback function which will ensure
    //                  'glfwTerminate()' is called before this process ends. 
    //
    // IMPORTANCE  --  This is the most important of the GLFW clean-up functions
    //                 It must always be the final 'atexit' cleanup callback called
    //                 since it forcibly destroys all remaining active GLFW 
    //                 components. 
    //                
    // FATAL ERROR ON FAILURE  --  Due to the importance of ensuring that GLFW is
    //                             able to clean up after itself within the OS, it is
    //                             necessary to always treat a possible failure in
    //                             assigning this callback by calling 'glfwTerminate()'
    //                             then crashing. Any other alternative to this would 
    //                             result in the failure for 'glfwTerminate()' ever 
    //                             being called, and leaving the OS with a mess to 
    //                             clean up once this process ends. The benefit of 
    //                             reacting this way is the Application is able to 
    //                             guarantee that it always cleans up after itself 
    //                             properly.
    void assignAtExitTerminationFunction() noexcept;

    //      DO NOT CALL THIS FUNCTION DIRECTLY. INSTEAD USE THE ABOVE FUNCTION
    //            WHICH SETS ALL OF THE 'atexit()' CALLBACKS AT ONCE!
    //
    // DESCRIPTION  --  Assign the 'atexit()' callback function which ensures
    //                  any GLFWwindow handles that are still active when this
    //                  Application is ready to exit each get successfully
    //                  released back to the OS. This callback function MUST
    //                  be assigned after the termination 'atexit()' callback 
    //                  function has been assigned!
    //
    // NON-FATAL WARNING ON FAILURE  --  If this 'atexit()' callback function 
    //                                   fails to be assigned for any reason, 
    //                                   the Application luckily is able to 
    //                                   simply issue a warning and continue 
    //                                   its execution since the termination 
    //                                   callback function will still ensure 
    //                                   all GLFW components are released. The
    //                                   clean-up may not be as elegant as if
    //                                   this callback were able to run before
    //                                   the final termination, but at least it
    //                                   can be guaranteed that no resources will
    //                                   be leaking out past the lifetime of this
    //                                   process for the OS to have to deal with.
    void assignAtExitFunctionForReleasingAllActiveWindowHandles() noexcept;

};



#endif // GLFW_INIT_H_ 