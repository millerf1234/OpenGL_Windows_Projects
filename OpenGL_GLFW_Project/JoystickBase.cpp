#include "JoystickBase.h"


namespace {
	static constexpr const int UNASSIGNED = -1;
}

void JoystickBase::initialize() {
	mConnected_ = false;
	mTokenID_ = UNASSIGNED;
	mType_ = JoystickType::INVALID;
	mJoystickName_ = "UNASSIGNED";

	mWasAssignedAsJoystickUserPointer_ = false;
}

JoystickBase::JoystickBase(int joystickToken) {
	initialize();
}


//JoystickBase::~JoystickBase() {
//	//I was thinking it would be nice to make absolutly sure an object of this type removes itself 
//	//as the attached user pointer of a Joystick in case it hadn't already removed itself, 
//	//but this just seems like a bad idea...
//	/* if (mWasAssignedAsJoystickUserPointer_) { //If this object still is set as a GLFW Joystick's user pointer
//		//Check to see if the Joystick object still exists
//		if (glfwJoystickPresent(mTokenID_)) {
//			void * temp = glfwGetJoystickUserPointer(mTokenID_);
//			if (temp != NULL) {
//				//See if temp is equal to this objects address
//				if (temp == static_cast<void*> (this)) {
//					glfwSetJoystickUserPointer(mTokenID_, NULL);
//				}
//			}
//		}
//	} */ //Too dangerous 
//}


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




