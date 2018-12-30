#include "MappedGamepad.h"



MappedGamepad::MappedGamepad(int id) : mJoyID_(id) {
	if (!glfwJoystickIsGamepad(id)) {
		mHasMapping_ = false;
		mGamepadMappingName_ = "[UNIDENTIFIED MAPPING]";
		mState_ = nullptr;
	}
	else if (glfwJoystickPresent(id)) {
		mHasMapping_ = true;
		mGamepadMappingName_ = glfwGetGamepadName(id);
		int success = glfwGetGamepadState(id, mState_);
		if (success != GLFW_TRUE) {
			mHasMapping_ = false;
			mState_ = nullptr;
		}
	}
}


MappedGamepad::MappedGamepad(MappedGamepad&& that) noexcept {
	mJoyID_ = that.mJoyID_;
	mHasMapping_ = that.mHasMapping_;
	that.mHasMapping_ = false;
	mGamepadMappingName_ = that.mGamepadMappingName_;
	mState_ = that.mState_;
	that.mState_ = nullptr;
}


MappedGamepad& MappedGamepad::operator=(MappedGamepad&& that) noexcept {
	if (this != &that) {
		mJoyID_ = that.mJoyID_;
		mHasMapping_ = that.mHasMapping_;
		that.mHasMapping_ = false;
		mGamepadMappingName_ = that.mGamepadMappingName_;
		mState_ = that.mState_;
		that.mState_ = nullptr;
	}
	return (*this);
}

GLFWgamepadstate MappedGamepad::captureGamepadState() {
	if (mHasMapping_) {
		if (mState_ != nullptr) {
			//Then record a copy of the current state
			return (*mState_);
		}
	}
	
	GLFWgamepadstate emptyState;
	//Luckily the maximum number of buttons and axises is hardcoded into GLFW, so 
	//all the code here needs to do is set each of emptyState's data to the most neutral
	//option
	for (int i = 0; i < 15; i++) { 
		emptyState.buttons[i] = GLFW_RELEASE; //Set all buttons to release
	}
	for (int i = 0; i < 6; i++) {
		emptyState.axes[i] = 0.0f;
	}
	return emptyState;
}


