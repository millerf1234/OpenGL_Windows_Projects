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

	CallbackStreamBuffer::CallbackStreamBuffer() {
		initialize();
	}


	CallbackStreamBuffer::~CallbackStreamBuffer() {

	}


	void CallbackStreamBuffer::initialize() {
		mActiveBufferPair_ = 0u;
		initializeBufferPairs();

		mStoredBufferEventsSinceLastCheck_ = 0u;
	}

	void CallbackStreamBuffer::initializeBufferPairs() {
		//For each buffer pair
		for (size_t i = 0; i < NUM_BUFFER_PAIRS; i++) {
			//For the pair's State Change Buffer:
			WindowStateChangeBuffer stateChangeBuffer = std::get<0>(mBufferPairs_[mActiveBufferPair_]);
			// 1)  Populate the state change buffer to INITIAL_BUFFER_SIZE size
			stateChangeBuffer.resize(INITIAL_STATE_CHANGE_BUFFER_SIZE);
			// 2)  Iterate through the populated buffer, setting every element's eventID to 0 and type to NO_EVENT
			for (auto iterator = stateChangeBuffer.begin(); iterator != stateChangeBuffer.end(); iterator++) {
				(*iterator).eventID = 0ull;
				(*iterator).eventType = WindowStateChangeCallbackType::NO_EVENT;
			}

			//Then repeat the same steps for the Window Input Buffer
			WindowInputEventBuffer inputEventBuffer = std::get<1>(mBufferPairs_[mActiveBufferPair_]);
			inputEventBuffer.resize(INITIAL_WINDOW_INPUT_BUFFER_SIZE);
			for (auto iterator = inputEventBuffer.begin(); iterator != inputEventBuffer.end(); iterator++) {
				(*iterator).eventID = 0ull;
				(*iterator).eventType = WindowInputCallbackType::NO_EVENT;
			}

			//Finally each buffer pair, set the corrosponding atomic array og 
		}
	}

	/*void CallbackStreamBuffer::addWindowStateChangeEvent(WindowStateChangeCallbackEvent && event) {

	}*/
	

	/*void CallbackStreamBuffer::addWindowInputCallbackEvent(WindowInputCallbackEvent && event) {

	}*/

}
