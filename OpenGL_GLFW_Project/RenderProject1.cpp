#include "RenderProject1.h"

using namespace ShaderInterface;


void RenderProject1::initialize() {
	error = false;
	window = nullptr;
	frameNumber = 0ull;
	frameUnpaused = 0ull;
	frameOfMostRecentColorRecording = 0ull;
	counter = 0.0f;
	zRotation = 0.0f;

	//Set initial background color
	backgroundColor = glm::vec3(0.0f, 0.5f, 0.75f);


}



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
		fprintf(ERRLOG, "An error occured while loading RenderProject1\n");
		return;
	}
	fprintf(MSGLOG, "\nRENDER PROJECT 1 IS LOADED AND RUNNING!\n");


	
	

	fprintf(MSGLOG, "\nUnloaded TextEngine since no text rendering is yet possible...\n");

	fprintf(MSGLOG, "\nTesting graphics language!\n");

	
	fprintf(MSGLOG, "\nEntering Render Loop...\n");


	renderLoop();


}

void RenderProject1::loadAssets() {
	//txtEngine = std::make_unique<TextEngine>("Fonts\\Roboto-Black.ttf");

	loadShaders();
	createTriangles2D();
	
	//createDataBuffer();
	//Test Updating/Caching uniform locations too!
}


void RenderProject1::loadShaders() {
	fprintf(MSGLOG, "\nInitializing Shaders!\n");
	sceneShader = std::make_unique<ShaderProgram>();

	fprintf(MSGLOG, "\nAttaching secondary helper vertex shader!\n");
	std::unique_ptr<ShaderInterface::VertexShader> vertHelper = std::make_unique<ShaderInterface::VertexShader>("VertMath.vert");
	if (!vertHelper)
		return;
	vertHelper->makeSecondary();

	sceneShader->attachSecondaryVert(vertHelper.get());

	fprintf(MSGLOG, "\nAttaching main vertex shader!\n");
	sceneShader->attachVert("RenderProject1.vert");
	fprintf(MSGLOG, "\nAttaching fragment shader!\n");
	sceneShader->attachFrag("RenderProject1.frag");

	fprintf(MSGLOG, "\nAttaching a geometry shader!\n");
	sceneShader->attachGeom("gst.geom");

	fprintf(MSGLOG, "\nAttempting to link program!\n");
	sceneShader->link();
	if (sceneShader->checkIfLinked()) {
		fprintf(MSGLOG, "Program Successfully linked!\n");
	}
	else {
		fprintf(ERRLOG, "Shader Program was not successfully linked!\n");
	}

	
	/*fprintf(MSGLOG, "\nCreating a new shader program for scene!\n");
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
	}*/
}

//void RenderProject1::createDataBuffer() {
//	fprintf(MSGLOG, "\nLoading Vertex Data for scene...\n");
//
//
//	vertices = std::make_unique<SimpleVertexBuffer>(triangles2D.size() * sizeof(GLfloat));
//	vertices->create(); //Create self in context
//	vertices->attach(triangles2D); //Attach data
//	vertices->setLayout(VertLayout::VERT2);
//	vertices->loadBufferToGPU(); //Send data to GPU
//	vertices->bindToContext(); //Bind buffer to context as the active Vertex input buffer
//
//}
//
//
//
//void RenderProject1::updateDataBuffer() {
//
//}


void RenderProject1::createTriangles2D() {
	//std::vector<GLfloat> positions = { 0.75f, -0.75f, 0.0f, -0.75f, -0.75f, 0.0f, 0.75f, 0.75f, 0.000f };
	//std::vector<GLfloat> colors = { 0.15f, 0.25f, 0.35f, 0.45f, 0.55f, 0.65f, 0.75f, 0.95f, 0.75f };

	std::vector<GLfloat> positions;
	std::vector<GLfloat> colors;
	float xStart = -0.5f;
	float yStart = -0.5f;
	float z = 0.0f;

	float increment = 0.0f;

	for (int i = 0; i < NUMBER_TRIANGLES; i++) {
		positions.push_back(xStart + (float)((float)(i*(0.75f*(float)i)) / (float)(NUMBER_TRIANGLES+NUMBER_TRIANGLES)));
		positions.push_back(yStart + (float)((float)(i+i) / (float)NUMBER_TRIANGLES));
		positions.push_back(z);


		increment += 0.05f;
		if (increment > 0.6f) {
			increment = -0.05f;
			//increment = 0.01f;
		}
		
		for (int j = 0; j < 3; j++) {
			colors.push_back(0.59f + (float)j / increment);
		} 
		//colors.push_back(MathFunc::getRandomInRangef(0.001f + increment, 0.33f+increment));
		//colors.push_back(MathFunc::getRandomInRangef(0.0f, increment));
		//colors.push_back(MathFunc::getRandomInRangef(0.667f, 0.88f));
	}

	float colorZeroReplacer = 0.0f;
	std::vector<GLfloat>::iterator colorIterator = colors.begin();
	for (; colorIterator != colors.end(); colorIterator++) {
		if (*colorIterator <= 0.0f) {
			colorZeroReplacer += 0.05f;
			if (colorZeroReplacer > 1.00f) {
				colorZeroReplacer = 0.15f;
			}
		
			*colorIterator = (colorZeroReplacer);
		}
	}

	constexpr int NUM_VERTEX_ARRAYS = 2;

	vertexAttributes = std::make_unique<GenericVertexAttributeSet>(NUM_VERTEX_ARRAYS);
	
	if (!vertexAttributes)
		return;
	vertexAttributes->sendDataToVertexBuffer(0, positions, 3, 0);
	vertexAttributes->sendDataToVertexBuffer(1, colors, 3, 0);
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
		
		updateUniforms();


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
	//*green = glm::abs(sin(glm::min(1.0f, (*red) + ((*blue) * (*blue) / (*red)))));
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

void RenderProject1::updateUniforms() {
	sceneShader->use();
	//Update uniform locations
	sceneShader->uniforms->updateUniform1f("zoom", 1.5f);
	sceneShader->uniforms->updateUniform1f("time", counter);

	
	zRotation += 0.02f;
	//fprintf(MSGLOG, "zRot is %f\n", zRotation);
	sceneShader->uniforms->updateUniform1f("xRotation", 0.0f);
	sceneShader->uniforms->updateUniform1f("yRotation", 0.0f*zRotation * 4.0f);
	sceneShader->uniforms->updateUniform1f("zRotation", zRotation);
}

void RenderProject1::drawVerts() {
	if (sceneShader)
		sceneShader->use();
	if (vertexAttributes)
		vertexAttributes->use();
	
	/*glDrawArrays(GL_TRIANGLES, 0, 3);*/
	glDrawArrays(GL_POINTS, 0, NUMBER_TRIANGLES);
}


void RenderProject1::prepareGLContextForNextFrame() {
	glBindVertexArray(0);
	glUseProgram(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}