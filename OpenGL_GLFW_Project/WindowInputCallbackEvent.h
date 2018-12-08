//File:                      CallbackEventBucket.h
//
//       Compare with WindowStateChangeCallbackEvent.h
//
//              I am not sure yet if this will work, so description has been left lacking...
//
//     Loosly based on: https://en.wikibooks.org/wiki/C%2B%2B_Programming/Unions
//
//Programmer:                Forrest Miller
//Date:                      December 7, 2018

#pragma once

#ifndef WINDOW_INPUT_CALLBACK_EVENT_H_
#define WINDOW_INPUT_CALLBACK_EVENT_H_

#include <atomic>

namespace WindowCallbackInternal {


	//-------------------------------------------------------
	//    Input Callback Events
	//-------------------------------------------------------

	enum class WindowInputCallbackType { KEYBOARD_PRESS, KEYBOARD_RELEASE, KEYBOARD_REPEAT, MOUSE_BUTTON,
		MOUSE_SCROLL, CURSER_ENTER, CURSER_POSITION, FILE_DROP, NO_EVENT };

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

	struct MouseButton {
		int button;
		int action;    //either GLFW_PRESS or GLFW_RELEASE
		int mods;
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

	struct NoEvent {

	};


	struct WindowInputCallbackEvent {
		uint64_t eventID;
		WindowInputCallbackType eventType;
		union {
			KeyboardPress kbPress;
			KeyboardRelease kbRelease;
			KeyboardRepeat kbRepeat;
			MouseButton mouseBtn;
			MouseScroll mouseScrl;
			CurserEnter cEnter;
			CurserPosition cPos;
			Filedrop fd;
			NoEvent noEvent;
		};
	};



} //namespace WindowCallbackInternal



#endif //WINDOW_INPUT_CALLBACK_EVENT_H_

