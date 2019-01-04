//File:          GLFrameworkMonitorEventCallback.cpp
//
//Description:   Implementation file for the GLFrameworkMonitorEventCallback function and 
//               its related components. 
//
//               I am going to try something here. The implementation is based off the Singleton pattern as laid out
//               in section 47 of Scott Meyer's book "Effective C++ 2e" (from 1998!). The idea is to hide each 
//               static object inside a function returning a reference to it. Pretty straight forward.
//
//
//               This implementation is operating somewhat under the assumption that it is highly unlikely 
//               for there to be a very large number of monitor connection events to trigger over the span
//               of a single frame. At least if for some reason there were to be many connections at one instant,
//               this code would not be the cause of any bottleneck.
//
//
//  About Assigning the Callback Function:    As of now this file assumes that the callback function 
//                                            for processing the Monitor events will be assigned else-
//                                            where in the code. 
//
//
//Programmer:    Forrest Miller
//Date:          January 2, 2019


#include "GLFrameworkMonitorEventCallback.h"

namespace GLFrameworkInternal {

	//-------------------------------------------------------
	//
	//        Implementation of the public interface
	//
	//-------------------------------------------------------

	bool checkMonitorEventQueues() {
		if ((MonitorEventCallbackInternal::recentMonitorConnections().empty()) &&
			(MonitorEventCallbackInternal::recentMonitorDisconnections().empty())) 
			return false;
		return true;
	}


	bool checkMonitorConnectionEventQueue() {
		if ((MonitorEventCallbackInternal::recentMonitorConnections().empty()))
			return false;
		return true;
	}

	
	bool checkMonitorDisconnectionEventQueue() {
		if (MonitorEventCallbackInternal::recentMonitorDisconnections().empty())
			return false;
		return true;
	}


	std::optional<GLFWmonitor*> getNextAvailableMonitorConnection() {
		std::optional<GLFWmonitor*> monitorHandle = std::optional<GLFWmonitor*>(/*empty*/);
		if ((MonitorEventCallbackInternal::recentMonitorConnections().empty())) 
			return monitorHandle; //return the empty optional
		else {
			monitorHandle = std::make_optional<GLFWmonitor*>(MonitorEventCallbackInternal::recentMonitorConnections().front());
			MonitorEventCallbackInternal::recentMonitorConnections().pop(); //Remove monitor from front of queue
			return monitorHandle;
		}
	}


	std::optional<GLFWmonitor*> getNextAvailableMonitorDisconnection() {
		std::optional<GLFWmonitor*> monitorHandle = std::optional<GLFWmonitor*>(/*empty*/);
		if ((MonitorEventCallbackInternal::recentMonitorDisconnections().empty()))
			return monitorHandle; //return the empty optional
		else {
			monitorHandle = std::make_optional<GLFWmonitor*>(MonitorEventCallbackInternal::recentMonitorDisconnections().front());
			MonitorEventCallbackInternal::recentMonitorDisconnections().pop(); //Remove monitor from front of queue
			return monitorHandle;
		}
	}


	namespace MonitorEventCallbackInternal {

		//-------------------------------------------------------
		//
		//   Monitor Connection and Disconnection Event Queues
		//
		//-------------------------------------------------------

		//Two queues are declared following the Singleton pattern. These queues are responsible for 
		//storing the handles provided by GLFW to the monitorEventCallback function until they 
		//can be processed by the application.

		std::queue<GLFWmonitor*>& recentMonitorConnections() {
			static std::queue<GLFWmonitor*> awaitingConnection;
			return awaitingConnection;
		}

		std::queue<GLFWmonitor*>& recentMonitorDisconnections() {
			static std::queue<GLFWmonitor*> awaitingDisconnection;
			return awaitingDisconnection;
		}

		//-------------------------------------------------------
		//
		//   The MonitorEventCallback function to be assigned to GLFW 
		//
		//-------------------------------------------------------
		

		void graphicsLanguageFrameworkMonitorEventCallbackFunction(GLFWmonitor* handle, int event) {
			if (event == GLFW_CONNECTED) {
				recentMonitorConnections().push(handle);
			}
			else if (event == GLFW_DISCONNECTED) {
				recentMonitorDisconnections().push(handle);
			}
			else {
				fprintf(MSGLOG, "\nWARNING! Monitor Event Callback Function was called with an invalid Event code!\n");
			}
		}


	} //namespace MonitorEventCallbackInternal

} //namespace GLFrameworkInternal