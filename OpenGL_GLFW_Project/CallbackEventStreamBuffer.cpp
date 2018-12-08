#include "CallbackEventStreamBuffer.h"

namespace WindowCallbackInternal {

	////////////////////////////////////////////////////////////////////////////////////
	///////                 Callback Event Comparison Functions                 ////////
	////////////////////////////////////////////////////////////////////////////////////
	bool operator<(const WindowStateChangeCallbackEvent& event1, const WindowStateChangeCallbackEvent& event2) {
		return (event1.eventID < event2.eventID);
	}
	bool operator<(const WindowStateChangeCallbackEvent& event1, const WindowInputCallbackEvent& event2) {
		return (event1.eventID < event2.eventID);
	}
	bool operator<(const WindowInputCallbackEvent& event1, const WindowStateChangeCallbackEvent& event2) {
		return (event1.eventID < event2.eventID);
	}
	bool operator<(const WindowInputCallbackEvent& event1, const WindowInputCallbackEvent& event2) {
		return (event1.eventID < event2.eventID);
	}


	void CallbackStreamBuffer::initialize() {
		mActiveBufferPair_ = 0u;
		std::get<0>(mBufferPairs_[mActiveBufferPair_]).reserve(BUFFER_SPACE_TO_RESERVE);
		std::get<1>(mBufferPairs_[mActiveBufferPair_]).reserve(BUFFER_SPACE_TO_RESERVE);

		mStoredBufferEventsSinceLastCheck_ = 0u;
		mCurrentlyWritingEvents_ = 0u;
	}


	CallbackStreamBuffer::CallbackStreamBuffer() {
		initialize();
	}


	CallbackStreamBuffer::~CallbackStreamBuffer() {

	}


	void CallbackStreamBuffer::addWindowStateChangeEvent(WindowStateChangeCallbackEvent && event) {

	}
	

	void CallbackStreamBuffer::addWindowInputCallbackEvent(WindowInputCallbackEvent && event) {

	}

}
