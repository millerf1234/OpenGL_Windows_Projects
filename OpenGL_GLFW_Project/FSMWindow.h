//File:          FSMWindow.h
//
// Description:    todo...
//
// Programmer:   Forrest Miller
// Date:         December 2018


//Note:  Windowed mode windows can be made full screen by setting a monitor with glfwSetWindowMonitor(). Likewise,
//       Fullscreen windows can be made windowed through the same function.

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