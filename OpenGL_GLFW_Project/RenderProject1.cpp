#include "RenderProject1.h"

using namespace ShaderInterface;


int RenderProject1::numTriangles = 20000;

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
	
	txtEngine.release();
	fprintf(MSGLOG, "\nUnloaded TextEngine since no text rendering is yet possible...\n");

	fprintf(MSGLOG, "\nTesting graphics language!\n");

	
	fprintf(MSGLOG, "\nEntering Render Loop...\n");


	renderLoop();


}

void RenderProject1::loadAssets() {
	//txtEngine = std::make_unique<TextEngine>("Fonts\\Roboto-Black.ttf");

	loadShaders();
	
	createDataBuffer();

	//Update uniform locations
	sceneShader->uniforms->updateUniform1f("zoom", 0.5f);

	//Test Updating/Caching uniform locations too!
}


void RenderProject1::initialize() {
	error = false;
	window = nullptr;
	frameNumber = 0ull;
	frameUnpaused = 0ull;
	frameOfMostRecentColorRecording = 0ull;
	counter = 0.0f;

	txtEngine = nullptr;

	//Set initial background color
	backgroundColor = glm::vec3(0.0f, 0.5f, 0.75f);

	vertices = nullptr;

	createTriangles2D();
	
}

//This function just does some random math to make the triangles
void RenderProject1::createTriangles2D() {  
	//glm::vec3 eulerAngles(0.0f, 0.0f, 0.5f);
	//glm::quat rotation(eulerAngles);
	//rotationMat = glm::quaternion::toMat4(rotation);

	//Rotation
	glm::mat2 rotation(cos(0.01f), sin(0.01f), -sin(0.01f), cos(0.01f));

	//Translation
	glm::vec2 translation(0.01f, 0.02f);

	glm::vec2 vert0(triangleBase2D[0], triangleBase2D[1]);
	glm::vec2 vert1(triangleBase2D[2], triangleBase2D[3]);
	glm::vec2 vert2(triangleBase2D[4], triangleBase2D[5]);

	
	for (int i = 0; i < numTriangles; i++) {
		vert0 = vert0 + (rotation * translation);
		vert1 = vert1 + (rotation * translation);
		vert2 = vert2 + (rotation * (translation + vert2));

		triangles2D.push_back(vert0.x);
		triangles2D.push_back(vert0.y);
		triangles2D.push_back(vert1.x);
		triangles2D.push_back(vert1.y);
		triangles2D.push_back(vert2.x);
		triangles2D.push_back(vert2.y);
	}

	
}

void RenderProject1::loadShaders() {
	fprintf(MSGLOG, "\nCreating a new shader program for scene!\n");
	sceneShader = std::make_unique<ShaderProgram>();

	fprintf(MSGLOG, "\nAttaching vertex shader!\n");
	sceneShader->attachVert(VERT_PASSTHROUGH2D);
	fprintf(MSGLOG, "\nAttaching fragment shader!\n");
	sceneShader->attachFrag(FRAG_CONSTANTCOLOR);

	fprintf(MSGLOG, "\nAttempting to link program!\n");
	sceneShader->link();
	if (sceneShader->checkIfLinked()) {
		fprintf(MSGLOG, "Program Successfully linked!\n");
	}
	else {
		fprintf(ERRLOG, "Shader Program was not successfully linked!\n");
	}
}

void RenderProject1::createDataBuffer() {
	fprintf(MSGLOG, "\nLoading Vertex Data for scene...\n");


	vertices = std::make_unique<SimpleVertexBuffer>(triangles2D.size() * sizeof(GLfloat));
	vertices->create(); //Create self in context
	vertices->attach(triangles2D); //Attach data
	vertices->setLayout(VertLayout::VERT2);
	vertices->loadBufferToGPU(); //Send data to GPU
	vertices->bindToContext(); //Bind buffer to context as the active Vertex input buffer

}



void RenderProject1::updateDataBuffer() {

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

		if (checkIfShouldRecordColor())
			recordColorToLog();

		updateFrameClearColor();

		drawVerts();

		counter += 0.0125f;

		

		glfwSwapBuffers(window);
		glfwPollEvents();
		frameNumber++; //Increment the frame counter
		prepareGLContextForNextFrame();
	}

}


bool RenderProject1::checkToSeeIfShouldCloseWindow() const {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		return true;
	}
	return false;
}

bool RenderProject1::checkIfShouldPause() const {
	if ((frameNumber >= (frameUnpaused + DELAY_LENGTH_OF_PAUSE_CHECKING_AFTER_UNPAUSE))
		&& (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)) {
		return true;
	}
	return false;
}

bool RenderProject1::checkIfShouldRecordColor() const {
	if ((frameNumber >= (frameOfMostRecentColorRecording +
		DELAY_BETWEEN_SCREEN_COLOR_RECORDINGS_IN_RENDER_PROJECT_1))
		&& (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)) {
		return true;
	}
	return false;
}

void RenderProject1::recordColorToLog() {
	frameOfMostRecentColorRecording = frameNumber;
	int colorDigits = 6; //Digits to print out for each color

	//Syntax Note: With '%*f', the '*' means that the width will be provided as an additional parameter
	fprintf(MSGLOG, "\nThe background color of frame %llu is:\n\tRed: %*f,\tGreen: %*f,\tBlue: %*f\n",
		frameNumber, colorDigits, backgroundColor.r, colorDigits, backgroundColor.g, colorDigits, backgroundColor.b);

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

void RenderProject1::updateFrameClearColor() {
	//To look into:
	//GL_UseProgram_Stages 
	float * red = &(backgroundColor.x);
	float * green = &(backgroundColor.y);
	float * blue = &(backgroundColor.z);

	*red = glm::min(1.0f, (*red + *green + *blue) / 3.0f);
	*green = glm::abs(sin(glm::min(1.0f, (*red) + ((*blue) * (*blue) / (*red)))));
	*blue = 0.5f + 0.25f*cos(counter);

	//static bool sleep = false;
	if (abs(ceilf(counter) - counter) < 0.0049f) {
		float temp = *red;
		*red = *green;
		*green = *blue;
		*blue = temp;
		//sleep = true;
	}

	//red += red * glm::sin(static_cast<float>(counter++));
	//green = cos(static_cast<float>(counter++));
	//blue = 0.5*sin(red + (static_cast<float>(counter++)));

	glClearColor(*red, *green, *blue, 1.0f);
}

void RenderProject1::drawVerts() {
	sceneShader->use();
	vertices->bindToContext();

	glDrawArrays(GL_TRIANGLES, 0, numTriangles);
}


void RenderProject1::prepareGLContextForNextFrame() {
	glBindVertexArray(0);
	glUseProgram(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}