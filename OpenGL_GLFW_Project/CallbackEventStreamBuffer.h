//File:                       CallbackEventStreamBuffer.h
//
//Description:                Really this class represents 2 seperate buffers, one for WindowInputCallbackEvents
//                            and one for WindowStateChangeCallbackEvents. None the less, this class is designed
//                            to buffer events from the callback functions as they happen, and then provide those
//                            events to the rest of the application when asked for them. 
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

#include "WindowStateChangeCallbackEvent.h"
#include "WindowInputCallbackEvent.h"

namespace WindowCallbackInternal {

	typedef std::pair<std::vector<WindowStateChangeCallbackEvent>, std::vector<WindowInputCallbackEvent>>  CallbackEventBufferPair;

	static constexpr const size_t BUFFER_PAIRS = 3u;
	static constexpr const size_t BUFFER_SPACE_TO_RESERVE = 100u;

	bool operator<(const WindowStateChangeCallbackEvent& event1, const WindowStateChangeCallbackEvent& event2);
	bool operator<(const WindowStateChangeCallbackEvent& event1, const WindowInputCallbackEvent& event2);
	bool operator<(const WindowInputCallbackEvent& event1, const WindowStateChangeCallbackEvent& event2);
	bool operator<(const WindowInputCallbackEvent& event1, const WindowInputCallbackEvent& event2);



	class CallbackStreamBuffer final{
	public:
		CallbackStreamBuffer();
		~CallbackStreamBuffer();
	
		//Adds a completed WindowStateChangeCallbackEvent to the end of the current WindowStateChangeCallbackEventBuffer.
		//It is up to code calling this function to ensure that the provided struct is complete. Passing this function
		//an incomplete struct could cause catastrophic problems down the road (i.e. crash). 
		void addWindowStateChangeEvent(WindowStateChangeCallbackEvent && event);

		//Adds a completed WindowStateChangeCallbackEvent to the end of the current WindowStateChangeCallbackEventBuffer.
		//It is up to code calling this function to ensure that the provided struct is complete. Passing this function
		//an incomplete struct could cause catastrophic problems down the road (i.e. crash). 
		void addWindowInputCallbackEvent(WindowInputCallbackEvent && event);
	
		//To be called by the main application thread, this returns a CallbackEventBufferPair containing all recorded
		//Callback Events since the previous time this function was called. Calling this function causes any subsequent
		//events to begin recording into a new buffer.
		CallbackEventBufferPair getCallbackEvents();
		
		//To be called by the main application thread, this function takes in an empty CallbackEventBufferPair by reference
		//and swaps it out with it's own currentl active CallbackEventBufferPair containing all recorded Callback Events
		//since the previous time this function was called. Calling this function causes any subsequent
		//events to begin recording into a new buffer. (This function is identical to the one returning a CallbackEventBufferPair 
		//except it uses the std::vector member function 'swap()' to reduce copying)
		void getCallbackEvents(CallbackEventBufferPair& eventPair);

		//Returns the number of stored events currently in the buffer. Loads the atomically stored value, so this number
		//could include events that are still in the process of adding themselves to the buffer
		size_t getStoredBufferEventsSinceLastFlush() const { return mStoredBufferEventsSinceLastCheck_.load(); } 

	private:
		std::atomic_size_t mActiveBufferPair_;
		std::array<CallbackEventBufferPair, BUFFER_PAIRS> mBufferPairs_;

		std::atomic_size_t mStoredBufferEventsSinceLastCheck_;
		std::atomic_size_t mCurrentlyWritingEvents_;

		
		//Helper functions
		void initialize();
	};

} //namespace WindowCallbackInternal

#endif //CALLBACK_EVENT_STREAM_BUFFER_H_