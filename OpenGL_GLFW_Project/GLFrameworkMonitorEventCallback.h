//File:                               GLFrameworkMonitorEventCallback.h
//
// Description:                       Provides an interface for responding to Monitor Callback events.
//                                    See anticiapted usgae section.
//
//
// Anticipated Usage Pattern:         Once every frame or so, the application should call
//                                    the function 'checkMonitorEventQueues()'. It is expected
//                                    that 99.99% of the time this will return false, but in the 
//                                    event that this function does return true, the additional
//                                    functions can be used to poll both queues to discover if either 
//                                    (or both) has recorded an event. 
//                                    The queues here do not perform any validation on the data they
//                                    get passed through the Operating System's callback, so please
//                                    ensure the Application validates every event.
//                                    
//
//
//                                    
// Programmer:                        Forrest Miller
// Date:                              January 2, 2019

#pragma once

#ifndef GL_FRAMEWORK_MONITOR_EVENT_CALLBACK_H_
#define GL_FRAMEWORK_MONITOR_EVENT_CALLBACK_H_

#include <queue>
#include <sstream>
#include <optional>

#include "GlobalIncludes.h"  

namespace GLFrameworkInternal {

	///////////////////////////////////////////////////////////////////////////////////////
	////////////      Interface to be used by the rest of the Application      ////////////
	///////////////////////////////////////////////////////////////////////////////////////

	//Returns true if there are any monitor connection/disconnection events that
	//occured recently which require processing by the Application. Returns false if
	//no monitor events require processing.
	bool checkMonitorEventQueues();

	//Check the Monitor connection event queue to see if any Monitor connection 
	//events have recently occured which would require processing. Returns false
	//if there are no connection events to process.
	bool checkMonitorConnectionEventQueue();

	//Check the Monitor disconnection event queue to see if any Monitor disconnection 
	//events have recently occured which would require processing. Returns false
	//if there are no disconnection events to process.
	bool checkMonitorDisconnectionEventQueue();

	//Retrieves the next available Monitor Connection event off the 
	//Connection event queue and returns an optional-wrapped GLFWmonitor*
	//for the newly connected monitor. Be aware that if the Monitor 
	//Connection event queue is empty when this function is called, the 
	//returned optional will be empty.
	//WARNING! Despite being wrapped in the std::optional by this 
	//MonitorEvent queue system, no validation has yet been performed
	//on the underlying GLFWmonitor*. Please ensure to validate the 
	//value before using it (e.g. it could be a nullptr)
	std::optional<GLFWmonitor*> getNextAvailableMonitorConnection();

	//Retrieves the next available Monitor Disconnection event off the 
	//Disconnection event queue and returns an optional-wrapped GLFWmonitor*
	//for the recently disconnected monitor. Be aware that if the Monitor 
	//Disconnection event queue is empty when this function is called, the 
	//returned optional will be empty. 
	//WARNING! Despite being wrapped in the std::optional by this 
	//MonitorEvent queue system, no validation has yet been performed
	//on the underlying GLFWmonitor*. Please ensure to validate the 
	//value before using it (e.g. it could be a nullptr)
	std::optional<GLFWmonitor*> getNextAvailableMonitorDisconnection();


	//Declaration of callback function to assign to GLFW and the two queue 'functions,' 
	//don't call
	namespace MonitorEventCallbackInternal {
		void graphicsLanguageFrameworkMonitorEventCallbackFunction(GLFWmonitor*, int);
		std::queue<GLFWmonitor*>& recentMonitorConnections();
		std::queue<GLFWmonitor*>& recentMonitorDisconnections();
	} //namespace MonitorEventCallbackInternal

} //namespace GLFrameworkInternal

#endif //GL_FRAMEWORK_MONITOR_EVENT_CALLBACK_H_