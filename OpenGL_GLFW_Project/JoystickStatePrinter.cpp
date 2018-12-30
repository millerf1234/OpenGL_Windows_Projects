#include "JoystickStatePrinter.h"

#include <sstream>

#define PRINT_ALL_BUTTON_STATES

void JoystickStatePrinter::printState() {
	if (glfwJoystickPresent(mID_)) { havePrintedTheDisconnectedMessage = false; }
	if (havePrintedTheDisconnectedMessage) { return; }  //No need to print this repeatedly

	std::stringstream stateMsg;

	//Put a header at the top of the message
	stateMsg << "\n\n";
	stateMsg << "+------------------------------------------------------------------------------+\n";
	stateMsg << "|vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv|\n";
	stateMsg << "+------------------------------------------------------------------------------+\n";
	stateMsg << "                        Current State of Gamepad: " << mName_ << "\n";
	stateMsg << "\tConnection Status: ";
	
	//Find out if the Joystick is connected
	if (glfwJoystickPresent(mID_)) {
		stateMsg << "CONNECTED\n";
	}
	else {
		stateMsg << "DISCONNECTED\n";
		havePrintedTheDisconnectedMessage = true;
	}

	if (!havePrintedTheDisconnectedMessage) { //Use as flag for only printing out information for a connected controller
		
		//Joystick Axis State
		int axes;
		const float * axis = glfwGetJoystickAxes(mID_, &axes);
		if (axes > 0) {
			stateMsg << "Read Axis values: \n";
			for (int i = 0; i < axes; i++) {
				stateMsg << "Axis" << i << ": " << axis[i] << "\t";
				if ((i % 3) == 2) {
					stateMsg << "\n";
				}
			}
			if ((axes % 3) != 2) {
				stateMsg << "\n";
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
#if defined PRINT_ALL_BUTTON_STATES
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
#else 
			for (int i = 0; i < buttons; i++) {
				if (buttonStates[i] == GLFW_PRESS) {
				stateMsg << "Button[" << i << "] is ";
					"PRESSED";
					stateMsg << "    (Predicted Button Name is " << predictButton(i) << ")\n";
				}
		    }
#endif 
		}
		else {
			stateMsg << "Joystick has no available buttons to querry the state of...\n";
		}



		//Joystick Hat States
		int hats;
		const unsigned char* hatStates = glfwGetJoystickHats(mID_, &hats);
		if (hats > 0) {
			stateMsg << "  [todo: Add hat state printing]\n";

		}
		else {
			stateMsg << "Joystick has no hats to report the state of!\n\n";
		}

	}


	//Place an ending to the message to match the header
	stateMsg << "+------------------------------------------------------------------------------+\n";
	stateMsg << "|^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^|\n";
	stateMsg << "+------------------------------------------------------------------------------+\n";

	fprintf(MSGLOG, "%s", stateMsg.str().c_str());
}




std::string JoystickStatePrinter::predictButton(int buttonIndex) const {
	return "[Guess To be implemented...]";
}