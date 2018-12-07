//File:                            PrimaryWindowAttachment.h
//
//Description:                     Defines a class that is intended to be attached to the primary
//                                 GLFW window for an application with the window's assignable
//                                 user pointer. Handles setting and tracking the windows state as
//                                 well as providing a buffer for callback functions triggered by
//                                 the window.
//
//This class has high cohesion with the functions defined in GLFW_CallbackFunctions.h
//
// Programmer:                    Forrest Miller
// Date:                          December 7, 2018

#pragma once

#ifndef PRIMARY_WINDOW_ATTACHMENT_H_
#define PRIMARY_WINDOW_ATTACHMENT_H_

#include "WindowCallbackBuffer.h"
#include "WindowCurserState.h"

#include <atomic>              //Used for synchnization between Application's main thread and Window Callbacks
#include <mutex>               //Used for synchronization between main thread and callbacks

static constexpr const size_t EVENT_CYCLES_TO_SAVE = 8u;

class PrimaryWindowAttachment {
public:
	//Constructor
	PrimaryWindowAttachment();
	//Destructor
	~PrimaryWindowAttachment();

	//No copying/Moving of the window attachment allowed
	PrimaryWindowAttachment(const PrimaryWindowAttachment&) = delete;
	PrimaryWindowAttachment(PrimaryWindowAttachment&&) = delete;
	PrimaryWindowAttachment& operator=(const PrimaryWindowAttachment&) = delete;
	PrimaryWindowAttachment& operator=(PrimaryWindowAttachment&&) = delete;


	//--------------------------------------------------------------
	// Public Synchronus functions for setting and tracking the windows
	// state
	//--------------------------------------------------------------




	//-------------------------------------------------------------
	//  Public function(s) to be called by the Application main thread
	//     [Will temporarily block the callbacks from writing]
	//-------------------------------------------------------------
	WindowCallbackBuffer& getBufferedEvents();

	//-------------------------------------------------------------
	//  Public function(s) to be called by the Callback Functions to record 
	//  their data to the BufferedEvents for the current cycle.
	//     [Will temporarily block the main thread from reading until done writing]
	//-------------------------------------------------------------
	


	
	

private:
	std::mutex mCallbackWriteLock_;
	std::atomic_size_t mCurrentEventCycle_;
	std::vector<WindowCallbackBuffer> mBufferedEventCycles_;


	//Private helper functions
	void initialize();

};


#endif //PRIMARY_WINDOW_ATTACHMENT_H_
