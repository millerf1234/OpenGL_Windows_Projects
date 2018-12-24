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

		fprintf(MSGLOG, "\nGLFW_CALLBACK::Detected framebuffer size update! New fb is %dx%d pixels\n", width, height);
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

	void joystickConnectionCallback(GLFWwindow * window, int joyID, int connected) {

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