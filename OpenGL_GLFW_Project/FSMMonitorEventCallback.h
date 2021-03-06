//File:                               FSMMonitorEventCallback.h
//
// Description:                       Provides an interface for responding to Monitor Callback events.
//                                    See 'Anticiapted Usage' section for proper usage.
//
//
// Anticipated Usage Pattern:         Once every frame or so, the application should call
//                                    the function 'checkMonitorEventQueues()'. It is expected
//                                    that 99.99% of the time this will return false, but in the 
//                                    event that this function does return true, the additional
//                                    functions can be used to poll both the ConnectionEvent and 
//                                    DisconnectionEvent queues to discover if either (or both)
//                                    has an event awaiting further processing. 
//                            
// Other info:                        IMPORTANT! Please be aware that the queues here will not perform
//                                    any validation on the data they receive from OS through the callback,
//                                    so please ensure the Application validates every event (i.e. check
//                                    to make sure the GLFWmonitor* is not NULL; if woried about security
//                                    then after a connection event check to make sure the GLFWmonitor* for
//                                    new monitor is contained in array returned by 'glfwGetMonitors()', 
//                                    etc...)
//                                    
//                                    
// Programmer:                        Forrest Miller
// Date:                              January 2, 2019

#pragma once

#ifndef FSM_MONITOR_EVENT_CALLBACK_H_
#define FSM_MONITOR_EVENT_CALLBACK_H_

#include <queue>
#include <sstream>
#include <optional>

#include "GlobalIncludes.h"  

namespace FSMEngineInternal {

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


	//////////////////////////////////////////////////
	//////   Implementation Details, Can Ignore   ////
	//////////////////////////////////////////////////
	namespace MonitorEventCallbackInternal {
		//Declaration of callback function to assign to GLFW and prototypes the two queue 'functions,' 
		//don't call these functions 
		void graphicsLanguageFrameworkMonitorEventCallbackFunction(GLFWmonitor*, int);
		std::queue<GLFWmonitor*>& recentMonitorConnections();
		std::queue<GLFWmonitor*>& recentMonitorDisconnections();
	} //namespace MonitorEventCallbackInternal

} //namespace FSMEngineInternal

#endif //GL_FRAMEWORK_MONITOR_EVENT_CALLBACK_H_