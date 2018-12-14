//File:                        WindowCallbackEvent.h
//
//Description:                 Defines a number of different types representing Window Callback Events (which are 
//                             essentially tiny datatypes designed to encapsulate the details of a GLFW callback
//                             function call). GLFW provides a number of available callback function pointers that
//                             can be assigned on a per-window basis. In order to faciliate a robust and accurate 
//                             interface between these callback functions and the main application, the intention 
//                             behind these types is to encapsulate the state of a Callback function call event,
//                             which can then be fed into one or more buffers* for parsing by the application. It is                        *(Implementation TBD)
//                             assumed (by me) that these callback functions are assynchronus, so care has (will) been (be) taken 
//                             to provide proper synchronization measures.
//                             Since GLFW is a C API, the design of the Callback Event System will follow a C-style 
//                             implementation using a union; I imagine there is a also a C++ based method using an 
//                             abstract base CallbackEvent class that each type of event can inherit from, but that 
//                             is not the route I am persuing
//
// Notes:                       
//                             -While writing this code, there was at one point a distinction made between Input Callback Events
//                              and callback events caused by window state changes. However, it is preferable to have all callback
//                              events to remain in the chronological order that they occured, which has caused the two event types
//                              to be recombined into the single Callback Event type defined in this file.
//
// Reference:                   (GLFW Input Guide) https://www.glfw.org/docs/latest/input_guide.html
//                              (GLFW Window Guide) https://www.glfw.org/docs/latest/window_guide.html
//                              (GLFW Monitor Guide) https://www.glfw.org/docs/latest/monitor_guide.html
//                              (Text input will be UTF32 little-endian)  https://en.wikipedia.org/wiki/UTF-32
//                              (For working with UTF in C-based languages)  https://unicodebook.readthedocs.io/unicode_encodings.html
//
// Date:                       December 12, 2018
// Programmer:                 Forrest Miller

#pragma once

#ifndef WINDOW_CALLBACK_EVENT_H_
#define WINDOW_CALLBACK_EVENT_H_

#include <cstdint>    //Ensures availability of exact-width integer types

namespace WindowCallbackInternal {

	// The follow enum corrosponds to one of every available types of GLFW callback 
	//        ( aside from the special cases of:
	//                i)  NO_EVENT, which represents the lack of a callback event;
	//               ii)  the three KEYBOARD_PRESS/RELEASE/REPEAT and two MOUSE_PRESS/RELEASE events,
	//                      each set of which is created from the same callback function; and
	//              iii)  GLFW_ERROR, which is set once per application (instead of per-window/per-context)
	//                      and is implemented differently [notice it doesn't take a GLFWwindow* parameter] )
	//
	// +---------------------------------------+---------------------------------------+------------------------------------------------------------+ \\
	// |            CallbackEventType          |       glfwSet______Callback()         |                    callback function signature             | \\
	// +---------------------------------------+---------------------------------------+------------------------------------------------------------+ \\
	// |                                       |                                       |                                                            | \\ 
	// |              NO_EVENT                 |                   N/A                 |                                N/A                         | \\
	// |                                       |                                       |                                                            | \\
	// |             GLFW_ERROR                |         glfwSetErrorCallback()        |     error_callback(int error, const char * description)    | \\  //NOTE: Seperate From all of the other Callback types, 
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
	// |   JOYSTICK_CONNECTION_STATUS_UPDATE   |       glfwSetJoystickCallback()       |             joystick_callback(int joy, int event)          | \\ 'event' will be either GLFW_CONNECTED or GLFW_DISCONNECTED
    // |                                       |                                       |                                                            | \\ 
	// |         WINDOW_CLOSE_ATTEMPT          |     glfwSetWindowCloseCallback()      |          window_close_callback(GLFWwindow* window)         | \\
	// |                                       |                                       |                                                            | \\
	// |         GET_UTF32_CHARACTER           |         glfwSetCharCallback()         |    character_callback(GLFWwindow* window,                  | \\
	// |                                       |                                       |                                   unsigned int codepoint)  | \\ //Should treat 'codepoint' as little-endian UTF32  (see references at top)
	// |      GET_UTF32_MODS_CHARACTER         |       glfwSetCharModsCallback()       |  charmods_callback(GLFWwindow* window,                     | \\
	// |                                       |                                       |                         unsigned int codepoint, int mods)  | \\
	// |      MONITOR_CONNECTION_EVENT         |        glfwSetMonitorCallback()       |   
	// +---------------------------------------+---------------------------------------+------------------------------------------------------------+ \\

	enum class CallbackEventType { NO_EVENT, KEYBOARD_PRESS, KEYBOARD_RELEASE, KEYBOARD_REPEAT, MOUSE_BUTTON_PRESS,
		MOUSE_BUTTON_RELEASE, MOUSE_SCROLL, CURSER_ENTER, CURSER_POSITION, FILE_DROP, WINDOW_POSITION_UPDATE, 
		WINDOW_REFRESH_REQUIRED, WINDOW_SIZE_UPDATED, FRAMEBUFFER_SIZE_UPDATED, WINDOW_FOCUS_CHANGE, WINDOW_ICONIFY,
		JOYSTICK_CONNECTION_STATUS_UPDATE, WINDOW_CLOSE_ATTEMPT, GET_UTF32_CHARACTER, GET_UTF32_MODS_CHARACTER
	};


	/*                                                                                                                     *\
	     ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	    /////       For each type of Callback Event, a struct is used to represent the specifics that event       //////
	   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	\*                                                                                                                     */

	struct NoEvent {  //Default type representing an incomplete/null event

	};

	struct KeyboardPress {
		int key;
		int scancode;
		int mods;
	};

	struct KeyboardRelease {
		int key;
		int scancode;
		int mods;
	};

	struct KeyboardRepeat {
		int key;
		int scancode;
		int mods;
	};

	struct MouseButtonPress {
		int button;
		int mods;
	};

	struct MouseButtonRelease {  // Note from GLFW documentation: When a window window loses focus, synthetic release events will be automatically triggered
		int button;              //                                for all pressed mouse buttons. The way to tell the difference between user input and 
		int mods;                //                                 synthetic is to change state of window focus with the window focus callback.
	};

	struct MouseScroll {
		double scrollX; //left is positive x, right is negative x
		double scrollY; //towards screen is positive y
	};

	struct CurserEnter {
		int entered;
	};

	struct CurserPosition {
		double curserX;   //Positions start with {0.0, 0.0} as the top left corner and have  
		double curserY;   //extent equal to the window size in screen-space coordinates.
	};

	struct Filedrop {
		int count;
	};

	struct WindowPosition {
		int newPosX;
		int newPosY;
	};

	struct WindowRefresh {
		//Nothing
	};

	struct WindowResize {
		int newWindowWidth;
		int newWindowHeight;
	};

	struct FramebufferResize {
		int newFramebufferWidth;
		int newFramebufferHeight;
	};

	struct WindowFocus {
		int focused;
	};

	struct JoystickConnection {
		int joyID;
		int connected;  //either GLFW_CONNECTED or GLFW_DISCONNECTED
	};

} //namespace WindowCallbackInternal

#endif //WINDOW_CALLBACK_EVENT_H_
