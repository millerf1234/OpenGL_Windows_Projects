//File:          GraphicsLanguageFrameworkWindow.h
//
// Description:    todo...
//
// Programmer:   Forrest Miller
// Date:         December 2018


//Note:  Windowed mode windows can be made full screen by setting a monitor with glfwSetWindowMonitor(). Likewise,
//       Fullscreen windows can be made windowed through the same function.

#pragma once

#ifndef GRAPHICS_LANGUAGE_FRAMEWORK_WINDOW_H_
#define GRAPHICS_LANGUAGE_FRAMEWORK_WINDOW_H_

namespace FSMEngineInternal {

	class GraphicsLanguageFrameworkWindow {
	public:
		GraphicsLanguageFrameworkWindow();
		~GraphicsLanguageFrameworkWindow();

	private:
		bool mFullScreen_;

	};


} //namespace FSMEngineInternal


#endif //GRAPHICS_LANGUAGE_FRAMEWORK_WINDOW_H_