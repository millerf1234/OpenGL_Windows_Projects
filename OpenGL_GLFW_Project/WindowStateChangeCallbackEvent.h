//File:                      CallbackEventBucket.h
//
//              I am not sure yet if this will work, so description has been left lacking...
//
//     Loosly based on: https://en.wikibooks.org/wiki/C%2B%2B_Programming/Unions
//
//Programmer:                Forrest Miller
//Date:                      December 7, 2018

#pragma once

#ifndef WINDOW_STATE_CHANGE_CALLBACK_EVENT_H_
#define WINDOW_STATE_CHANGE_CALLBACK_EVENT_H_

#include <atomic>

namespace WindowCallbackInternal {

	//-------------------------------------------------------
	//     Window State Change Callback Events
	//-------------------------------------------------------
	enum class WindowStateChangeCallbackType { WINDOW_POSITION_UPDATE, WINDOW_REFRESH_REQUIRED, WINDOW_SIZE_UPDATED, FRAMEBUFFER_SIZE_UPDATED,
	WINDOW_FOCUS_CHANGE, JOYSTICK_CONNECTION_STATUS_UPDATE, NO_EVENT };
	
	//The following structs represent the required data associated with each variety of callback. 

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

	struct NoEvent {

	};

	struct WindowStateChangeCallbackEvent {
		uint64_t eventID;
		WindowStateChangeCallbackType eventType;
		union {
			WindowPosition winPos;
			WindowRefresh winRef;
			WindowResize winRes;
			FramebufferResize fbRes;
			WindowFocus winFoc;
			JoystickConnection joyCon;
			NoEvent noEvent;
		};
	};

} //namespace WindowCallbackInternal

#endif // WINDOW_STATE_CHANGE_CALLBACK_EVENT_H_
