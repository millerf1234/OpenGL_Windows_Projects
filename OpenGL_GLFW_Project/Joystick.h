//File:                       Joystick.h
//Class:                      Joystick
//
//Description:                This class exists to serve as the primary liaison between input-sensitive
//                              components of this application and currently connected Joysticks as being
//                              reported by GLFW. 
//                         
//                              
//  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!--IMPORTANT DEPENDENCY--!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//  !!    This class requires for the GLFW library to have already been initialized* before   !!
//  !!                        usable instances can be constructed.                            !!
//  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// 
//    * The GLFW library is initialized after a single call to the function:
//          'int glfwInit()'
//      If the 'glfwInit' function returns GLFW_TRUE, the initialization was
//          a success, which unlocks the majority of this class's functionality.
//      GLFW will remain initialized until the function glTerminate() is 
//          called, upon which the data contained within these objects
//          becomes invalidated.
//                 
//
//
//   Programmer:      Forrest Miller
//   Date:            December 28, 2018                         

#pragma once

#ifndef JOYSTICK_H_
#define JOYSTICK_H_

#include <optional>
#include "MappedGamepad.h"

//A stand-alone struct which ties together all of a Joysticks
//attributes
struct JoystickInfo {
	std::string name;
	std::string guid;
	int buttonCount;
	const unsigned char * buttons;
	int numberAvailableAxes;
	const float* axes;
	int hatCount;
	const unsigned char * hats;
	JoystickInfo() { 
		name = "[unassigned]";
		guid = "[N/A]";
		buttonCount = numberAvailableAxes = hatCount = 0;
		buttons = nullptr;
		axes = nullptr;
		hats = nullptr;
	}
};

class Joystick {
public:
	//----------------------------\\
	//        Constructor         \\
	//----------------------------\\

	//A unique ID number is required for construction. Please note that as part of 
	//this object's construction process, GLFW will be queryed based off the ID provided
	//as this constructor's parameter to verify that this controller is indeed connected.
	Joystick(int joyID); 
	
	//----------------------------\\
	//         Destructor         \\
	//----------------------------\\

	~Joystick() = default;

	//----------------------------\\
	//            Move            \\
	//----------------------------\\

	Joystick(Joystick&&);
	Joystick& operator=(Joystick&&);

   //------------------------------------//
   //  Public State-Querying Functions   //
   //------------------------------------//

	bool connected() const { return mIsConnected_; }
    //Retrieves this objects uniquely assigned joystickID
	int tokenID() const { return mJoyID_; }
    //Checks to see if this joystick has a specific mapping available


	//----------------------------\\
	//   Disabled Functionality   \\
	//----------------------------\\
	
	//No default constructor
	Joystick() = delete;
	//No Copy constructor
	Joystick(const Joystick&) = delete;
	//No Copy operator
	Joystick& operator=(const Joystick&) = delete;



private:
	int mJoyID_;
	bool mIsConnected_;
	bool mHasGamepadMapping_;
	std::optional<MappedGamepad> mGamepadMapping_;

	
};

#endif //JOYSTICK_H_