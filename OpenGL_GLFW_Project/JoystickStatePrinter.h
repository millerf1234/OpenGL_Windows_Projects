// JoystickPrinter.h
//
// Description:        A class built for debugging which simply echos input
//                     read from a connected Joystick. Requires GLFW to have been initialized.
//
// How To Use:         Pass in target joystick's ID during construction to specify the 
//                     specific joystick to have input echoed from. Default construction
//                     will set target joystick ID to 0.
//
//                     Once constructed, within the same loop that glfwPollEvents() is called
//                     call this object's member function 'printState()' to have this class 
//                     print directly to the MessageLog. 
//                     Also can call getStateString() to retrieve the formated printout as 
//                     a C++ string.
//
//                     Can change which joystick is active by calling changeID()
//
//                     There are 2 modes that dictate the type of message that will be reported:
//                        (i)  The default state is to print every value being read from the joystick
//                                each iteration. This mode is useful for getting a look at every 
//                                possible value reported by the Joystick in real time.
//                        (ii) The optional alternative mode only prints out inputs reported by the
//                                controller that are different from it's default state. This means:
//                                     - Buttons will only be reported when their state evaluates
//                                          as 'pressed'
//                                     - Hats will only be reported if their state differs from 
//                                          'centered'
//                                     - Axis input will be reported only if read magnitude exceeds a
//                                          minimum neutral-dead-zone. The exception to this rule is
//                                          for triggers, which (hopefully) will begin at either -1.0
//                                          or 1.0 when the joystick is first connected.
//                                Additionally, while in this alternative mode, if a gamepad mapping was
//                                detected for the joystick, then every button-press and axis-input will
//                                be accompanied by the Gamepad-Mapped value for that input.
//                                                               
//
// Please Note:        This class will only print out a message a single time if it fails to
//                     detect a joystick located at it's target ID. Thus it is safe to call
//                     this class's member functions every loop iteration.
//
//                     This class expects that when a connection is first made with a Joystick,
//                     the Joystick will be in a neutral state. Thus connecting a controller that 
//                     is not in a neutral state (i.e. has buttons held down or has an analog stick
//                     pushed in a direction or even has a trigger depressed any amount) may cause
//                     inaccurate/unusual behavior to be reported in the printout messages. To fix
//                     this issue, reconnect the joystick while ensuring it's state is neutral.
//                     
//
// Has Dependency:     The GLFW library must have been initialized for this class to operate
//                     properly.
//
// Programmer:         Forrest Miller
// Date:               December 29-30, 2018


#pragma once

#include <string>
#include <vector>

#include "GlobalIncludes.h"

static const std::string DISCONNECTED_JOYSTICK_NAME = "[NO_JOYSTICK_AVAILABLE]";
static constexpr const float AXIS_DEAD_ZONE_SIZE = 0.085f;  //This is just an estimate ... Tweak as needed 

class JoystickStatePrinter final {
public:

	//Constructing/Deleting/Copying/Moving
	JoystickStatePrinter() : JoystickStatePrinter(0) { ; }
	JoystickStatePrinter(int id);
	~JoystickStatePrinter() = default;
	JoystickStatePrinter(const JoystickStatePrinter&) = delete;
	JoystickStatePrinter(JoystickStatePrinter&&) noexcept = default;
	JoystickStatePrinter& operator=(const JoystickStatePrinter&) = delete;
	JoystickStatePrinter& operator=(JoystickStatePrinter&&) noexcept = default;

	//------------------------------------------------------------------------------//
	//                           Public Interface                                   //
	//------------------------------------------------------------------------------//

	///////////////////////////////////
	////      Print Functions      ////
	///////////////////////////////////

	//Prints out a formated message containing the data retrieved from 
	//a joystick with target ID. If no joystick is detected, a single
	//'undetected' message is printed until a joystick reactivates this
	//object.
	void printState();
	//Returns an empty string if no Joystick is located which has the target ID
	std::string getStateString();

	///////////////////////////////////
	////       Change Target       ////
	///////////////////////////////////

	//Gets the ID of the joystick being monitored by this object
	int id() const { return mID_; }
    //Sets the ID of the joystick this object is monitoring to the specified value.
	//Values outside of the range from 0 to GLFW_JOYSTICK_LAST will be ignored.
	void changeID(int);

	//Switches target Joystick_ID to be printed by going to the next ID. Note that
	//this doesn't mean next connected Joystick. So if Joysticks 1 and 3 are connected
	//and state is state to poll joystick 1, calling this function will move polling 
	//target to Joystick 2 even though no Joystick with that ID is connected.
	void nextJoystick(); 

	//Switches target Joystick_ID to be printed by going to the previous ID. Note that
	//this doesn't skip disconnected Joysticks. So if Joysticks 1 and 3 are connected
	//and state is state to poll joystick 3, calling this function will move polling 
	//target to Joystick 2 even though no Joystick with ID==2 is connected.
	void previousJoystick();

	///////////////////////////////////
	////     Change Output Type    ////
	///////////////////////////////////
	
	//Changes the mode of output from this object. There are 2 available 
	//modes currently, one which prints the entire controllers state and
	//a second mode which only reports inputs that differ from the 
	//joysticks default state.
	void toggleMessageOutputMode();
	
	void setMode_FullStateReport() { mPrintFullJoystickState_ = true; }
	void setMode_ReportDetectedInputOnly() { mPrintFullJoystickState_ = false; }

	//Returns true if this object is currently in its alternative mode where 
	//only detected inputs are printed. Otherwise this will return false if
	//the object is in it's primary mode of printing the entire Joystick state.
	bool isUsingEchoInputPrintMode() const { return !mPrintFullJoystickState_; }


	///////////////////////////////////
	////       Reset Joystick      ////       //This only really affects the DetectedInput print mode
	///////////////////////////////////

	//Refreshes the stored internal default state for this Joystick. Useful for 
	//fixing a Joystick that wasn't in default state when it was first intialized.
	//Will not have any effect if there is no controller connected with target ID.
	void reaquireDefaultState();

	

private:
	//Object Internal Members
	bool mPrintedDisconnectMessage_;
	bool mWasSetToNewID_;
	bool mHasName_;
	std::string mName_;

	typedef struct JoystickStateAtConnection {
		int numAxes, numButtons, numHats;
		std::vector<float> initialAxesValues; //Value read from each axis
		std::vector<int> axesPredictedToBeTriggers; //Indices of each axis suspected of being a trigger
	} BaseJoyState;

	bool mHasDefaultState_;
	BaseJoyState mDefaultState_;


	//User-Configurable Members
	int mID_;
	bool mPrintFullJoystickState_;


	
	//Internal Functions: 

	//To be called whenever a new joystick is first connected, records
	//the current state of joystick to use as a default for use with
	//secondary print mode.
	void recordNewlyConnectedJoystickState(); 
	//Prepares the default-state recording struct to capture a new state.
	// Warning! [This functions should only be called from 
	// 'recordNewlyConnectedJoystickState()']
	void clearPreviouslyStoredDefaultState();
	

	std::string buildStateString();
	//Build Full-State Mode message
	void buildFullStateMessage(std::stringstream& stateMsg);
	//Build the alternative 
	void buildDetectedInputsMessage(std::stringstream& stateMsg);

	std::string predictButton(int buttonIndex) const;
	//std::string getButtonGamepadMapping(int buttonIndex) const;
	std::string getAxisGamepadMapping(int axis) const;
	std::string convertHatInputToText(char hatState) const;
};

