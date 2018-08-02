//An OpenGL Render Project

//Created by Forrest Miller on July 18, 2018

#pragma once

#ifndef RENDER_PROJECT_1_H_
#define RENDER_PROJECT_1_H_

#include <limits>
#include <thread>
#include <chrono>

#include "ProjectConstants.h"
#include "ProjectParameters.h"
#include "GLFW_Init.h"
#include "ProjectSetup.h"

#include "ProjectResourceFilepaths.h"

#include "TextEngine.h"

#include "ShaderProgram.h"
//#include "ShaderProgramBuilder.h"

class RenderProject1 {
public:
	RenderProject1() = delete;
	RenderProject1(std::shared_ptr<MonitorData> screenInfo);
	~RenderProject1();

	void run();
	void loadAssets();
	

private:
	bool error;
	GLFWwindow * window;
	unsigned long long frameNumber, frameUnpaused;
	float counter;


	std::unique_ptr<ShaderProgram> testProgram; //Test shader program
	std::unique_ptr<TextEngine> txtEngine;


	void initialize();
	
	void renderLoop();

	bool checkToSeeIfShouldCloseWindow(); 
	bool checkIfShouldPause();
	void pause();

	void prepareGLContextForNextFrame();
};

#endif //RENDER_PROJECT_1_H_