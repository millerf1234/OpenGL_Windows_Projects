//  File:                      FSMCallbackInitializer.h
//  Class:                     FSMCallbackInitializer
//  QuickInfo:
//       Default Constructor    ENABLED
//       Copy Operations        DISABLED
//       Move Operations        DISABLED
//
//
//  Description:               Sets up callback functions 
//
//  
//
// Defines several functions which manage ensuring that all of
//                             the necessary callback functions have each of their requirements
//                             initialized and that each callback gets assigned to GLFW. 
//
//  Programmer:                Forrest Miller
//  Date:                      January 8, 2019



// Table of every GLFW callback
// +=======================================+=======================================+============================================================+ \\ 
// |                                       |                                       |                                                            | \\ 
// |                Event                  |  Function Used For Setting Callback   |                  Callback Function Signature               | \\ 
// |                                       |        'glfwSet______Callback()'      |                                                            | \\ 
// +=======================================+=======================================+============================================================+ \\ 
// |                                                                                                                                            | \\ 
// |                                                                                                                                            | \\ 
// |           +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+                                                                                     | \\ 
// |           |  CATEGORY: PRE-INITIALIZATION CALLBACKS  |       (These callbacks should be set before the GLFW library is initialized)        | \\ 
// |           +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+                                                                                     | \\
// +---------------------------------------+---------------------------------------+------------------------------------------------------------+ \\    
// |                                       |                                       |                                                            | \\ 
// |             GLFW_ERROR                |         glfwSetErrorCallback()        |     error_callback(int error, const char * description)    | \\ 
// |                                       |                                       |                                                            | \\ 
// +---------------------------------------+---------------------------------------+------------------------------------------------------------+ \\ 
// |                                                                                                                                            | \\ 
// |                                                                                                                                            | \\ 
// |           +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+                                                                                    | \\ 
// |           |  CATEGORY: POST-INITIALIZATION CALLBACKS  |     (These callback functions require the GLFW library to have been initialized)   | \\ 
// |           +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+                                                                                    | \\ 
// +---------------------------------------+---------------------------------------+------------------------------------------------------------+ \\ 
// |                                       |                                       |                                                            | \\ 
// |   JOYSTICK_CONNECTION_STATUS_UPDATE   |        glfwSetJoystickCallback()      |             joystick_callback(int joy, int event)          | \\ 'event' will be either GLFW_CONNECTED or GLFW_DISCONNECTED
// |                                       |                                       |                                                            | \\  
// |      MONITOR_CONNECTION_EVENT         |        glfwSetMonitorCallback()       |                                                            | \\ 
// |                                       |                                       |                                                            | \\  
// +---------------------------------------+---------------------------------------+------------------------------------------------------------+ \\ 
//
//
//            +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+
//            |    CATEGORY: PER-WINDOW CALLBACKS    |     (These callback functions are set on a per-window basis)
//            +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+
// +---------------------------------------+---------------------------------------+------------------------------------------------------------+ \\
// |                                       |                                       |                                                            | \\ 
// |           KEYBOARD_PRESS              |                                       |                                                            | \\
// |           KEYBOARD_RELEASE            |          glfwSetKeyCallback()         | key_callback(GLFWwindow* window, int key, int scancode,    | \\
// |           KEYBOARD_REPEAT             |                                       |                                      int action, int mods) | \\
// |                                       |                                       |                                                            | \\ 
// |         MOUSE_BUTTON_PRESS            |      glfwSetMouseButtonCallback()     | mouse_button_callback(GLFWwindow* window, int button,      | \\
// |        MOUSE_BUTTON_RELEASE           |                                       |                                      int action, int mods) | \\ 
// |                                       |                                       |                                                            | \\
// |            MOUSE_SCROLL               |         glfwSetScrollCallback()       | scroll_callback(GLFWwindow* window, double xoffset,        | \\
// |                                       |                                       |                                            double yoffset) | \\
// |            CURSER_ENTER               |      glfwSetCursorEnterCallback()     |  cursor_enter_callback(GLFWwindow* window, int entered)    | \\
// |                                       |                                       |                                                            | \\ 
// |           CURSER_POSITION             |      glfwSetCursorPosCallback()       | cursor_position_callback(GLFWwindow* window,               | \\ //NOTE: GLFW has the cursor position callback function 
// |                                       |                                       |                                 double xpos, double ypos)  | \\ //       declared as 'static void cursor_position_callback(...)'
// |              FILE_DROP                |          glfwSetDropCallback()        |   drop_callback(GLFWwindow* window, int count,             | \\ 
// |                                       |                                       |                                       const char** paths)  | \\ 
// |       WINDOW_POSITION_UPDATE          |       glfwSetWindowPosCallback()      | window_pos_callback(GLFWwindow* window, int xpos, int ypos)| \\
// |                                       |                                       |                                                            | \\
// |       WINDOW_REFRESH_REQUIRED         |    glfwSetWindowRefreshCallback()     |          window_refresh_callback(GLFWwindow* window)       | \\
// |                                       |                                       |                                                            | \\
// |          WINDOW_SIZE_UPDATE           |      glfwSetWindowSizeCallback()      |  window_size_callback(GLFWwindow* window, int width,       | \\
// |                                       |                                       |                                                int height) | \\
// |       FRAMEBUFFER_SIZE_UPDATE         |   glfwSetFramebufferSizeCallback()    |  framebuffer_size_callback(GLFWwindow* window, int width,  | \\
// |                                       |                                       |                                                int height) | \\
// |         WINDOW_FOCUS_CHANGE           |     glfwSetWindowFocusCallback()      |   window_focus_callback(GLFWwindow* window, int focused)   | \\
// |                                       |                                       |                                                            | \\
// |           WINDOW_ICONIFY              |     glfwSetWindowIconifyCallback()    | window_iconify_callback(GLFWwindow* window, int iconified) | \\
// |                                       |                                       |                                                            | \\
// |         WINDOW_CLOSE_ATTEMPT          |     glfwSetWindowCloseCallback()      |          window_close_callback(GLFWwindow* window)         | \\
// |                                       |                                       |                                                            | \\
// |         GET_UTF32_CHARACTER           |         glfwSetCharCallback()         |    character_callback(GLFWwindow* window,                  | \\
// |                                       |                                       |                                   unsigned int codepoint)  | \\ //Should treat 'codepoint' as little-endian UTF32  (see references at top)
// |      GET_UTF32_MODS_CHARACTER         |       glfwSetCharModsCallback()       |  charmods_callback(GLFWwindow* window,                     | \\
// |                                       |                                       |                         unsigned int codepoint, int mods)  | \\
// +---------------------------------------+---------------------------------------+------------------------------------------------------------+ \\



#pragma once

#ifndef FSM_CALLBACK_INITIALIZER_H_
#define FSM_CALLBACK_INITIALIZER_H_

#include "FSMErrorCallback.h"
#include "FSMMonitorEventCallback.h"

namespace FSMEngineInternal {

	class FSMCallbackInitializer final {
	public:
		FSMCallbackInitializer();
		~FSMCallbackInitializer() = default;
		FSMCallbackInitializer(const FSMCallbackInitializer&) = delete;
		FSMCallbackInitializer(FSMCallbackInitializer&&) = delete;
		FSMCallbackInitializer& operator=(const FSMCallbackInitializer&) = delete;
		FSMCallbackInitializer& operator=(FSMCallbackInitializer&&) = delete;
		

		//Call this function to releast all GLFW callbacks 
		void reset();


		//To be called before GLFW is initialized
		//Callbacks functions to be set:
		//   -) Error callback function
		void configurePreInitializationCallbackFunctions();


		//To be called soon after GLFW has returned from a successful 
		//initialization.
		//Callback functions to be set:
		//   -) Monitor Connection Event Callback
		//   -) Joystick Connected Event Callback 
		void configurePostInitializationCallbackFunctions();


		//Once a render window has been created, it almost certainly will  
		//be desired to have both the necessary and unnecessary callbacks 
		//assigned to the window so that it behaves properly.
		//These are the callbacks that will be assigned to the window:

		//void configureWindowSpecificCallbackFunctions(GLFWwindow*);



		/////////////////////////////////////////////
		///////   Querry the callback states    /////
		/////////////////////////////////////////////


	private:
		struct Callback_State;
	};



	








} //namespace FSMEngineInternal

#endif //GL_FRAMEWORK_CALLBACK_INITIALIZER_H_
