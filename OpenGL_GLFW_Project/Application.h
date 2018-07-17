// Header file for class that contains the application
//Created by Forrest Miller on July 16, 2018


#pragma once

#ifndef APPLICATION_H
#define APPLICATION_H

#include <memory>
#include <fstream>

#include "ProjectSetup.h"
#include "ProjectConstants.h"
#include "ProjectParameters.h"
#include "GLFW_Init.h"

class Application
{
public:
	Application();
	~Application();

	void launch();

private:
	bool mApplicationValid;
	std::shared_ptr<MonitorData> mDisplayInfo;

	void setupGLFW();

	//for now:
	bool loop();


	//eventually:
	//void renderLoop(void *);
	//void logicLoop(void *);

	//void getInput(void * inputFlags);
};



#endif //define APPLICATION_H