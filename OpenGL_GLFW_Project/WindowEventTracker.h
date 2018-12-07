//File:                                 WindowEventTracker.h
//Class:                                WindowEventTracker
//
//Description:                          Intended to be attached to a GLFWWindow through the window's provided 
//                                      UserPointer member, this class provides a way to keep a record events 
//                                      that are responded to asynchronously through GLFW callback functions. 
//                                      The design of this class is based around storing WindowEvents that 
//                                      can be processed by the rest of the application when it is ready to do
//                                      so. 
//                                      I am not sure yet how the final implementation will look, so the description
//                                      will be left pretty vague for now. Expect to see some sort of atomic counter 
//                                      representing either a frame or number of polls by the rest of the application.
//
//Programmer:                           Forrest Miller
//Date:                                 December 5, 2018

#pragma once

#ifndef WINDOW_EVENT_TRACKER_H_
#define WINDOW_EVENT_TRACKER_H_

//Constants
static inline constexpr const unsigned long long BASE_EVENT_PERIODS_TO_KEEP = 120ull;

class WindowEventTracker {
public:
	WindowEventTracker();
	~WindowEventTracker();
private:

};


#endif //WINDOW_EVENT_TRACKER_H_



