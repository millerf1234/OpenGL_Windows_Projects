//File:                       JoystickBase.h
//Class:                      JoystickBase
//
//Description:                Wrapper class for a generic 'JoystickBase' controller. Essentially this
//                               class's purpose is to exist as a wrapper for any device the GLFW library
//                               recognizes as a 'JoystickBase'. This class is seperate from the rest of GLFW
//                               however because unlike pretty much every other part of GLFW that is in some
//                               way connected, Joysticks/Controllers/Gamepads are treated globally across 
//                               an entire program. The only restriction is that GLFW should be initialized
//                               first.
//     
//+--------------------------+----------------------------------------------------------------------------------------------+
//|     !!! IMPORTANT !!!    |   To ensure proper program execution, careful attention should be paid to due to the         |
//|                          |     external and physical nature of USB Joystick Controllers, there is always a risk         |
//|      Please Read --->    |     of a controller disconnect event occuring at any time during the course of a program's   |
//|                          |     execution. This class provides an internal flag representing the controller's            |
//|                          |     connection state; as soon as a disconnection event occurs that flag will be set to mark  |
//|                          |     this controller as having been disconnected. The connection status of each controller    |
//|                          |     can be polled through the use of the provided public member function:                    |
//|                          |               bool connected()                                                               |
//|                          |   While this class and it's derived types should be able to prevent any major catastrophes   |
//|                          |     arising from trying to read input from a disconnected controller, it is HIGHLY           |
//|                          |     recommended to poll each controller's connection status before reading input.            |
//+--------------------------+----------------------------------------------------------------------------------------------+
//
//
//
//  //////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  //         Joystick Implementation Details:                                                                 //
//  //                     [Only need to know this if planning on adding additional Joystick derived types]     //
//  //////////////////////////////////////////////////////////////////////////////////////////////////////////////
//         It is highly likly that at some point in the future it will be desired to add additional joystick types
//             which derive from this class. However to properly implement these new types, it is vital to understand
//             a few things about the inner-workings of this class and GLFW's joystick API functionality.
//         First off, the most important detail to grasp is how GLFW communicates controller-disconnections.
//             How it works is GLFW alerts an application to a 'Joystick Event' through the use of a callback function.
//              A 'Joystick event' is one of 2 possibilities:  
//                         1) a new joystick gets connected; or
//                         2) a connected joystick becomes disconnected
//         Since GLFW is a C-API, there are some restrictions on what functions are allowed to be used as callbacks,
//             with the biggest restriction being that no class-member functions are allowed. This raises the dilemma 
//             then of how does a non-member callback function manage to modify an encapsulated data-field member of 
//             a JoystickBase object? (i.e. this class's member variable 'mConnected_' needs to be set to false in
//             response to a controller disconnection event.
//         The solution is composed of several parts, each of which will seem unconventional when encountered without
//             any explanation. Here is a run-through of how it works:
//
//                   (a) Luckily, GLFW make available to applications the option to assign a 'User-Pointer' to each
//                          joystick it tracks. The user-pointer is simply a void* that GLFW promises not to modify
//                          over the course of the lifespan of a joystick. Thus, the first part of the strategy is
//                          to set each GLFW Joystick's user-pointer to an instance of this class (JoystickBase).
//                          This will allow for specific instances of JoystickBase to be accessed from within the 
//                          callback function without having to rely on class-independent methods (such as a static
//                          data structure). However, we still have the issue of 'mConnected_' being private. 
//             So...       
//                   (b) We can be super tricky and take advantage of a C++ feature known as "Friendship". To put
//                          it more precisely, we can declare our callback function as a friend of our JoystickBase
//                          class. Fortunatly it just so happens that friend functions of a class do not count as 
//                          member functions while still being able to access all of the inner workings of the class
//                          they are declared friends of. 
//
//          Putting those 2 pieces together, the lifecycle of a physical joystick will look as follows (events are 
//            in chronological order):
//
//               (i)      GLFW is initalized.
//
//               (ii)     Every physically connected Joystick device which is plugged in during GLFW's initialization gets
//                           detected by GLFW. GLFW assigns a unique ID to each Joystick (up to GLFW_JOYSTICK_LAST,
//                           which represent the maximum supported number of connected Joysticks allowed by GLFW
//                           at one time). 
//
//               (iii)    The application sets up the JoystickEventCallback function using the friend function declared
//                           within the JoystickBase class.
//
//               (iv)     Application querries GLFW to discover if there exist any connected Joysticks and if so,
//                          aquires their IDs from GLFW.
//
//               (v)      For each ID, the application creates a JoystickBase object. This object will be the interface 
//                          between what GLFW reports as input from the physical controller and the Application. The
//                          JoystickBase object queries GLFW to fill in additional information about the joystick.
//
//               (vi)     As the final step of a successful JoystickBase's construction, the object automatically assigns
//                          itself to its GLFW counterpart's UserPointer. This simplfies the responsibility for the rest
//                          of the Application's code in that the entire GLFW-Joystick-to-JoystickBase entanglement process
//                          is accomplished through a single constructor call.
//
//              (vii-a)   In the event that a new controller is connected, step 2 followed by steps 4-6 are performed for 
//                           the newly connected controller. 
//
//              (vii-b)   In the event that a connected controller gets disconnected, GLFW calls the callback function, which
//                           is able to use the attached user-pointer for the controller that triggered the event to mark 
//                           its member field to reflect the controller being disconnected.
//
//      
//   One Last Important Detail:
//             It should be noted that because JoystickBase allows for move operations to be performed, it is vital that
//               each move operation also ensures the GLFW's Joystick User-pointer is updated to reflect any changes made to
//               where the JoystickBase is stored.
//-----------------------------------------------------------------------------------------------------------------------------
//
//    UPDATE:  24 hours after I intially started writing this class, I now have a much more thourough grasp on how 
//             GLFW and Joysticks interact with eachother and the App. All four of the concerns I list below I now
//             have answers too. Unfortunatly, with more knowledge comes more questions. I believe it will be wise 
//             to investigate the properties of certain joysticks which GLFW recognizes as meeting a criteria to 
//             be called a 'gamepad', and there is some sort of link between GLFW and SDL for keeping a document that
//              allows the very many different types and vareities of gamepad to be parseable and useable. Perhaps there
//              now needs to be a 'gamepad' class that inherits from this class.
//            I have also discovered that each joystick has a unqiue value known as its GUID/UUID. Perhaps I will modify 
//             this class to track joysticks by their GUID, which will remain constant even if several controllers are 
//              connected/disconnected randomly. They may not be completly unique though, more testing is required...
//
//  IDEA(s)  (delete these next few sections once implementation is more complete): 
//                            There will be this class which wraps individual JoystickBase objects, and 
//                               then probably I will break my rule and create a 'manager' class that gets 
//                               attached to the application and handles all JoystickBase instances in a global 
//                               manner. Not sure how this will look yet...
//                            I am also considering having there exist several derived classes from this one
//                               that can wrap specific controller types. It looks as though GLFW provides some
//                               basic properties shared by all joysticks, plus the option to get an array of 
//                               pointers to available axis's and available buttons. I will have to do some 
//                               experimenting to see if this would be plausible...
//                               Which brings me too... 
//   Worries/Concerns:        I am worried I won't know how to determine controller type without making an 
//                               educated guess based off the detected properties (i.e. button count and axis 
//                               count, etc...). Perhaps it won't be an issue. Luckily I have several 
//                               different types of USB controllers readily available which I can experiment
//                               with.
//                            I am also worried about the very unlikly event that multiple controllers get
//                               unplugged during a multiplayer game, and then upon being reconnected they
//                               fail to get reassigned to the correct ID's. I am not sure yet whether to
//                               just delete outright a controller in the event it gets unplugged or to 
//                               keep a record of it around (until program is closed) in case it gets 
//                               reconnected at a future time. It sounds like controller names are not 
//                               unique.
//
//
//  Reference:                https://www.glfw.org/docs/3.3/input_guide.html#joystick
//                            http://discourse.glfw.org/t/glfw-controller-database/768
//                            https://www.glfw.org/docs/3.3/group__input.html#gaed5104612f2fa8e66aa6e846652ad00f
//
//  Programmer:               Forrest Miller
//  Date:                     December 27, 2018

#pragma once

#ifndef JOYSTICK_H_
#define JOYSTICK_H_

#include <string>

#include "ProjectSetup.h"


//Note that chances are any joystick which is assigned the type 'GENERIC' probably hasn't been
//tested by me at all and thus may behave unconventionally. 
enum class JoystickType { XBOX_ONE, GAMECUBE, NES, GENERIC, INVALID};

class JoystickBase {
public:
	//----------------------------\\
	//        Constructor         \\
	//----------------------------\\

	//A unique ID number is required for construction. Please note that as part of 
	//this object's construction process, GLFW will be queryed based off the ID provided
	//as this constructor's parameter to verify that this controller is indeed connected.
	JoystickBase(int joystickToken); 
	
	//----------------------------\\
	//         Destructor         \\
	//----------------------------\\

	virtual ~JoystickBase() = default;

	//----------------------------\\
	//            Move            \\
	//----------------------------\\

	JoystickBase(JoystickBase&&);
	JoystickBase& operator=(JoystickBase&&);

	 //----------------------------------//
	//  Public State-Querying Functions  //
   //------------------------------------//

	bool connected() const { return mConnected_; }
    //Retrieves this objects uniquely assigned joystickID
	int tokenID() const { return mTokenID_; }

	JoystickType getType() const { return mType_; }

	//-----------------------------\\
	//     Overloaded Operators    \\
	//-----------------------------\\

	//Returns true if controller is connected and false if controller is disconnected
	explicit operator bool() const { return mConnected_; }

	//First checks for self-equality. Once it has been determined the two Joysticks are
	//not stored at the same memory address (self-equality), this operator next verifies each
	//controller's connection flag. If either (or both) controllers are disconnected, this
	//operator returns false. Otherwise, this operator compares each joystick's
	//uniquly-assigned ID for equality. 
	bool operator==(const JoystickBase& that) const;
	//First checks for self-equality. Once it has been determined the two Joysticks are
	//not stored at the same memory address (self-equality), this operator next verifies each
	//controller's connection flag. If either (or both) controllers are disconnected, this
	//operator returns true. Otherwise, this operator compares each joystick's
	//uniquly-assigned ID for inequality. 
	bool operator!=(const JoystickBase& that) const;

	//----------------------------\\
	//   Disabled Functionality   \\
	//----------------------------\\
	
	//No default contructor
	JoystickBase() = delete;
	//No Copy constructor
	JoystickBase(const JoystickBase&) = delete;
	//No Copy operator
	JoystickBase& operator=(const JoystickBase&) = delete;

protected: //These are basically the fields that are directly assignable from GLFW
	bool mConnected_; 
	int mTokenID_; 
	JoystickType mType_;
	std::string mJoystickName_;


private: 
	//Use a flag to verify if this object successfully gets assigned to a GLFW Joystick's UserPointer
	bool mWasAssignedAsJoystickUserPointer_;
	
	//Internal Utility functions
	void initialize();
    

	//-----------------------------------------------\\
	//    GLFW Joystick Event Callback Function      \\      (Note that this is not actually a member function)
	//-----------------------------------------------\\

	friend void joystickEventCallback(int joyID, int connected);  //Second parameter is either GLFW_CONNECTED or GLFW_DISCONNECTED

};


#endif //JOYSTICK_H_