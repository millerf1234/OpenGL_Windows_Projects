//File:                          JoystickManager.cpp
//
//Description:                   Implementation file for the JoystickManager class. 
//
//
//Programmer:                     Forrest Miller
//Date:                           December 28, 2018


#include "JoystickManager.h"


//------------------------------------------------------
//            Initialize Static Members
//------------------------------------------------------

//    This class operates around a static array holding all
//    of the currently connected Joysticks. The following
//    variables are used to keep track of the status of 
//    this static array:

//A Joystick-connection event callback function is assigned
//to GLFW to allow new Joysticks to add themselves to the
//static Joystick array. Since this callback function is set
//globally, this static flag marks whether the function has
//been assigned yet
bool JoystickManager::joystickEventCallbackFunctionIsAssigned = false;

//As the number of connected Joysticks grows and shrinks, 
//this static variable activly keeps track of the total number 
//of Joysticks connected at the current time.
std::atomic_size_t JoystickManager::numberOfConnectedJoysticks = 0u;




JoystickManager::JoystickManager() {
	mSetupComplete_ = false;
	initializeJoystickArray();

}


void JoystickManager::runSetup() {
	setupJoystickEventCallback();
	detectAllCurrentlyConnectedJoysticks();

}


void JoystickManager::initializeJoystickArray() {
	for (size_t joystickArrayInitializer = 0u;
		joystickArrayInitializer < static_cast<size_t>(GLFW_JOYSTICK_LAST);
		joystickArrayInitializer++) {
		///mJoysticks_[joystickArrayInitializer] = nullptr;
	}
}


void JoystickManager::setupJoystickEventCallback() {
	//glfwSetJoystickCallback(joystickEventCallback);
}

void JoystickManager::detectAllCurrentlyConnectedJoysticks() {
	for (int i = 0; i < GLFW_JOYSTICK_LAST; i++) {
		int present = glfwJoystickPresent(i);
		if (present == GLFW_TRUE) {
			///mNumberConnectedJoysticks_++;
			///mJoysticks_[i] = std::make_unique<Joystick>(i);
			//While we are here, might as well just attach it to the window's user pointer...
			///glfwSetJoystickUserPointer(i, static_cast<void *>(&mJoysticks_));
		}
	}
}

//This is the callback function that will be run by GLFW any time a Joystick is
//connected or disconnected. 
void joystickEventCallback(int joyID, int event) {
	//Important to remember: If the event is GLFW_DISCONNECTED, then there are
	//only 2 available functions which will return correct answers. These are 
	//getting the Joystick name and getting its userpointer.
	if (event == GLFW_DISCONNECTED) {
		std::string nameOfDisconnectedJoystick = glfwGetJoystickName(joyID);
		fprintf(MSGLOG, "Connection lost to %s\n", nameOfDisconnectedJoystick.c_str());

		void * userPointer = glfwGetJoystickUserPointer(joyID);
		if (userPointer) {
			//It is assumed that the user-pointer assigned to each Joystick is only
			//ever done set by this, the JoystickManager class, so this following pointer 
			//dereference isn't as bad as it seems...
			auto joystickArray = static_cast<std::array<std::unique_ptr<Joystick>, GLFW_JOYSTICK_LAST>*>(userPointer);
			//So now we have a pointer to the array of Joysticks. Thus we can locate the joystick object in the array
			//that was representing the disconnected joystick. 
			//We can confirm...
			if ((*joystickArray)[joyID] == nullptr) {
				fprintf(ERRLOG, "\nOOps! Trying to dereference a nullptr. Bad idea!\n");
				return;
			}
			else {
				//int joystickIDThatWasRemovedInCallback = (*joystickArray)[joyID]->tokenID;
				/*fprintf(MSGLOG, "\n\nIn the Joystick callback, it was determined that as "
					"a result of\nJoystick %s having been disconnected physically, the\n"
					"corrosponding Joystick in the stored array of joysticks with id %d "
					"is a match for deletion!\n\n", joyID, joystickIDThatWasRemovedInCallback); */
				//delete the joystick
				//(*joystickArray)[joyID] = nullptr;
			}
		}
	}
	else if (event == GLFW_CONNECTED) {
		

		//Then set up a new joystick object from newly connect joystick, print out a message 
		//about the new joystick to the event log, and finally   ...arrg it might not work....

	}
}