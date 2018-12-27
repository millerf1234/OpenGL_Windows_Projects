//File:                       Application.h
//Class:                      Application
//
//Description (out-of-date):
//                           The class 'Application' exists at the very top of the program and is responsible
//                            for first performing high-level initialization and then providing an environment for 
//                            individual 'RenderDemos' to operate in. More specifically, this class operates in
//                            two phases. The first phase occurs when the constructor for 'Application' is called. In
//                            this phase, the following tasks are performed (in order):
//                                [Initialization Phase]
//                                  - initialize a window and graphics context through the GLFW 3.x library
//                                  - load the graphics language functions using GLAD 
//                                  - configure graphics context by setting callbacks and global state
//                            
//                            Once these initialization tasks are accomplished, the second phase is commenced by calling
//                            the member funcion launch(). The way launch() operates is to first verify that all 
//                            initialization has occured without issue, followed by presenting the program user with a             *(to be implemented still...)
//                            menu selection* of the various 'RenderDemos' available.     
//                            Once a 'RenderDemo' is selected, the application uses the polymorphic behavior of the 
//                            'RemderDemoBase' class and the member function runRenderDemo() to run the various 'RenderDemos'
//                            available.
//
//Programmer:                 Forrest Miller
//Dates:                      July - November 2018   


//RANDOM NOTE: To see vcpkg commands, go to the link: https://docs.microsoft.com/en-us/cpp/vcpkg#command-line-reference

#pragma once

#ifndef APPLICATION_H_
#define APPLICATION_H_

#include <memory>   //Mostly for smart pointers
#include <fstream>  //Gotta have that fstream

#include "ProjectConstants.h"
#include "ApplicationConstantSettings.h"
#include "GLFW_Init.h"

#include "AssetLoadingDemo.h"
#include "LightsourceTestDemo.h"
#include "TeapotExplosion.h"


//static constexpr const int MONITOR_TO_USE = 0;  //Index of monitor in array of available monitors
//
//static constexpr const bool USE_FULLSCREEN = false;
//
//static constexpr const bool USE_VSYNC = true;
//
//static constexpr int DEFAULT_AA_SAMPLES = 8;



class Application final {
public:
	Application();
	~Application();
	void launch();

	//No Copying or Moving allowed
	Application(const Application&) = delete;
	Application(Application&&) = delete;
	Application& operator=(const Application&) = delete;
	Application& operator=(Application&&) = delete;


private:
	bool mApplicationValid;
	std::shared_ptr<MonitorData> displayInfo;
	std::unique_ptr<GLFW_Init> glfwInitializer;
	

	void initialize();

	bool setupGLFW(); //Loads the GLFW library
	bool loadGraphicsLanguageFunctions(); //(Well actually load their pointers) (This uses 'glad' to load Graphics Language)
	void configureGraphicsContextDebugCallbackFunctions() const; //The callback function is provided by Application and is found in its own header file 
	void setInitialGLState();

	void checkMSAA() const; //Prints MSAA config to MSGLOG
	
	void playIntroMovie();


	
	//Function to run a RenderDemo. The provided unique pointer for the renderDemo should be 
	//newly created, without either of the load() or run() methods having been called. This
	//function is intended to be called by the more specific run_____Demo() functions below.
	void runRenderDemo(std::unique_ptr<RenderDemoBase> & renderDemo, const char * name = "\0");




	//The following functions are for setting up and running different demo programs. These functions rely on
	//the runRenderDemo() function above for polymorphically calling their overriden virtual functions. 
	
	void runAssetLoadingDemo();
	void runLightsourceTestDemo();
	void runTeapotExplosionDemo();

};



#endif //#define APPLICATION_H_