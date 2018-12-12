#include "CallbackEventStreamBuffer.h"

namespace WindowCallbackInternal {

	//Set the static nextEventID variable
	std::atomic_uint64_t CallbackStreamBuffer::nextEventID = 1u;

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

	//-------------------------------------------------/\
	//                  Constructor                    \\
	//-------------------------------------------------\/
	CallbackStreamBuffer::CallbackStreamBuffer() {
		initialize();
	}
	
	//-------------------------------------------------/\
	//                   Destructor                    \\
	//-------------------------------------------------\/
	CallbackStreamBuffer::~CallbackStreamBuffer() {

	}

	//-------------------------------------------------/\
	//                Initialization                   \\
	//-------------------------------------------------\/

	void CallbackStreamBuffer::initialize() {
		mActiveBufferPair_ = 0u;
		initializeBufferPairs();

		mStoredBufferEventsSinceLastCheck_ = 0u;
		mStoredWindowStateChanges_.store(0u);
		mStoredInputEvents_.store(0u);
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
				(*iterator).eventType = WindowInputCallbackType::NULL_INPUT_EVENT;
			}
		}
		//Finally each buffer pair, set the corrosponding atomic array to track number of callbacks at any given time writing to Buffers at that index
		for (size_t i = 0u; i < NUM_BUFFER_PAIRS; i++) {
			mCurrentlyWritingEvents_[i] = 0ull;
		}
	}

	void CallbackStreamBuffer::addWindowPositionEvent(int newPosX, int newPosY) {
		//Increment atomic counter for duration of functions execution
		std::atomic_size_t eventBufferPairIndex = mActiveBufferPair_ % NUM_BUFFER_PAIRS; 
		mCurrentlyWritingEvents_[eventBufferPairIndex]++;

		std::atomic_size_t windowStateChangeEventIndex = mStoredWindowStateChanges_++;

		std::get<0>(mBufferPairs_[eventBufferPairIndex])[windowStateChangeEventIndex].eventID = nextEventID++;
		std::get<0>(mBufferPairs_[eventBufferPairIndex])[windowStateChangeEventIndex].eventType = WindowStateChangeCallbackType::WINDOW_POSITION_UPDATE;
		std::get<0>(mBufferPairs_[eventBufferPairIndex])[windowStateChangeEventIndex].winPos.newPosX = newPosX;
		std::get<0>(mBufferPairs_[eventBufferPairIndex])[windowStateChangeEventIndex].winPos.newPosY = newPosY;

		//Decrement atomic counter after finished writing
		mCurrentlyWritingEvents_[eventBufferPairIndex]--;
	}

	void CallbackStreamBuffer::addWindowRefreshEvent() {
		//Increment atomic counter for duration of functions execution
		std::atomic_size_t eventBufferPairIndex = mActiveBufferPair_ % NUM_BUFFER_PAIRS;
		mCurrentlyWritingEvents_[eventBufferPairIndex]++;

		std::atomic_size_t windowStateChangeEventIndex = mStoredWindowStateChanges_++;

		std::get<0>(mBufferPairs_[eventBufferPairIndex])[windowStateChangeEventIndex].eventID = nextEventID++;
		std::get<0>(mBufferPairs_[eventBufferPairIndex])[windowStateChangeEventIndex].eventType = WindowStateChangeCallbackType::WINDOW_REFRESH_REQUIRED;

		//Decrement atomic counter after finished writing
		mCurrentlyWritingEvents_[eventBufferPairIndex]--;
	}

	void CallbackStreamBuffer::addWindowResizeEvent(int newWindowWidth, int newWindowHeight) {
		//Increment atomic counter for duration of functions execution
		std::atomic_size_t eventBufferPairIndex = mActiveBufferPair_ % NUM_BUFFER_PAIRS;
		mCurrentlyWritingEvents_[eventBufferPairIndex]++;

		std::atomic_size_t windowStateChangeEventIndex = mStoredWindowStateChanges_++;

		std::get<0>(mBufferPairs_[eventBufferPairIndex])[windowStateChangeEventIndex].eventID = nextEventID++;
		std::get<0>(mBufferPairs_[eventBufferPairIndex])[windowStateChangeEventIndex].eventType = WindowStateChangeCallbackType::WINDOW_SIZE_UPDATED;
		std::get<0>(mBufferPairs_[eventBufferPairIndex])[windowStateChangeEventIndex].winRes.newWindowWidth = newWindowWidth;
		std::get<0>(mBufferPairs_[eventBufferPairIndex])[windowStateChangeEventIndex].winRes.newWindowHeight = newWindowHeight;

		//Decrement atomic counter after finished writing
		mCurrentlyWritingEvents_[eventBufferPairIndex]--;
	}

	void CallbackStreamBuffer::addFramebufferResizeEvent(int newFramebufferWidth, int newFramebufferHeight) {
		//Increment atomic counter for duration of functions execution
		std::atomic_size_t eventBufferPairIndex = mActiveBufferPair_ % NUM_BUFFER_PAIRS;
		mCurrentlyWritingEvents_[eventBufferPairIndex]++;

		std::atomic_size_t windowStateChangeEventIndex = mStoredWindowStateChanges_++;

		std::get<0>(mBufferPairs_[eventBufferPairIndex])[windowStateChangeEventIndex].eventID = nextEventID++;
		std::get<0>(mBufferPairs_[eventBufferPairIndex])[windowStateChangeEventIndex].eventType = WindowStateChangeCallbackType::FRAMEBUFFER_SIZE_UPDATED;
		std::get<0>(mBufferPairs_[eventBufferPairIndex])[windowStateChangeEventIndex].fbRes.newFramebufferWidth = newFramebufferWidth;
		std::get<0>(mBufferPairs_[eventBufferPairIndex])[windowStateChangeEventIndex].fbRes.newFramebufferHeight = newFramebufferHeight;

		//Decrement atomic counter after finished writing
		mCurrentlyWritingEvents_[eventBufferPairIndex]--;
	}

	void CallbackStreamBuffer::addWindowFoucsEvent(int focused) {
		//Increment atomic counter for duration of functions execution
		std::atomic_size_t eventBufferPairIndex = mActiveBufferPair_ % NUM_BUFFER_PAIRS;
		mCurrentlyWritingEvents_[eventBufferPairIndex]++;

		std::atomic_size_t windowStateChangeEventIndex = mStoredWindowStateChanges_++;

		std::get<0>(mBufferPairs_[eventBufferPairIndex])[windowStateChangeEventIndex].eventID = nextEventID++;
		std::get<0>(mBufferPairs_[eventBufferPairIndex])[windowStateChangeEventIndex].eventType = WindowStateChangeCallbackType::WINDOW_FOCUS_CHANGE;
		std::get<0>(mBufferPairs_[eventBufferPairIndex])[windowStateChangeEventIndex].winFoc.focused = focused;

		//Decrement atomic counter after finished writing
		mCurrentlyWritingEvents_[eventBufferPairIndex]--;
	}

	void CallbackStreamBuffer::addJoystickConnectionEvent(int joyID, int connected) {
		//Increment atomic counter for duration of function's execution
		std::atomic_size_t eventBufferPairIndex = mActiveBufferPair_ % NUM_BUFFER_PAIRS;
		mCurrentlyWritingEvents_[eventBufferPairIndex]++;

		std::atomic_size_t windowStateChangeEventIndex = mStoredWindowStateChanges_++;

		std::get<0>(mBufferPairs_[eventBufferPairIndex])[windowStateChangeEventIndex].eventID = nextEventID++;
		std::get<0>(mBufferPairs_[eventBufferPairIndex])[windowStateChangeEventIndex].eventType = WindowStateChangeCallbackType::JOYSTICK_CONNECTION_STATUS_UPDATE;
		std::get<0>(mBufferPairs_[eventBufferPairIndex])[windowStateChangeEventIndex].joyCon.joyID = joyID;
		std::get<0>(mBufferPairs_[eventBufferPairIndex])[windowStateChangeEventIndex].joyCon.connected = connected;

		//Decrement atomic counter after finished writing
		mCurrentlyWritingEvents_[eventBufferPairIndex]--;
	}




	//Window Input Callback Event

	void CallbackStreamBuffer::addKeyboardPressEvent(int key, int scancode, int mods) {
		//Increment atomic counter for duration of functions execution
		std::atomic_size_t eventBufferPairIndex = mActiveBufferPair_ % NUM_BUFFER_PAIRS;
		mCurrentlyWritingEvents_[eventBufferPairIndex]++;

		std::atomic_size_t inputEventIndex = mStoredInputEvents_++;

		std::get<1>(mBufferPairs_[eventBufferPairIndex])[inputEventIndex].eventID = nextEventID++;
		std::get<1>(mBufferPairs_[eventBufferPairIndex])[inputEventIndex].eventType = WindowInputCallbackType::KEYBOARD_PRESS;
		std::get<1>(mBufferPairs_[eventBufferPairIndex])[inputEventIndex].kbPress.key = key;
		std::get<1>(mBufferPairs_[eventBufferPairIndex])[inputEventIndex].kbPress.scancode = scancode;
		std::get<1>(mBufferPairs_[eventBufferPairIndex])[inputEventIndex].kbPress.mods = mods;

		//Decrement atomic counter after finished writing
		mCurrentlyWritingEvents_[eventBufferPairIndex]--;
	}

	void CallbackStreamBuffer::addKeyboarReleaseEvent(int key, int scancode, int mods) {
		//Increment atomic counter for duration of functions execution
		std::atomic_size_t eventBufferPairIndex = mActiveBufferPair_ % NUM_BUFFER_PAIRS;
		mCurrentlyWritingEvents_[eventBufferPairIndex]++;

		std::atomic_size_t inputEventIndex = mStoredInputEvents_++;

		std::get<1>(mBufferPairs_[eventBufferPairIndex])[inputEventIndex].eventID = nextEventID++;
		std::get<1>(mBufferPairs_[eventBufferPairIndex])[inputEventIndex].eventType = WindowInputCallbackType::KEYBOARD_RELEASE;
		std::get<1>(mBufferPairs_[eventBufferPairIndex])[inputEventIndex].kbRelease.key = key;
		std::get<1>(mBufferPairs_[eventBufferPairIndex])[inputEventIndex].kbRelease.scancode = scancode;
		std::get<1>(mBufferPairs_[eventBufferPairIndex])[inputEventIndex].kbRelease.mods = mods;

		//Decrement atomic counter after finished writing
		mCurrentlyWritingEvents_[eventBufferPairIndex]--;
	}

	void CallbackStreamBuffer::addKeyboardRepeatEvent(int key, int scancode, int mods) {
		//Increment atomic counter for duration of functions execution
		std::atomic_size_t eventBufferPairIndex = mActiveBufferPair_ % NUM_BUFFER_PAIRS;
		mCurrentlyWritingEvents_[eventBufferPairIndex]++;

		std::atomic_size_t inputEventIndex = mStoredInputEvents_++;

		std::get<1>(mBufferPairs_[eventBufferPairIndex])[inputEventIndex].eventID = nextEventID++;
		std::get<1>(mBufferPairs_[eventBufferPairIndex])[inputEventIndex].eventType = WindowInputCallbackType::KEYBOARD_REPEAT;
		std::get<1>(mBufferPairs_[eventBufferPairIndex])[inputEventIndex].kbRepeat.key = key;
		std::get<1>(mBufferPairs_[eventBufferPairIndex])[inputEventIndex].kbRepeat.scancode = scancode;
		std::get<1>(mBufferPairs_[eventBufferPairIndex])[inputEventIndex].kbRepeat.mods = mods;

		//Decrement atomic counter after finished writing
		mCurrentlyWritingEvents_[eventBufferPairIndex]--;
	}

	void CallbackStreamBuffer::addMouseButtonPressEvent(int button, int action, int mods) {
		//Increment atomic counter for duration of functions execution
		std::atomic_size_t eventBufferPairIndex = mActiveBufferPair_ % NUM_BUFFER_PAIRS;
		mCurrentlyWritingEvents_[eventBufferPairIndex]++;

		std::atomic_size_t inputEventIndex = mStoredInputEvents_++;

		std::get<1>(mBufferPairs_[eventBufferPairIndex])[inputEventIndex].eventID = nextEventID++;
		std::get<1>(mBufferPairs_[eventBufferPairIndex])[inputEventIndex].eventType = WindowInputCallbackType::MOUSE_BUTTON;
		std::get<1>(mBufferPairs_[eventBufferPairIndex])[inputEventIndex].mouseBtn.button = button; 
		std::get<1>(mBufferPairs_[eventBufferPairIndex])[inputEventIndex].mouseBtn.action = action;
		std::get<1>(mBufferPairs_[eventBufferPairIndex])[inputEventIndex].mouseBtn.mods = mods;

		//Decrement atomic counter after finished writing
		mCurrentlyWritingEvents_[eventBufferPairIndex]--;
	}

	void CallbackStreamBuffer::addMouseScrollEvent(double scrollX, double scrollY) {
		//Increment atomic counter for duration of functions execution
		std::atomic_size_t eventBufferPairIndex = mActiveBufferPair_ % NUM_BUFFER_PAIRS;
		mCurrentlyWritingEvents_[eventBufferPairIndex]++;

		std::atomic_size_t inputEventIndex = mStoredInputEvents_++;

		std::get<1>(mBufferPairs_[eventBufferPairIndex])[inputEventIndex].eventID = nextEventID++;
		std::get<1>(mBufferPairs_[eventBufferPairIndex])[inputEventIndex].eventType = WindowInputCallbackType::MOUSE_SCROLL;
		std::get<1>(mBufferPairs_[eventBufferPairIndex])[inputEventIndex].mouseScrl.scrollX = scrollX;
		std::get<1>(mBufferPairs_[eventBufferPairIndex])[inputEventIndex].mouseScrl.scrollY = scrollY;
		//Decrement atomic counter after finished writing
		mCurrentlyWritingEvents_[eventBufferPairIndex]--;
	}

	void CallbackStreamBuffer::addCurserWithinWindowEvent(int entered) {
		//Increment atomic counter for duration of functions execution
		std::atomic_size_t eventBufferPairIndex = mActiveBufferPair_ % NUM_BUFFER_PAIRS;
		mCurrentlyWritingEvents_[eventBufferPairIndex]++;

		std::atomic_size_t inputEventIndex = mStoredInputEvents_++;

		std::get<1>(mBufferPairs_[eventBufferPairIndex])[inputEventIndex].eventID = nextEventID++;
		std::get<1>(mBufferPairs_[eventBufferPairIndex])[inputEventIndex].eventType = WindowInputCallbackType::CURSER_ENTER;
		std::get<1>(mBufferPairs_[eventBufferPairIndex])[inputEventIndex].cEnter.entered = entered;
		//Decrement atomic counter after finished writing
		mCurrentlyWritingEvents_[eventBufferPairIndex]--;
	}

	void CallbackStreamBuffer::CallbackStreamBuffer::addCurserPositionEvent(double curserX, double curserY) {
		//Increment atomic counter for duration of functions execution
		std::atomic_size_t eventBufferPairIndex = mActiveBufferPair_ % NUM_BUFFER_PAIRS;
		mCurrentlyWritingEvents_[eventBufferPairIndex]++;

		std::atomic_size_t inputEventIndex = mStoredInputEvents_++;

		std::get<1>(mBufferPairs_[eventBufferPairIndex])[inputEventIndex].eventID = nextEventID++;
		std::get<1>(mBufferPairs_[eventBufferPairIndex])[inputEventIndex].eventType = WindowInputCallbackType::CURSER_POSITION;
		std::get<1>(mBufferPairs_[eventBufferPairIndex])[inputEventIndex].cPos.curserX = curserX;
		std::get<1>(mBufferPairs_[eventBufferPairIndex])[inputEventIndex].cPos.curserX = curserY;
		//Decrement atomic counter after finished writing
		mCurrentlyWritingEvents_[eventBufferPairIndex]--;
	}

	void CallbackStreamBuffer::addFiledropEvent(int count) {
		//Increment atomic counter for duration of functions execution
		std::atomic_size_t eventBufferPairIndex = mActiveBufferPair_ % NUM_BUFFER_PAIRS;
		mCurrentlyWritingEvents_[eventBufferPairIndex]++;

		std::atomic_size_t inputEventIndex = mStoredInputEvents_++;

		std::get<1>(mBufferPairs_[eventBufferPairIndex])[inputEventIndex].eventID = nextEventID++;
		std::get<1>(mBufferPairs_[eventBufferPairIndex])[inputEventIndex].eventType = WindowInputCallbackType::FILE_DROP;
		std::get<1>(mBufferPairs_[eventBufferPairIndex])[inputEventIndex].fd.count = count;
		//Decrement atomic counter after finished writing
		mCurrentlyWritingEvents_[eventBufferPairIndex]--;
	}


	CallbackEventBufferPair& CallbackStreamBuffer::getCallbackEvents() {
		//First off, atomically increase the atomic value mActiveBufferPair_ to push all incoming events to the next buffer pair
		mActiveBufferPair_++;

		mStoredBufferEventsSinceLastCheck_.store(0u);
		mStoredWindowStateChanges_.store(0u);
		mStoredInputEvents_.store(0u);
		
		//Next, check to see if the number of currently writing events for this buffer is greater than 0
		while (mCurrentlyWritingEvents_[(mActiveBufferPair_ - 1u) % NUM_BUFFER_PAIRS].load() > 0u) {
			std::this_thread::yield(); //Yeild to the Operating System Scheduler to try to let other threads complete themselves...
			fprintf(stdout, "\nDEBUG!!! Attempting to get callback events but an event cause this thread to yield!\n\n"); //Delete this message once testing is done
		}

		return mBufferPairs_[(mActiveBufferPair_ - 1u) % NUM_BUFFER_PAIRS];
	}





	/*void CallbackStreamBuffer::addWindowStateChangeEvent(WindowStateChangeCallbackEvent && event) {

	}*/
	

	/*void CallbackStreamBuffer::addWindowInputCallbackEvent(WindowInputCallbackEvent && event) {

	}*/

}
