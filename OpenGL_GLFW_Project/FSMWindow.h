//File:          FSMWindow.h
//
// Description:    todo...
//
// Programmer:   Forrest Miller
// Date:         January 2019


//Note:  Windowed mode windows can be made full screen by setting a monitor with glfwSetWindowMonitor(). Likewise,
//       Fullscreen windows can be made windowed through the same function.

//
//  Notes on GLFW behavior:     
//                            -If a monitor with one or more fullscreen windows on it gets disconnected,
//                             those windows are forced into windowed mode. 


#pragma once

#ifndef FSM_WINDOW_H_
#define FSM_WINDOW_H_

namespace FSMEngineInternal {

	class FSMWindow {
	public:
		FSMWindow();
		~FSMWindow();

	private:
		bool mFullScreen_;

	};


} //namespace FSMEngineInternal


#endif //FSM_WINDOW_H_