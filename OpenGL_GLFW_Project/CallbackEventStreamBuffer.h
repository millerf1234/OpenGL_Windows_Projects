//File:                       CallbackEventStreamBuffer.h
//
//Description:       (OUT OF DATE):         Really this class represents 2 seperate buffers, one for WindowInputCallbackEvents
//                            and one for WindowStateChangeCallbackEvents. None the less, this class is designed
//                            to buffer events from the callback functions as they happen, and then provide those
//                            events to the rest of the application when asked for them. 
//
//NOTE:                       All of the synchronization between the application main thread and the assynch GLFW callback events
//                            is performed by this class using atomics. 
//
//
//Programmer:                 Forrest Miller
//Date:                       December 7, 2018

#pragma once

#ifndef CALLBACK_EVENT_STREAM_BUFFER_H_
#define CALLBACK_EVENT_STREAM_BUFFER_H_

#include <atomic>
#include <tuple>
#include <array>
#include <vector>
#include <thread>

#include "WindowStateChangeCallbackEvent.h"
#include "WindowInputCallbackEvent.h"

namespace WindowCallbackInternal {

	typedef std::vector<WindowStateChangeCallbackEvent> WindowStateChangeBuffer;
	typedef std::vector<WindowInputCallbackEvent> WindowInputEventBuffer;
	//typedef std::pair<std::vector<WindowStateChangeCallbackEvent>, std::vector<WindowInputCallbackEvent>>  CallbackEventBufferPair;
	typedef std::pair<WindowStateChangeBuffer, WindowInputEventBuffer> CallbackEventBufferPair;

	static constexpr const size_t NUM_BUFFER_PAIRS = 3u;
	static constexpr const size_t INITIAL_STATE_CHANGE_BUFFER_SIZE = 128u;  //This is just initial sizes for the buffers, each can
	static constexpr const size_t INITIAL_WINDOW_INPUT_BUFFER_SIZE = 1024u; //  be expanded if need be

	bool operator<(const WindowStateChangeCallbackEvent& event1, const WindowStateChangeCallbackEvent& event2);
	bool operator<(const WindowStateChangeCallbackEvent& event1, const WindowInputCallbackEvent& event2);
	bool operator<(const WindowInputCallbackEvent& event1, const WindowStateChangeCallbackEvent& event2);
	bool operator<(const WindowInputCallbackEvent& event1, const WindowInputCallbackEvent& event2);



	class CallbackStreamBuffer final{
	public:
		//Constructor
		CallbackStreamBuffer();
		//Destructor
		~CallbackStreamBuffer();
		
		//Copying and Moving are disabled
		CallbackStreamBuffer(const CallbackStreamBuffer &) = delete;
		CallbackStreamBuffer(CallbackStreamBuffer&&) = delete;
		CallbackStreamBuffer operator=(const CallbackStreamBuffer &) = delete;
		CallbackStreamBuffer operator=(CallbackStreamBuffer&&) = delete;

		//------------------------------------------------
		//Public Functions for callback functions to call to add themselves to the buffer of their respective type
		//------------------------------------------------
		
		//Window State Change Events
		void addWindowPositionEvent(int newPosX, int newPosY);

		void addWindowRefreshEvent();

		void addWindowResizeEvent(int newWindowWidth, int newWindowHeight);

		void addFramebufferResizeEvent(int newFramebufferWidth, int newFramebufferHeight);

		void addWindowFoucsEvent(int focused);

		void addJoystickConnectionEvent(int joyID, int connected);


		//Window Inut Callback Event

		void addKeyboardPressEvent(int key, int scancode, int mods);

		void addKeyboarReleaseEvent(int key, int scancode, int mods);

		void addKeyboardRepeatEvent(int key, int scancode, int mods);

		void addMouseButtonPressEvent(int button, int action, int mods);

		void addMouseScrollEvent(double scrollX, double scrollY);

		void addCurserWithinWindowEvent(int entered);

		void addCurserPositionEvent(double curserX, double curserY);

		void addFiledropEvent(int count);
	
		//To be called by the main application thread, this returns a CallbackEventBufferPair containing all recorded
		//Callback Events since the previous time this function was called. This function will both block itself from
		//reading the buffer until all callbacks currently writing to it are completed, and will also redirect any new 
		//incoming events to record themselves in the next buffer. The vector returned by this function is guarenteed
		//to remain unmodified by this class or callback functions until this function is called again (or the window
		//is destroyed).
		CallbackEventBufferPair& getCallbackEvents();
	
		//Returns the number of stored events currently in the buffer. Loads the atomically stored value, so this number
		//could include events that are still in the process of adding themselves to the buffer
		size_t getNumberEventsSinceLastFlush() const { return mStoredBufferEventsSinceLastCheck_.load(); } 

	private:
		std::atomic_size_t mActiveBufferPair_;
		std::array<CallbackEventBufferPair, NUM_BUFFER_PAIRS> mBufferPairs_;

		std::atomic_size_t mStoredBufferEventsSinceLastCheck_;
		std::atomic_size_t mStoredWindowStateChanges_;
		std::atomic_size_t mStoredInputEvents_;
		std::array<std::atomic_size_t, NUM_BUFFER_PAIRS> mCurrentlyWritingEvents_; 

		
		//Helper functions
		void initialize();
		void initializeBufferPairs();

		static std::atomic_uint64_t nextEventID;
	};

} //namespace WindowCallbackInternal

#endif //CALLBACK_EVENT_STREAM_BUFFER_H_