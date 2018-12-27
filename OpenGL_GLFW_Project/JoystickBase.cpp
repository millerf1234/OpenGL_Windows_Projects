#include "JoystickBase.h"


namespace {
	static constexpr const int UNASSIGNED = -1;
}

void JoystickBase::initialize() {
	mConnected_ = false;
	mTokenID_ = UNASSIGNED;
	mType_ = JoystickType::INVALID;
	mJoystickName_ = "UNASSIGNED";
}

JoystickBase::JoystickBase(int joystickToken) {
	initialize();
}


JoystickBase::~JoystickBase() {

}


//Comparing JoystickType should not be necessary if GLFW's assigned IDs are 
//truly unique
bool JoystickBase::operator==(const JoystickBase& that) const {
	if (this == &that) //Check for self-equality
		return true; 
	if ((!mConnected_) || (!that.mConnected_)) //Verify that both are connected
		return false;
	if (mTokenID_ == that.mTokenID_) //Compare unique IDs
		return true;
	return false;
}

//See note for equality operator as for why it shouldn't be necessary to compare 
//the two Joystick's types.
bool JoystickBase::operator!=(const JoystickBase& that) const {
	if (this == &that) //Check for self-equality
		return false;
	if ((!mConnected_) || (!that.mConnected_)) //Verify that both are connected
		return true;
	if (mTokenID_ != that.mTokenID_) //Compare unique IDs
		return true;
	return false;
}




