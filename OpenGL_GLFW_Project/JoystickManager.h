//File:                          JoystickManager.h
//
//Description:                   I am breaking one of my closely-followed rules with this class by it
//                               bearing the forbidden name of 'manager', but for it's intended purpose
//                               which is the task of providing a predictable and robust user experience 
//                               for user's despite having to deal with responding to the chaotic
//                               nature of Joystick Events, having a well behaved 'manager' class that 
//                               lives as part of the GraphicsLanguageFramework.
//
//
//                               The GLFW library assigns to the macro GLFW_JOYSTICK_LAST the maximum
//                               number of joysticks that can be connected at the same time, which allows
//                               for this class to be built around a static array representing every
//                               currently connected Joystick.                       
//                                        
//                               
//
//Programmer:                    Forrest Miller
//Date:                          December 28, 2018

#pragma once

#ifndef JOYSTICK_MANAGER_H_
#define JOYSTICK_MANAGER_H_

#include <array>
#include <atomic>
#include <memory>
#include "LoggingMessageTargets.h"
#include "Joystick.h"

class JoystickManager final {
public:
	//Calling this constructor will bring a JoystickManager into existence, but note that the 
	//required setup for a completely functional JoystickManager must involve an additional 
	//function call. This is good news though, because this object can be constructed without 
	//first requiring any dependent libraries (*cough* GLFW *cough) to be initialized. 
	//
	// (TL;DR) Be sure to call this object's 'runSetup()' function after GLFW is initialized
	JoystickManager();

	~JoystickManager() noexcept { ; };

	//This function should be called ideally as soon as possible after GLFW is successfully
	//initialized. There are a number of different steps that get performed by this class
	//which are necessary to allow it to interface with the GLFW library and every connected
	//controller.  
	void runSetup(); 

	//------------------------------------//
	//    Joystick-Related Functions      //
	//------------------------------------//

	//Returns the number of currently connected Joysticks
	size_t activeCount() const noexcept { return numberOfConnectedJoysticks.load(); }


	//Returns true if the Joystick event callback function was successfully setup. If something
	//went wrong with setting up the callback, returns false.
	bool joystickCallbackFunctionWasAssigned() const { return joystickEventCallbackFunctionIsAssigned; }

	//Checks to see if this object was able to complete all parts of its setup
	//without failure
	bool setupCompleted() const { return mSetupComplete_; }

	
private:
	static bool joystickEventCallbackFunctionIsAssigned; //Will become true if assigned successfully as part of setup
	static std::atomic_size_t numberOfConnectedJoysticks;
	static std::array<std::unique_ptr<Joystick>, GLFW_JOYSTICK_LAST> joystickArray;

	bool mSetupComplete_;
	
	
	


	//----------------------------------------------//
	//             Utility Functions                //
	//----------------------------------------------//
	void initializeJoystickArray(); //Initializes every element in array to nullptr
	void setupJoystickEventCallback();
	void detectAllCurrentlyConnectedJoysticks(); 



	
	//-----------------------------------------------\\
	//    GLFW Joystick Event Callback Function      \\      (Note that this is not actually a member function)
	//-----------------------------------------------\\

	friend void joystickEventCallback(int joyID, int event); //Second parameter is either GLFW_CONNECTED or GLFW_DISCONNECTED
	

};

#endif //JOYSTICK_MANAGER_H_

