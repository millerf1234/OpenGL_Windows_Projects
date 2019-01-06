//  File:                   JoystickStatePrinter.cpp
//  
//  Description:            See header for details
//
//
//  IMPLEMENTATION DISCLAIMER: 
//                          When I wrote this class, I did not intend for it to be something
//                          that would be used beyond my initial goal of quickly getting a rough 
//                          sense of the sort of input I could expect to recieve from some various
//                          Joystick devices I had lying around. However this class soon proved to
//                          be incredibly useful and so it grew into a permanent fixture. Due to 
//                          the way in which this code was hastily written, it does not meet my
//                          typical standards for both clarity and good design. If you are 
//                          attempting to update this code, perhaps it might be quicker to just
//                          re-write this entire implementation than understand what is happening.
//
//  
//  Programmer:             Forrest Miller
//  Date:                   December 29-30, 2018



#include "JoystickStatePrinter.h"

#include <sstream>
#include <algorithm>


JoystickStatePrinter::JoystickStatePrinter(int id) : mID_(id) {
	mPrintedDisconnectMessage_ = false;
	mWasSetToNewID_ = false;
	mPrintFullJoystickState_ = false; //Print full state by default

	if ((id < 0) || (id > GLFW_JOYSTICK_LAST)) {
		fprintf(WRNLOG, "\nWARNING! Invalid Joystick ID requested for input echoing!\n"
			"Joystick %d was requested, but only Joysticks 0-%d are available for state printing!\n"
			"By default, Joystick 0 will be printed\n", id, GLFW_JOYSTICK_LAST);
		id = 0;
	}

	if (glfwJoystickPresent(id)) {
		mName_ = glfwGetJoystickName(id);
		mHasName_ = true;
		mHasDefaultState_ = false;
		recordNewlyConnectedJoystickState();
	}
	else {
		mName_ = DISCONNECTED_JOYSTICK_NAME;
		mHasName_ = false;
		mHasDefaultState_ = false;
	}
}


void JoystickStatePrinter::changeID(int id) {

	if ( (id < 0) || (id == mID_) || (id > GLFW_JOYSTICK_LAST) ) { 
		fprintf(WRNLOG, "\nWARNING! Unable to switch Joystick echoing target to Joystick %d!\n"
			"Only Joysticks 0-%d are available!\n", id, GLFW_JOYSTICK_LAST);
		return; 
	}
	else {
		fprintf(MSGLOG, "\nNow echoing the state of Joystick %d\n", id);
	}
	mPrintedDisconnectMessage_ = false;
	mWasSetToNewID_ = false;
	mID_ = id;
	mHasDefaultState_ = false;
	if (glfwJoystickPresent(id)) {
		mName_ = glfwGetJoystickName(id);
		mHasName_ = true;
	}
	else {
		mName_ = DISCONNECTED_JOYSTICK_NAME;
		mHasName_ = false;
	}
}

void JoystickStatePrinter::nextJoystick() {
	if (mID_ == GLFW_JOYSTICK_LAST)
		changeID(0);
	else
		changeID(mID_ + 1);	
}

void JoystickStatePrinter::previousJoystick() {
	if (mID_ == 0)
		changeID(GLFW_JOYSTICK_LAST);
	else 
		changeID(mID_ - 1);
}

void JoystickStatePrinter::printState() {
	fprintf(MSGLOG, "%s", buildStateString().c_str());
}

std::string JoystickStatePrinter::getStateString() {
	return buildStateString();
}

void JoystickStatePrinter::toggleMessageOutputMode() {
	mPrintFullJoystickState_ = !mPrintFullJoystickState_;
}

void JoystickStatePrinter::reaquireDefaultState() {
	//Please Note that this function is not intended to 
	//be used to get the initial state of a Joystick when
	//one if first connected. Calling this function
	//with mHasDefaultState_ set to true will return 
	//without any modification.
	if (mHasDefaultState_) {
		BaseJoyState joyStateBackup = mDefaultState_;

		mHasDefaultState_ = false; //Temporarily fake object not having a set default state
		recordNewlyConnectedJoystickState(); //Try to aquire a default state

		if (!mHasDefaultState_) { //If something went wrong
            //Revert to stored backup 
			mDefaultState_ = joyStateBackup; 
			mHasDefaultState_ = true;
		}
	}
}

void JoystickStatePrinter::recordNewlyConnectedJoystickState() {
	if (mHasDefaultState_) { return; } //If a default state is already recorded, then return

	//Confirm that there is a joystick availble
	if (!glfwJoystickPresent(mID_)) { return; } 

	clearPreviouslyStoredDefaultState(); 

	//Build the struct
	const float * axes = glfwGetJoystickAxes(mID_, &(mDefaultState_.numAxes));
	glfwGetJoystickButtons(mID_, &(mDefaultState_.numButtons));
	glfwGetJoystickHats(mID_, &(mDefaultState_.numHats));

	for (int i = 0; i < mDefaultState_.numAxes; i++) {
		mDefaultState_.initialAxesValues.push_back(axes[i]);
		if ((axes[i] * axes[i]) == 1.0f) {
			//Mark the axis as predicted as being a trigger 
			mDefaultState_.axesPredictedToBeTriggers.push_back(i); 
		}
	}
	mHasDefaultState_ = true;
}


void JoystickStatePrinter::clearPreviouslyStoredDefaultState() {
	mDefaultState_.numAxes = 0;
	mDefaultState_.numButtons = 0;
	mDefaultState_.numHats = 0;
	mDefaultState_.initialAxesValues.clear();
	mDefaultState_.axesPredictedToBeTriggers.clear();
}




std::string JoystickStatePrinter::buildStateString() {
	bool joystickPresent = glfwJoystickPresent(mID_);
	
	//While a joystick is connected, keep the disconnectedMessage flag unsignaled
	if (joystickPresent) { mPrintedDisconnectMessage_ = false; }
	else { mHasDefaultState_ = false; } //Lose recorded default state if controller disconnects

	//If the disconnectedMessage flag is signaled, then don't print out a message
	if (mPrintedDisconnectMessage_) { return ""; }  //No need to print a message repeatedly for a disconnected joystick

	//If we are connected but don't have a name currently, that must be fixed by 
	//aquiring a name for our newly connected Joystick
	if (joystickPresent && !mHasName_) {
		mName_ = glfwGetJoystickName(mID_);
		mHasName_ = true;
	}

	std::stringstream stateMsg;

	if (mWasSetToNewID_) {
		stateMsg << "\nJoystick to poll input from changed to Joystick #" << (mID_ + 1);
		mWasSetToNewID_ = false;
	}


	if (mPrintFullJoystickState_ || (!joystickPresent)) {  //The full-state message mode includes the Disconnected message
		buildFullStateMessage(stateMsg); //stateMsg passed as reference 
	}
	else { //Print the alternative message style where only detected inputs are echoed
		buildDetectedInputsMessage(stateMsg); //stateMsg passed as reference
	}
	return stateMsg.str();
}


void JoystickStatePrinter::buildFullStateMessage(std::stringstream& stateMsg) {
	
	//Put a header at the top of the message
	stateMsg << "\n\n";
	stateMsg << "+-----------------------------------------------------------------------------+\n";
	stateMsg << "|vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv|\n";
	stateMsg << "+-----------------------------------------------------------------------------+\n";
	stateMsg << "                        Current State of Joystick: " << mName_ << "\n";

	stateMsg << "\tConnection Status: ";
	//Find out if the Joystick is connected
	if (glfwJoystickPresent(mID_)) {
		stateMsg << "CONNECTED\n";
	}
	else {
		stateMsg << "DISCONNECTED\n";
		mPrintedDisconnectMessage_ = true;
		//Right after the joystick is disconnected and we printed its name with the 'disconnected'
		//message, here is an excellent spot to reset ourselves for the next joystick connection
		//to come along...
		mHasName_ = false;
		mName_ = DISCONNECTED_JOYSTICK_NAME;
	}

	//Find out if the Joystick matches a SDL Gamepad-Mapping (GLFW provides this functionality, but
	//the mappings come from SDL)
	stateMsg << "Gamepad Mapping Available: ";
	if (glfwJoystickIsGamepad(mID_)) {
		//const char * gamepadName = glfwGetGamepadName(mID_);
		stateMsg << "TRUE\nGamepadMappingName: " << glfwGetGamepadName(mID_) << "\n";
	}
	else {
		stateMsg << "FALSE\n";
	}

	//If the disconnectedMessage flag became signaled above, we can skip over
	//the main body of the output message. Otherwise, we build the output message
	//as follows:
	if (!mPrintedDisconnectMessage_) {


		//Joystick Axis State
		int axes;
		const float * axis = glfwGetJoystickAxes(mID_, &axes);
		if (axes > 0) {
			stateMsg << "Read Axis values: \n";
			for (int i = 0; i < axes; i++) {
				stateMsg << "Axis" << i << ": " << axis[i] << "        ";
				if ((i % 3) == 2) { //after every 3rd iteration 
					stateMsg << "\n"; //start printing on a new line
				}
			}
			if ((axes % 3) != 2) { //If loop didn't exit on a power of 3
				stateMsg << "\n"; //add a newline 
			}
		}
		else {
			stateMsg << "Joystick has no available axes to report values from...\n";
		}



		//Joystick Button States  (which include hat's by default)
		int buttons;
		const unsigned char* buttonStates = glfwGetJoystickButtons(mID_, &buttons);
		if (buttons > 0) {
			stateMsg << "\nJoystick has " << buttons << " buttons available for querrying\n";
			for (int i = 0; i < buttons; i++) {
				stateMsg << "Button[" << i << "] is ";
				if (buttonStates[i] == GLFW_PRESS)
					stateMsg << "PRESSED";
				else
					stateMsg << "RELEASED";
				if ((i % 2) == 0)
					stateMsg << "\t\t";
				else
					stateMsg << "\n";
			}
			if ((buttons % 2) == 0)
				stateMsg << "\n";
		}
		else {
			stateMsg << "Joystick has no available buttons to querry the state of...\n";
		}



		//Joystick Hat States
		int hats;
		const unsigned char* hatStates = glfwGetJoystickHats(mID_, &hats);
		if (hats > 0) {
			if (hats == 1) {
				stateMsg << "Joystick has 1 'hat' input available for querrying\n";
				stateMsg << "Hat is currently in state: ";
				stateMsg << convertHatInputToText(hatStates[0]);
			}
			else {
				stateMsg << "Joystick has %d hats available for querrying\n";

				for (int i = 0; i < hats; i++) {
					stateMsg << "Hat[" << i << "] has state " << convertHatInputToText(hatStates[i]);
				}
			}
			stateMsg << "\n\n";
		}
		else {
			stateMsg << "Joystick has no hats to report the state of!\n\n";
		}

	}

	//Place an ending to the message to match the header
	stateMsg << "+-----------------------------------------------------------------------------+\n";
	stateMsg << "|^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^|\n";
	stateMsg << "+-----------------------------------------------------------------------------+\n";

}


void JoystickStatePrinter::buildDetectedInputsMessage(std::stringstream& stateMsg) {
	bool hasGamepadMapping = glfwJoystickIsGamepad(mID_);

	if (!mHasDefaultState_) { //If missing default state
		recordNewlyConnectedJoystickState(); //Try to get it
	}

	if (!mHasDefaultState_) { //If there still isn't any default state
		stateMsg << "\nERROR! Unable to aquire a default state for joystick!\n";
		return;
	}

	GLFWgamepadstate mappedInput;
	if (hasGamepadMapping) {
		if (!(glfwGetGamepadState(mID_, &mappedInput))) {
			hasGamepadMapping = false;
		}
	}

	//Detect Axis States First
	bool valueWasPrinted = false;
	bool axisWasPrinted = false;
	const float * axis;
	if (hasGamepadMapping) {
		axis = mappedInput.axes;
	}
	else {
		axis = glfwGetJoystickAxes(mID_, &(mDefaultState_.numAxes));
	}
	for (int i = 0; i < mDefaultState_.numAxes; i++) {
		//Find out if this axis has been predicted to be a trigger
		std::vector<int>::iterator suspectedTriggersIter;
		suspectedTriggersIter = std::find(mDefaultState_.axesPredictedToBeTriggers.begin(),
			mDefaultState_.axesPredictedToBeTriggers.end(), i);
		if (suspectedTriggersIter != mDefaultState_.axesPredictedToBeTriggers.end()) {
			//Then we suspect this Axis to be a trigger
			if (axis[i] != -1.0f) {
				axisWasPrinted = true;
				float axisPercentage = ((axis[i] + 1.0f) / 2.0f);
				stateMsg << "Axis[" << i << "] = " << axisPercentage << "%.  Axis predicted to be trigger.  ";
				if (hasGamepadMapping) {
					stateMsg << "Gamepad Mapping: " << getAxisGamepadMapping(i);
				}
				stateMsg << "\n";
			}
		}
		else {
			//This axis was not suspected to be a trigger
			if ( glm::abs(axis[i]) >= AXIS_DEAD_ZONE_SIZE ) {
			    //If axis is detected to be beyond the dead zone
				axisWasPrinted = true;
				stateMsg << "Axis[" << i << "] = " << axis[i];
				if (hasGamepadMapping) {
					stateMsg << "   Gamepad Mapping: " << getAxisGamepadMapping(i);
				}
				stateMsg << "\n";
			}
		}
	}
 	

	//Record if any text was output
	valueWasPrinted = axisWasPrinted;


	//Detect Button Presses
	bool buttonWasPrinted = false;
	const unsigned char* buttonStates;
	if (hasGamepadMapping) {
		buttonStates = mappedInput.buttons;
	}
	else {
		buttonStates = glfwGetJoystickButtons(mID_, &(mDefaultState_.numButtons));
	}
	int pressedButtonCounter = 0;
	
	for (int i = 0; i < mDefaultState_.numButtons; i++) {
		if (buttonStates[i] == GLFW_PRESS) {
			buttonWasPrinted = true;
			pressedButtonCounter++;
			if (!hasGamepadMapping) {
				stateMsg << "Button[" << i << "] PRESSED       ";
				if ((pressedButtonCounter % 4) == 3) {
					stateMsg << "\n";
				}
			}
			else {
				stateMsg << "Button[" << i << "] (" << predictButton(i) << ")           ";
				if ((pressedButtonCounter % 3) == 2) {
					stateMsg << "\n";
				}
			}
		}
	}
	if (buttonWasPrinted) {
		//Add the newline if final loop iteration didn't add one automatically
		if (!hasGamepadMapping) {
			if ((pressedButtonCounter % 4) != 3)
				stateMsg << "\n";
		}
		else {
			if ((pressedButtonCounter % 3) != 2)
				stateMsg << "\n";
		}
	}

	//Again record in any text has yet been output
	valueWasPrinted = (valueWasPrinted || buttonWasPrinted);

    //Hat press detection
	bool hatWasPrinted = false;
	const unsigned char* hatStates = glfwGetJoystickHats(mID_, &(mDefaultState_.numHats));
	
	for (int i = 0; i < mDefaultState_.numHats; i++) {
		if (hatStates[i] != GLFW_HAT_CENTERED) {
			hatWasPrinted = true;
			stateMsg << "Hat[" << i << "] reports state: " << convertHatInputToText(hatStates[i]) << "\n";
		}
	}

	valueWasPrinted = (valueWasPrinted || hatWasPrinted);

	if (valueWasPrinted) { //If anything was detected and printed at all, put some blank space at the end... 
		stateMsg << "\n\n";
	}
}


std::string JoystickStatePrinter::predictButton(int buttonIndex) const {
	std::string predictedMapping;
	if (buttonIndex < 0) {
		predictedMapping = "Error Negative Button!!!";
	}
	else if (buttonIndex > 14) {
		predictedMapping = "No Prediction available for buttons beyond index 14";
	}
	else {
		switch (buttonIndex) {
		case (GLFW_GAMEPAD_BUTTON_A):
			predictedMapping = "A";
			break;
		case (GLFW_GAMEPAD_BUTTON_B):
			predictedMapping = "B";
			break;
		case (GLFW_GAMEPAD_BUTTON_X):
			predictedMapping = "X";
			break;
		case (GLFW_GAMEPAD_BUTTON_Y):
			predictedMapping = "Y";
			break;
		case (GLFW_GAMEPAD_BUTTON_BACK):
			predictedMapping = "BACK";
			break;
		case (GLFW_GAMEPAD_BUTTON_START):
			predictedMapping = "START";
			break;
		case (GLFW_GAMEPAD_BUTTON_GUIDE):
			predictedMapping = "GUIDE";
			break;
		case (GLFW_GAMEPAD_BUTTON_DPAD_UP):
			predictedMapping = "DPAD UP";
			break;
		case (GLFW_GAMEPAD_BUTTON_DPAD_RIGHT):
			predictedMapping = "DPAD RIGHT";
			break;
		case (GLFW_GAMEPAD_BUTTON_DPAD_DOWN):
			predictedMapping = "DPAD DOWN";
			break;
		case (GLFW_GAMEPAD_BUTTON_DPAD_LEFT):
			predictedMapping = "DPAD LEFT";
			break;
		case (GLFW_GAMEPAD_BUTTON_LEFT_BUMPER):
			predictedMapping = "LEFT BUMPER";
			break;
		case (GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER):
			predictedMapping = "RIGHT BUMPER";
			break;
		case (GLFW_GAMEPAD_BUTTON_LEFT_THUMB):
			predictedMapping = "LEFT THUMB STICK";
			break;
		case (GLFW_GAMEPAD_BUTTON_RIGHT_THUMB):
			predictedMapping = "RIGHT THUMB STICK";
			break;
		default:
			predictedMapping = "?";
			break;
		}
	}
	return predictedMapping;
}

std::string JoystickStatePrinter::getAxisGamepadMapping(int axis) const {
	std::string axisName = "";
	if (axis < 0 || axis > GLFW_GAMEPAD_AXIS_LAST) {
		axisName = "[Invalid Axis Index] '" + std::to_string(axis) + "' ";
	}
	else {
		switch (axis) {
		default:
			axisName = "[UNKNOWN AXIS]";
			break;
		case GLFW_GAMEPAD_AXIS_LEFT_X:
			axisName = "Left X";
			break;
		case GLFW_GAMEPAD_AXIS_LEFT_Y:
			axisName = "Left Y";
			break;
		case GLFW_GAMEPAD_AXIS_RIGHT_X:
			axisName = "Right X";
			break;
		case GLFW_GAMEPAD_AXIS_RIGHT_Y:
			axisName = "Right Y";
			break;
		case GLFW_GAMEPAD_AXIS_LEFT_TRIGGER:
			axisName = "Left Trigger";
			break;
		case GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER:
			axisName = "Right Trigger";
			break;
		}
	}
	return axisName;
}

std::string JoystickStatePrinter::convertHatInputToText(char hatState) const {
	std::string stateMessage;
	switch (hatState) {
	default:
		stateMessage = "Unrecognized State!";
		break;
	case (GLFW_HAT_CENTERED):
			stateMessage = "Centered";
			break;
	case (GLFW_HAT_UP):
		stateMessage = "UP";
		break;
	case (GLFW_HAT_DOWN):
		stateMessage = "DOWN";
		break;
	case (GLFW_HAT_LEFT):
		stateMessage = "LEFT";
		break;
	case (GLFW_HAT_RIGHT):
		stateMessage = "RIGHT";
		break;
	case (GLFW_HAT_RIGHT_UP):
		stateMessage = "RIGHT UP";
		break;
	case (GLFW_HAT_RIGHT_DOWN):
		stateMessage = "RIGHT DOWN";
		break;
	case (GLFW_HAT_LEFT_UP):
		stateMessage = "LEFT UP";
		break;
	case (GLFW_HAT_LEFT_DOWN):
		stateMessage = "LEFT DOWN";
		break;
	}

	stateMessage.append("  (Read Value = ");
	stateMessage.append(std::to_string(static_cast<unsigned int>(hatState)));
	stateMessage.append(")");

	return stateMessage;
}