//  File:          ConnectedMonitors.h
//
//  Description:   Defines a class intended to manage and hold all of the 
//                 currently connected monitors. Intended to be used as 
//                 part of the GraphicsLanguageFramework.
//
//  Programmer:    Forrest Miller
//  Date:          January 2, 2019

#pragma once

#ifndef CONNECTED_MONITORS_H_
#define CONNECTED_MONITORS_H_

#include "GraphicsLanguageFrameworkMonitor.h"

class ConnectedMonitors {
public:
	ConnectedMonitors();
	~ConnectedMonitors();

	//Call as part of 'aquireGLFrameworkComponents'
	void aquireAllAvailableMonitors();
	void aquirePrimaryMonitor();
};


#endif //CONNECTED_MONITORS_H_

