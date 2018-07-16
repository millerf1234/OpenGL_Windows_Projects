// Header file for class that contains the application
//Created by Forrest Miller on July 16, 2018


#pragma once

#ifndef APPLICATION_H
#define APPLICATION_H

#include <memory>

#include "ProjectSetup.h"
#include "ProjectConstants.h"
#include "ProjectParameters.h"
#include "GLFW_Init.h"

class Application
{
public:
	Application();
	~Application();

	void run();
};



#endif //define APPLICATION_H