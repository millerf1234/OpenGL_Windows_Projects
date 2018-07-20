//An OpenGL Render Project

//Created by Forrest Miller on July 18, 2018

#pragma once

#ifndef HARFBUZZ_SETUP_TEST
#define HARFBUZZ_SETUP_TEST

#include <limits>

#include "ProjectConstants.h"
#include "ProjectParameters.h"
#include "GLFW_Init.h"
#include "ProjectSetup.h"

#include "HarfBuzzTester.h"



class HarfBuzzSetupTest {
public:
	HarfBuzzSetupTest() = delete;
	HarfBuzzSetupTest(std::shared_ptr<MonitorData> screenInfo);
	~HarfBuzzSetupTest();

	void run();


private:
	bool error;
	GLFWwindow * window;
	//unsigned long long frameNumber, frameUnpaused;
	//float counter;



	void initialize();
	//void loadAssets();


};

#endif //#ifndef HARFBUZZ_SETUP_TEST