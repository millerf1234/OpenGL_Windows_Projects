//File:                     PrimaryWindowCallbackFunctions.cpp
//
//Contains the implementation for the GLFW Window Callback functions.
//See header file for more details


#include "PrimaryWindowCallbackFunctions.h"



namespace WindowCallbackInternal {

	void windowPositionCallback(GLFWwindow* window, int xPos, int yPos) {

		fprintf(MSGLOG, "\nDetected that window position has changed!\n");
		
		//CallbackStreamBuffer * callbackBuffer = static_cast<CallbackStreamBuffer *>(glfwGetWindowUserPointer(window));
		//if (callbackBuffer) {
		//	callbackBuffer->addWindowPositionEvent(xPos, yPos);
		//}
	}

	void windowRefreshCallback(GLFWwindow* window) {

	}

	void windowSizeCallback(GLFWwindow* window, int width, int height) {
		fprintf(MSGLOG, "\nGLFW_CALLBACK::Detected window size update! New screen coordinates are %dx%d\n", width, height);
		//OpenGL operates using pixels instead of screen coordinates, which means there is nothing else to do here. GL_Viewport will be updated with framebufferSizeCallback()


		// CallbackStreamBuffer * callbackBuffer = static_cast<CallbackStreamBuffer *>(glfwGetWindowUserPointer(window));
		//if (callbackBuffer) {
		//	callbackBuffer->addWindowResizeEvent(width, height);
		//}

	}


	void framebufferSizeCallback(GLFWwindow* window, int width, int height) {

		//fprintf(MSGLOG, "\nGLFW_CALLBACK::Detected framebuffer size update! New fb is %dx%d pixels\n", width, height);
        if ( (width > 0) && (height > 0) )
		    glViewport(0, 0, width, height);

		//CallbackStreamBuffer * callbackBuffer = static_cast<CallbackStreamBuffer *>(glfwGetWindowUserPointer(window));
		//if (callbackBuffer) {
	//		callbackBuffer->addFramebufferResizeEvent(width, height);
		//}
	}

	void windowFocusChangeCallback(GLFWwindow* window, int focused) {

	}


	//---------------------------------------------------------------------------------
	//    Helper function for detecting modifiers    (Not a callback itself)
	//---------------------------------------------------------------------------------
	inline void determineMods(int mods) {
		fprintf(MSGLOG, "\tDetected Mods (int mods = %d): [", mods);
		if (mods == 0) {
			fprintf(MSGLOG, "none]\n");
		}
		else {
			if ((mods & GLFW_MOD_SHIFT)) {
				fprintf(MSGLOG, " shift ");
			}
			if (mods & GLFW_MOD_ALT) {
				fprintf(MSGLOG, " alt ");
			}
			if (mods & GLFW_MOD_CAPS_LOCK) {  //This is not used when clicking
				fprintf(MSGLOG, " caps_lock ");
			}
			if (mods & GLFW_MOD_CONTROL) {
				fprintf(MSGLOG, " control ");
			}
			if (mods & GLFW_MOD_NUM_LOCK) {
				fprintf(MSGLOG, " num_lock ");
			}
			if (mods & GLFW_MOD_SUPER) {
				fprintf(MSGLOG, " super ");
			}
			fprintf(MSGLOG, "]\n");
			int additionalMods = (GLFW_MOD_SHIFT | GLFW_MOD_ALT | GLFW_MOD_CAPS_LOCK | GLFW_MOD_CONTROL | GLFW_MOD_NUM_LOCK | GLFW_MOD_SUPER);
			if (mods & (!(additionalMods))) {
				fprintf(MSGLOG, "\tAdditional mods: %d\n", mods & (!(additionalMods)));
			}
		}
	}

    //IMPORTANT NOTICE: I thought there was a memory race in the function, but it turns out that is not the case. Rather,
	//                  GLFW has a few functions I call within the course of this function that GLFW specifically restricts
	//                  to only being called on the main execution thread. Since this is a callback function, it is violating
	//                  GLFW's API.
	//                  To remedy this issue, I am removing the offending API calls. Please note that this will lessen the 
	//                  usefulness of this code.
	void joystickConnectionCallback(int joyID, int connected) {
		fprintf(MSGLOG, "\n-----------------------------------------------------------------\n");
		fprintf(MSGLOG, "vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv\n");

		fprintf(MSGLOG, "\nGLFW Event Callback! Joystick Connection Event Detected!\n"
			"Joystick ID: %d    \tEvent Code: %d", joyID, connected);
		if (connected == GLFW_CONNECTED) {
			fprintf(MSGLOG, "\tEvent: Connected\n");
		}
		else if (connected == GLFW_DISCONNECTED) {
			fprintf(MSGLOG, "\tEvent: Disconnected\n");
			//fprintf(MSGLOG, "\n[Just for yuks... The function 'glfwJoystickPresent(%d)' has returned %d ]\n",
			//	glfwJoystickPresent(joyID));
			//fprintf(MSGLOG, "\n       [FYI GLFW_TRUE is %d and GLFW_FALSE is %d]\n\n", GLFW_TRUE, GLFW_FALSE);
			return;
		}
		else {
			fprintf(ERRLOG, "\nERROR: An invalid value was passed to the GLFW Joystick event Callback function!\n"
				"The value passed in was %d, which is not supposed to happen!\n", connected);
			return;
		}

		//Only do the following if 'event' was GLFW_CONNECTED
		fprintf(MSGLOG, "Let's take a closer look at what sort of info GLFW\n"
			"will convey to us about this new input device...\n");

		//fprintf(MSGLOG, "\nFirst let's just get the obvious out of the way... \n");
		//int controllerIsConnected = glfwJoystickPresent(joyID);
		//fprintf(MSGLOG, "\tThe function 'glfwJoystickPresent(%d)' has returned %d, which is also the macro: ",
		//	joyID, controllerIsConnected);
		//if (controllerIsConnected == GLFW_TRUE) {
		//	fprintf(MSGLOG, " GLFW_TRUE\n");
		//}
		//else if (controllerIsConnected == GLFW_FALSE) {
		//	fprintf(MSGLOG, " GLFW_FALSE\n");
		//}
		//else {
		//	fprintf(ERRLOG, " ERROR INVALID MACRO !!!\n");
		//	return;
		//}

#ifdef IGNORE_THE_GLFW_API_AND_CALL_FUNCTIONS_WHICH_ARE_NOT_THREAD_SAFE_FROM_THIS_CALLBACK
		//Most of the GLFW functions I call in this following section of code are in fact not at all thread safe and 
		//thus have no business what-so-ever being called from within this callback function. Funnily enough, I was
		//calling those functions as part of this code for nearly 3 whole years and only quite recently did I find 
		//there was a problem with them. I an going to do the correct thing for now by disabling this whole section 
		//of offending function calls, but really it seems to be quite rare that these did actually cause a problem
		//so I've left them disabled behind a macro which means it would be quite easy to re-enabled them again
		int isGamepad = glfwJoystickIsGamepad(joyID);
		std::string name = glfwGetJoystickName(joyID);
		
		int buttonCount = 0;
		const unsigned char* buttons = glfwGetJoystickButtons(joyID, &buttonCount);
		int axisStates = 0;
		const float* axes = glfwGetJoystickAxes(joyID, &axisStates);
		int hatCount = 0;
		const unsigned char* hats = glfwGetJoystickHats(joyID, &hatCount);
		std::string guid = glfwGetJoystickGUID(joyID);

		fprintf(MSGLOG, "\nHere are the interesting details:\n\tGUID = %s\n\t\tName = %s\n", guid.c_str(), name.c_str());
		fprintf(MSGLOG, "\t\tButton Count is %d\n\t\tAxes Count is %d\n\t\tHat Count is %d\n\n", buttonCount, axisStates, hatCount);
		fprintf(MSGLOG, "\nIs it a (recognized) gamepad?  Answer = ");
		if (isGamepad)
			fprintf(MSGLOG, "YES!  \n");
		else
			fprintf(MSGLOG, "no...     too bad...\n\n");


		if (isGamepad) {
			//Try out the gamepad-specific functions
			std::string gamepadName = glfwGetGamepadName(joyID);
			fprintf(MSGLOG, "\nGamepad name is %s\n", gamepadName.c_str());
		}
#endif //
	}


	void windowMaximizeCallback(GLFWwindow* window, int iconified) {
		if (iconified == GLFW_TRUE) {
			fprintf(MSGLOG, "\nWindow Maximized!\n");
		}
		else if (iconified == GLFW_FALSE) {
			fprintf(MSGLOG, "\nWindow Is No Longer Maximized!\n");
		}
		else {
			fprintf(ERRLOG, "\nERROR: An invalid value was passed to the GLFW Window Maximize Callback function!\n"
				"The value passed in was %d, which is not supposed to happen!\n", iconified);
		}
	}

	//---------------------------------------------------------------------------------
	//     User-Input-Triggered Callbacks
	//---------------------------------------------------------------------------------

	void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {

	}

	void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
		fprintf(MSGLOG, "MouseButton Callback Function called with the following parameters:\n");

		fprintf(MSGLOG, "\tButton: ");
		switch (button) {
		case (GLFW_MOUSE_BUTTON_1):
			fprintf(MSGLOG, "Left Mouse Button\n");
			break;
		case (GLFW_MOUSE_BUTTON_2):
			fprintf(MSGLOG, "Right Mouse Button\n");
			break;
		case (GLFW_MOUSE_BUTTON_3):
			fprintf(MSGLOG, "Middle Mouse Button\n");
			break;
		case (GLFW_MOUSE_BUTTON_4):
			fprintf(MSGLOG, "Mouse Button 4\n");
			break;
		case (GLFW_MOUSE_BUTTON_5):
			fprintf(MSGLOG, "Mouse Button 5\n");
			break;
		case (GLFW_MOUSE_BUTTON_6):
			fprintf(MSGLOG, "Mouse Button 6\n");
			break;
		case (GLFW_MOUSE_BUTTON_7):
			fprintf(MSGLOG, "Mouse Button 7\n");
			break;
		case (GLFW_MOUSE_BUTTON_8):
			fprintf(MSGLOG, "Mouse Button 8\n");
			break;
		default:
			fprintf(MSGLOG, " [UNRECOGNIZED BUTTON CODE: %d]\n", button);
			break;
		}

		fprintf(MSGLOG, "\tAction: ");
		if (action == GLFW_PRESS) {
			fprintf(MSGLOG, "PRESS\n");
		}
		else if (action == GLFW_RELEASE) {
			fprintf(MSGLOG, "RELEASE\n");
		}
		else {
			fprintf(MSGLOG, "NEITHER PRESS NOR RELEASE!!!\n");
		}

		determineMods(mods);
		fprintf(MSGLOG, "\n");
	}



	static std::atomic_bool mouseOnScreen = false;
	static std::atomic_int_fast64_t positionCallbacksCalled = 0;



	void mouseScrollCallback(GLFWwindow* window, double x, double y) {

		//Notes: Scrolling forward is y = 1.0, scrolling backwards is y = -1.0
		//       Scrollling to the left is x = 1.0, scrolling to the right is x = -1.0

		fprintf(MSGLOG, "MouseScroll Callback Function called with the following parameters:\n");
		fprintf(MSGLOG, "\t{x = %f,  y = %f}\n\n", x, y);
	}



	void curserEnterCallback(GLFWwindow* window, int entered) {
		bool mouseOnScreenValueOnEnteringCallback = mouseOnScreen.load();
		fprintf(MSGLOG, "CurserEnterWindow Callback Function called.\n\tDetected that the curser has ");
		if (entered == GLFW_FALSE) {
			fprintf(MSGLOG, "left ");
		}
		else if (entered == GLFW_TRUE) {
			fprintf(MSGLOG, "entered ");
		}
		else {
			fprintf(MSGLOG, "[UNRECOGNIZED VALUE: %d] ", entered);
		}
		fprintf(MSGLOG, "the window!\n");
		fprintf(MSGLOG, "\tmouseOnScreen was %s on entering this function and is now ", ((mouseOnScreenValueOnEnteringCallback) ? ("true\0") : ("false\0")));
		if (entered == GLFW_TRUE) {
			mouseOnScreenValueOnEnteringCallback = true;
		}
		else if (entered == GLFW_FALSE) {
			mouseOnScreenValueOnEnteringCallback = false;
		}
		fprintf(MSGLOG, "%d\n\n", ((mouseOnScreenValueOnEnteringCallback) ? (1) : (0)));

		mouseOnScreen.store(mouseOnScreenValueOnEnteringCallback);
	}


	void curserPositionCallback(GLFWwindow* window, double x, double y) {
		//bool mouseOnScreenValueOnEnteringCallback = mouseOnScreen.load();
		//fprintf(MSGLOG, "CurserPositionCallback    (TimesCalled = %lld):\n\tmouseOnScreen is %d,\t\t{x,y} = {%f,%f}\n\n", positionCallbacksCalled++,
		//	((mouseOnScreenValueOnEnteringCallback) ? (1) : (0)), x, y);
	}

	void filedropCallback(GLFWwindow* window, int count, const char ** filePaths) {
		if (count <= 0) {
			fprintf(ERRLOG, "\nERROR! For some reason the file drop callback function was called but\n"
				"the 'count' parameter representing the number of files that were dropped\n"
				"was set to the value %d! This is obviosly a major issue!\n", count);
		}
		else {
			fprintf(MSGLOG, "\nDetected that the following files was dragged and dropped onto the render window:\n");
			for (int i = 0; i < count; i++) {
				fprintf(MSGLOG, "\t(%d)  %s\n", i, filePaths[i]);
			}
		}
	}


} //namespace WindowCallbackInternal