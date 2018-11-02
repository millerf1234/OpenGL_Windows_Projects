// Header file for class that contains the application
//Created by Forrest Miller on July 16, 2018
//This file underwent continuous modifications up through November 2, 2018. 
//On November 2, 2018, the programmer (me) decided to clean up both this file
//and the '.cpp' implementation file to remove old/broken code. I'm on a bit 
//of a limited time frame to do so, so I probably won't finish today. 


//RANDOM NOTE: To see vcpkg commands, go to the link: https://docs.microsoft.com/en-us/cpp/vcpkg#command-line-reference

#pragma once

#ifndef APPLICATION_H
#define APPLICATION_H

#include <memory>   //Mostly for smart pointers
#include <fstream>  //Gotta have that fstream

#include "ProjectConstants.h"
#include "ProjectParameters.h"
#include "GLFW_Init.h"

//#include "RenderProject1.h"
//#include "GeometryShaderExplosion.h"
#include "AssetLoadingDemo.h"

#include "RenderDemoBase.h"
#include "TeapotExplosion.h"


class Application {
public:
	Application();
	~Application();
	void launch();

private:
	bool mApplicationValid;
	std::shared_ptr<MonitorData> displayInfo;
	std::unique_ptr<GLFW_Init> glfwInitializer;


	//Initialization Helper Functions (this was just me playing around and can be removed in future):
	void callProjectSetupFunctions() { //Mostly just checks system/compiler configuration macros
		OperatingSystemAndCompilerDependentConfiguationFunc::Call_Project_Setup_Functions(); //see project setup.h
	} 

	void setupGLFW(); //Loads the GLFW library
	void loadGraphicsLanguageFunctions(); //(Well actually load their pointers) (This uses 'glad' to load Graphics Language)
	void checkIfGraphicsContextDebugCallbackFunctionEnabled() const; //The callback function is provided by Application and is found in its own header file 
	
	void doExtraSetup() const;
	void checkMSAA() const; //Prints MSAA config to MSGLOG
	
	void playIntroMovie();
	

	//Function to run a RenderDemo. The provided unique pointer for the renderDemo should be 
	//newly created, without either of the load() or run() methods having been called.
	void runRenderDemo(std::unique_ptr<RenderDemoBase> & renderDemo, const char * name = "\0");

	void runAssetLoadingDemo();
};



#endif //#define APPLICATION_H