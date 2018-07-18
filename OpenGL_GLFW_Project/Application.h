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

#include "RenderProject1.h"

class Application
{
public:
	Application();
	~Application();

	void launch();

private:
	bool mApplicationValid;
	std::shared_ptr<MonitorData> mDisplayInfo;
	std::unique_ptr<GLFW_Init> mGLFWInitializer;

	//Initialization Helper Functions:
	void setupGLFW();
	void loadGraphicsLanguageFunctions();

	void checkMSAA(); //Prints MSAA config to MSGLOG

	void playIntroMovie();

	
	void runRenderProject1();



	//for now:
	//bool loop();

	//eventually:
	//void renderLoop(void *);
	//void logicLoop(void *);

	//void getInput(void * inputFlags);
};



#endif //define APPLICATION_H