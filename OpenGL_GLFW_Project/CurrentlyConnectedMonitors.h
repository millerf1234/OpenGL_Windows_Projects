//File:                  CurrentlyConnectedMonitors.h
//
//          WARNING!  Somewhat Convoluted Hack Implementation Alert!
//
//Description 
//    (Background):   This class was designed to offer a solution to the issue of properly 
//                       tracking Monitor connection/disconnection events (which are triggered
//                       through GLFW's C API) within data contained within a C++ class. 
//                       First, a better discription of the issue:
//                           GLFW allows for various callback functions to be assigned which are 
//                           then called in response to the occurance of particular events. A requirement
//                           imposed on each of these callback functions is they must have a signature
//                           that exactly matches what GLFW expects. In this particular case, there is a 
//                           need to have a callback function which is called whenever a monitor connection/disconnection
//                           event transpires. GLFW allows for a callback function to be assigned for response to one of
//                           these events, which must have a function signature matching the form:
//                               void monitor_callback_func(GLFWmonitor* monitor, int event);  
//                           The issue arises from the desire to have this callback function update information
//                           that is encapsulated within a C++ class. 
//
//                           Why not just assign a class member function with a signature matching that required of 
//                           the callback function? Well, unfortunatly due to the wway C++ class member functions are
//                           implemented, each on has an implicit first arguement of type void * called 'this', which 
//                           is how they are able to refer to class member fields. 
//                           So the quick and dirty solution is to just make the member function static, which removes
//                           the implicit 'this' pointer in the function signature. But then how to update the classes 
//                           member fields?
//
//    (Justification):  Luckily, chances are that (baring code running on multiple computers) all instances of
//                           classes that would like to know about currently connected monitors probably don't mind 
//                           whether it is they or some other code that updates the list of available monitors, just
//                           that the list is accurate. Thus this class is a class that is designed to potentially have
//                           multiple objects attempt to access and modify. It has safety inherently built into itself
//                           to avoid potential issues*. Since each monitor is given an opaque pointer (handle) that is               *(yet to be backed up through testing...)
//                           assigned by GLFW and is guarenteed to not change as long as GLFW is running and the monitor 
//                           remains connected. Thus any operation requested on this object regarding adding or removing 
//                           monitors just needs to check to see if that operation has already been performed on a monitor
//                           with a matching handle. 
//
//    (Implementation):  If things aren't already confusing enough, it is recommended that one read the information outlined
//                           in the CppReference page for "std::unique_ptr::unique_ptr" found at:  https://en.cppreference.com/w/cpp/memory/unique_ptr/unique_ptr  
//                           To oversimplyfy the idea, this class is built to exist as a static entity, which is set to null 
//                           for construction, [perhaps supports reference-counting], and is shared between any number of
//                           classes that will try to modify and read from it (hopefully all from within the same thread).
//
//     Disclaimer:      This class is very experimental and there is a decent chance it will not work as expected. 
//
//
//  Programmer:                Forrest Miller
//  Date:                      December 14, 2018

#pragma once

#ifndef CURRENTLY_CONNECTED_MONITORS_H_
#define CURRENTLY_CONNECTED_MONITORS_H_

#include <vector>
#include <memory>

#include "GraphicsLanguageFrameworkMonitor.h"

namespace GLFrameworkInternal {

	using Monitor = GraphicsLanguageFrameworkMonitor;

	inline static constexpr const int MONITOR_NOT_FOUND = -1; //Return value for failure to find a requested monitor

	class CurrentlyConnectedMonitors final {
	public:




	private:
		//fields
		std::vector<std::unique_ptr<Monitor>> mConnectedMonitors_;


		//Helper functions
		//int find(GLFWmonitor*) const;
			
		
	};

} //namespace GLFrameworkInternal

#endif //CURRENTLY_CONNECTED_MONITORS_H_