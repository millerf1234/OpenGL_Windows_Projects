
//This is just a test class to help me understand how various Joystick 
//objects behave. This is not intended to be a permanent part of the project.

#pragma once

#include "ProjectSetup.h"
#include "Joystick.h"

class JoystickStatePrinter final {
public:
	JoystickStatePrinter() : JoystickStatePrinter(0) { ; }

	JoystickStatePrinter(int id) : mID_(id) { 
		havePrintedTheDisconnectedMessage = false;
		if (glfwJoystickPresent(id)) {
			mName_ = glfwGetJoystickName(id);
		}
		else {
			mName_ = "[NO_JOYSTICK_AVAILABLE]";
		}
	}

	void printState();

	std::string predictButton(int buttonIndex) const;

	~JoystickStatePrinter() = default;
	JoystickStatePrinter(const JoystickStatePrinter&) = default;
	JoystickStatePrinter& operator=(const JoystickStatePrinter&) = default;
	JoystickStatePrinter& operator=(JoystickStatePrinter&&) = default;

private:
	bool havePrintedTheDisconnectedMessage;
	int mID_;
	//MappedGamepad mDetails_;
	//JoystickInfo mState_;
	std::string mName_;
};

