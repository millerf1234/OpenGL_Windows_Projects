//An OpenGL Render Project

//Created by Forrest Miller on July 18, 2018

#pragma once

#ifndef RENDER_PROJECT_1
#define RENDER_PROJECT_1

#include <limits>

#include "ProjectConstants.h"
#include "ProjectParameters.h"
#include "GLFW_Init.h"
#include "ProjectSetup.h"




class RenderProject1 {
public:
	RenderProject1() = delete;
	RenderProject1(std::shared_ptr<MonitorData> screenInfo);
	~RenderProject1();

	void run();
	

private:
	bool error;
	GLFWwindow * window;
	unsigned long long frameNumber, frameUnpaused;
	float counter;



	void initialize();
	void loadAssets(); 


};

#endif //RENDER_PROJECT_1