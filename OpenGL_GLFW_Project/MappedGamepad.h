//File:                                  Gamepad.h
//Class:                                 Gamepad
//
//   Description:   Wraps a GLFWgamepadstate object for reading input plus a small
//                  extra bit of Gamepad data. 
//
//
// Programmer:       Forrest Miller
// Date:             December 28, 2018
//

#pragma once

#ifndef MAPPED_GAMEPAD_H_
#define MAPPED_GAMEPAD_H_

#include <string>
#include "ProjectSetup.h"

class MappedGamepad final {
public:
	MappedGamepad(int id);
	//Moving is enabled for this type
	MappedGamepad(MappedGamepad&&) noexcept;
	MappedGamepad& operator=(MappedGamepad&&) noexcept;

	~MappedGamepad() = default;

	MappedGamepad() = delete; //No parameterless constructor
	MappedGamepad(const MappedGamepad&) = delete; //No copying
	MappedGamepad& operator=(const MappedGamepad&) = delete; //No Copy-Assigning

	//---------------------------------------------------------------------
	//     GAMEPAD INTERFACE
	//---------------------------------------------------------------------

	//Checks to see if a compatible mapping is available for this MappedGamepad. Should
	//always be true unless something went wrong.
	bool hasMappingAvailable() const { return mHasMapping_; }
	//Returns a copy of the current state of the gamepad. Note this
	//is not returning a pointer to where the gamepad is dynamically
	//updating too, so this function needs to be called for each update.
	GLFWgamepadstate captureGamepadState();

	//Returns the name associated with this object's current gamepad mapping
	std::string gamepadMappingName() const { return mGamepadMappingName_; }

	

private:
	bool mHasMapping_; //Not all joysticks will have their mappings available
	std::string mGamepadMappingName_;
	int mJoyID_;
	GLFWgamepadstate* mState_;
};



#endif //MAPPED_GAMEPAD_H_