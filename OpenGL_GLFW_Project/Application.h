// Header file for class that contains the application
//Created by Forrest Miller on July 16, 2018


//RANDOM NOTE: To see vcpkg commands, go to the link: https://docs.microsoft.com/en-us/cpp/vcpkg#command-line-reference

#pragma once

#ifndef APPLICATION_H
#define APPLICATION_H

#include <memory>
#include <fstream>

#include "ProjectConstants.h"
#include "ProjectParameters.h"
#include "GLFW_Init.h"
#include "ProjectResourceFilepaths.h"
#include "HarfBuzzSetupTest.h"

#include "ShaderInterfaceLogger.h" //Not yet in active use

#include "RenderProject1.h"


class Application {
public:
	Application();
	~Application();
	void launch();

private:
	bool mApplicationValid;
	std::shared_ptr<MonitorData> mDisplayInfo;
	std::unique_ptr<GLFW_Init> mGLFWInitializer;


	//Initialization Helper Functions:
	void callProjectSetupFunctions() { //Mostly just checks system/compiler configuration macros
		OperatingSystemAndCompilerDependentConfiguationFunc::Call_Project_Setup_Functions(); //see project setup.h
	} 

	void setupGLFW(); //Loads the GLFW library
	void loadGraphicsLanguageFunctions(); //(Well actually load their pointers) (This uses 'glad' to load Graphics Language)

	void doExtraSetup() const;
	void checkMSAA() const; //Prints MSAA config to MSGLOG
	


	void playIntroMovie();
	

	


	//Projects:
	void runHarfBuzzSetupTest();
	void runRenderProject1();



	//for now:
	//bool loop();

	//eventually:
	//void renderLoop(void *);
	//void logicLoop(void *);

	//void getInput(void * inputFlags);
};



#endif //#define APPLICATION_H