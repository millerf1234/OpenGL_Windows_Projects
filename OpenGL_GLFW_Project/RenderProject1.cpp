#include "RenderProject1.h"



RenderProject1::RenderProject1(std::shared_ptr<MonitorData> screenInfo) {
	initialize();
	//Make sure we have a monitor to render to
	if (!screenInfo || !screenInfo->activeMonitor) {
		error = true;
		return;
	}
	//Make sure the context is set to this monitor (and this thread [see glfw documentation])
	if (glfwGetCurrentContext() != screenInfo->activeMonitor) {
		std::ostringstream warning;
		warning << "\nWARNING!\n[In RenderProject1's constructor]\n" <<
			"RenderProject1 detected that the glfw active context was set" <<
			"\nto a different monitor or different execution-thread then\n" <<
			"the one passed to this contructor!\n";
		warning << "This means that running RenderProject1 will invalidate\n" <<
			"the previoud context by replacing it with this one, which\n" <<
			"could lead to errors! Please ensure that the correct context is\n" <<
			"being passed to RenderProject1!\n";

		fprintf(WRNLOG, warning.str().c_str());
		glfwMakeContextCurrent(screenInfo->activeMonitor);
	}
	window = screenInfo->activeMonitor;
}



RenderProject1::~RenderProject1() {


}


void RenderProject1::run() {
	if (error) {
		fprintf(MSGLOG, "An error occured while loading RenderProject1");
		return;
	}
	fprintf(MSGLOG, "\nRENDER PROJECT 1 IS LOADED AND RUNNING!\n");


	//fprintf(MSGLOG, "\nLoading TextEngine...\n");
	

	//fprintf(MSGLOG, "\nRender PROJECT 1 will attempt a HarfBuzz test...\n");
	//std::unique_ptr<HarfBuzzTester> hbTest = std::make_unique<HarfBuzzTester>();

	//hbTest->runTest();
	if (txtEngine) {
		fprintf(MSGLOG, "\nLoaded TextEngine! Text Engine ready status: %d\n", txtEngine->ready());
	}
	

	fprintf(MSGLOG, "\nUnloaded TextEngine since no text rendering is yet possible...\n");

	fprintf(MSGLOG, "\nTesting graphics language!\n");

	
	fprintf(MSGLOG, "\nEntering Render Loop...\n");
	renderLoop();




	//std::cin.get(); //keep the screen open


}



void RenderProject1::initialize() {
	error = false;
	window = nullptr;
	frameNumber = 0ull;
	frameUnpaused = 0ull;
	counter = 0.0f;

	txtEngine = nullptr;

}

void RenderProject1::loadAssets() {
	//txtEngine = std::make_unique<TextEngine>("Fonts\\Roboto-Black.ttf");

	fprintf(MSGLOG, "\nCreating a new shader program!\n");
	testProgram = std::make_unique<ShaderProgram>();

	fprintf(MSGLOG, "\nAttaching vertex shader!\n");
	testProgram->attachVert(VERT_PASSTHROUGH2D);
	fprintf(MSGLOG, "\nAttaching fragment shader!\n");
	testProgram->attachFrag(FRAG_CONSTANTCOLOR);

	fprintf(MSGLOG, "\nAttempting to link program!\n");
	testProgram->link();
	if (testProgram->checkIfLinked()) {
		fprintf(MSGLOG, "Program Successfully linked!\n");
	}
	else {
		fprintf(ERRLOG, "Shader Program was not successfully linked!\n");
	}

	testProgram->uniforms->updateUniform1f("zoom", 0.5f);

	//Test Updating/Caching uniform locations too!
}

void RenderProject1::renderLoop() {
	
	while (glfwWindowShouldClose(window) == GLFW_FALSE) {

		if (checkToSeeIfShouldCloseWindow()) {
			glfwSetWindowShouldClose(window, GLFW_TRUE);
			continue; //Skip the rest of this loop iteration to close window quickly
		}
	
		if (checkIfShouldPause()) {
			pause();
			continue;
		}


		//To look into:
		//GL_UseProgram_Stages 
		
		glfwSwapBuffers(window);
		glfwPollEvents();
		frameNumber++; //Increment the frame counter
		prepareGLContextForNextFrame();
	}

}


bool RenderProject1::checkToSeeIfShouldCloseWindow() {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		return true;
	}
	return false;
}

bool RenderProject1::checkIfShouldPause() {
	if ((glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) &&
		(frameNumber >= (frameUnpaused + 10ull))) {
		return true;
	}
	return false;
}

void RenderProject1::pause() {
	auto begin = std::chrono::high_resolution_clock::now(); //Time measurement
	auto end = std::chrono::high_resolution_clock::now();
	fprintf(MSGLOG, "PAUSED!\n"); 
	while (std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count() < 300000000) {
		std::this_thread::sleep_for(std::chrono::nanoseconds(2000000));
		end = std::chrono::high_resolution_clock::now();
	}

	//Enter an infinite loop checking for the unpaused key to be pressed
	while (true) { 
		glfwPollEvents();
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
			frameUnpaused = frameNumber;
			fprintf(MSGLOG, "UNPAUSED!\n");
			return;
		}
		else if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, GLFW_TRUE);
			return;
		}
		else { //wait for a little bit before polling again
			std::this_thread::sleep_for(std::chrono::nanoseconds(333333333));
		}
	}
}


void RenderProject1::prepareGLContextForNextFrame() {
	glBindVertexArray(0);
	glUseProgram(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}