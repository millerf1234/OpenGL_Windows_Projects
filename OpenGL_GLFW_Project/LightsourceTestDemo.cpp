//Demo of using shader noise and blending, trying to achieve a halo around
//a light source effect. See Header for more detail.

#include "LightsourceTestDemo.h"

void LightsourceTestDemo::initialize() {
	error = false;
	frameNumber = 0ull;
	frameUnpaused = 0ull;
	frameDitherLastToggled = 0ull;
	frameBlendLastToggled = 0ull;
	framePolygonSmoothLastToggled = 0ull;
	frameColorshiftLastToggled = 0ull;
	counter = 0.0f;
	vao = vbo = ebo = 0u;
	eboSize = 0u;

	colorShift = 1;

	zoom = 1.0f;
	zRotation = 0.0f;
	
	useDither = false;
	useBlend = true;
	usePolygonSmooth = false;
	

	noiseFunctionToUse = 0;
	noiseResolution = 5;


	//Set initial background color
	backgroundColor = glm::vec3(0.25f, 0.5f, 0.75f);
	
}

LightsourceTestDemo::LightsourceTestDemo(std::shared_ptr<MonitorData> screenInfo) : RenderDemoBase() {
	initialize();
	//Make sure we have a monitor to render to
	if (!screenInfo || !screenInfo->activeMonitor) {
		error = true;
		return;
	}
	//Make sure the context is set to this monitor (and this thread [see glfw documentation])
	if (glfwGetCurrentContext() != screenInfo->activeMonitor) {
		std::ostringstream warning;
		warning << "\nWARNING!\n" <<
			"LightsourceTestDemo detected that the GLFW active context was set" <<
			"\nto a different monitor or different execution-thread then\n" <<
			"the one passed to LightsourceTestDemo's contructor!\n";
		warning << "This means that running LightsourceTestDemo will invalidate\n" <<
			"the previous context by replacing it with this one, which\n" <<
			"could (probably) lead to errors! Please ensure that the correct context\n" <<
			"is being passed to LightsourceTestDemo in the application code!\n";

		fprintf(WRNLOG, warning.str().c_str());
		glfwMakeContextCurrent(screenInfo->activeMonitor);
	}
	window = screenInfo->activeMonitor;
}

void LightsourceTestDemo::cleanup() {
	//Delete Shader Programs
	if (!lightSourceShader->released()) {
		lightSourceShader->release();
	}
	//Delete buffers
	if (vbo != 0u) {
		glDeleteBuffers(1, &vbo);
	}
	if (ebo != 0u) {
		glDeleteBuffers(1, &ebo);
	}
	//Delete Vertex Array Objects
	if (vao != 0u) {
		glDeleteVertexArrays(1, &vao);
	}
}

LightsourceTestDemo::~LightsourceTestDemo() {

}


void LightsourceTestDemo::run() {
	if (error) {
		fprintf(ERRLOG, "An error occured while loading LightsourceTestDemo\n");
		return;
	}
	fprintf(MSGLOG, "\nLightsourceTestDemo project has loaded and will begin running!\n");


	fprintf(MSGLOG, "\n\tDemo Starting...!\n");


	fprintf(MSGLOG, "\nEntering Render Loop...\n");


	renderLoop();

	cleanup();
}

void LightsourceTestDemo::loadAssets() {
	loadShaders(); //load the GLSL shader code
	loadModels();  //load model data to buffers
	configureContext();
}


void LightsourceTestDemo::loadShaders() {
	fprintf(MSGLOG, "\nInitializing Shaders!\n");

	lightSourceShader = std::make_unique<ShaderProgram>();

	lightSourceShader->attachVert("LightsourceTestDemo.vert");


	std::unique_ptr<ShaderInterface::FragmentShader> noiseShader = std::make_unique<ShaderInterface::FragmentShader>("ShaderNoiseFunctions.frag");
	noiseShader->makeSecondary();

	lightSourceShader->attachSecondaryFrag(noiseShader.get());

	lightSourceShader->attachFrag("LightsourceTestDemo.frag");

	lightSourceShader->link();
	if (lightSourceShader->checkIfLinked()) {
		fprintf(MSGLOG, "Program Successfully linked!\n");
	}
	else {
		fprintf(ERRLOG, "Shader Program was not successfully linked!\n");
		fprintf(MSGLOG, "\t[Press 'ENTER' to attempt to continue program execution]\n");
		std::cin.get(); //Hold the window open if there was an error
	}
}

//This function is intended to be called only once during the setup phase. There is a 
//seperate series of functions for toggling the pipeline state once the render loop is 
//running.
void LightsourceTestDemo::configureContext() {
	if (useDither) {
		glEnable(GL_DITHER);
	}
	if (useBlend) {
		glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	}
	if (usePolygonSmooth) {
		glEnable(GL_POLYGON_SMOOTH);
	}
}

void LightsourceTestDemo::loadModels() {
	
	std::array<float, 3> pos = { 0.0f, 0.00f, 0.0f };
	std::array<float, 3> col = { 0.9955f, 0.9975f, 1.0f };

	size_t lightPolygonSides = 13u;
	size_t lightPolygonLayers = 96u;
	float lightPolygonBaseRadius = 0.0015f; //0.65f
	float lightPolygonLayerGrowth = 1.09f; //1.05f

	
	testLightEmitter = std::make_unique<LightEmitterSource>(pos,
															col,
															lightPolygonSides,
															lightPolygonLayers,
															lightPolygonBaseRadius,
															lightPolygonLayerGrowth);


	std::vector<GLfloat> sceneObjects = testLightEmitter->getData();
	std::vector<GLuint> elementArray = testLightEmitter->getElemOrder();
	eboSize = elementArray.size();
	glCreateBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, elementArray.size() * sizeof(GLuint), &elementArray.data()[0], GL_STATIC_DRAW);


	glCreateVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glCreateBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	fprintf(MSGLOG, "\nReality Check: sizeof(sceneObjects.data() is: %u\n", sizeof(sceneObjects.data()));
	fprintf(MSGLOG, "ANd sceneObjects.size() * sizeof(sceneObjects.data()) is: %u\n", sceneObjects.size() * sizeof(sceneObjects.data()));

	glBufferData(GL_ARRAY_BUFFER, sceneObjects.size() * sizeof(sceneObjects.data()), sceneObjects.data(), GL_STATIC_DRAW);

	glEnableVertexArrayAttrib(vao, 0); //Requires OpenGl 4.5 or newer, allows VAO to be specified as param
	//								   //glEnableVertexAttribArray(0);  //Since OpenGl 2.0, will use whatever VAO is bound globally to context
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, (6u) * sizeof(GLfloat), (GLvoid*)0u);


	glEnableVertexArrayAttrib(vao, 1);
	//glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, (6u) * sizeof(GLfloat), (GLvoid*)(3u * sizeof(GLfloat)));


}

void LightsourceTestDemo::renderLoop() {
	while (glfwWindowShouldClose(window) == GLFW_FALSE) {
		if (checkToSeeIfShouldCloseWindow()) {
			glfwSetWindowShouldClose(window, GLFW_TRUE);
			continue; //Skip the rest of this loop iteration to close window quickly
		}

		if (checkIfShouldPause()) {
			pause();
			continue;
		}

		if (checkIfShouldReset()) {
			//It is necessary to check for pausing again here to get proper
			//behavior if both pause and reset are pressed at same time (because
			//the delay on pause detection is based off frame counter). Note that
			//the function 'checkIfShouldPause()' is intentionally not used here.
			if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
				pause();
				continue;
			}
			reset();
		}

		
		
		changeNoiseType();
		togglePipelineEffects();
		toggleColorshift();

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


bool LightsourceTestDemo::checkToSeeIfShouldCloseWindow() const {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		return true;
	}
	return false;
}

bool LightsourceTestDemo::checkIfShouldPause() const {
	if ((frameNumber >= (frameUnpaused + DELAY_LENGTH_OF_PAUSE_CHECKING_AFTER_UNPAUSE))
		&& (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)) {
		return true;
	}
	return false;
}


bool LightsourceTestDemo::checkIfShouldReset() const {
	if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS)
		return true;
	return false;

}

void LightsourceTestDemo::pause() {
	auto begin = std::chrono::high_resolution_clock::now(); //Time measurement
	auto end = std::chrono::high_resolution_clock::now();
	fprintf(MSGLOG, "PAUSED!\n");
	while (std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count() < 300000000) {
		std::this_thread::sleep_for(std::chrono::nanoseconds(2000000));
		end = std::chrono::high_resolution_clock::now();
	}

	//Enter an infinite loop checking for the unpause key (or exit key) to be pressed
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
			std::this_thread::sleep_for(std::chrono::nanoseconds(3333333));
		}
	}
}

void LightsourceTestDemo::reset() {
	fprintf(MSGLOG, "\nReseting Demo...\n");
	counter = 0.0f; //Reset time to 0
	frameNumber = 0ull;
	frameUnpaused = 0ull;
	frameDitherLastToggled = 0ull;
	frameBlendLastToggled = 0ull;
	framePolygonSmoothLastToggled = 0ull;
	frameColorshiftLastToggled = 0ull;
	zoom = 1.0f;
}

void LightsourceTestDemo::changeNoiseType() {
	

	/*if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		noiseResolution++;
	}
	else if ((glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)) {
		noiseResolution--;
		if (noiseResolution == 0)
			noiseResolution = 1;
	}*/

	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		noiseFunctionToUse = 0;
	}
	else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		noiseFunctionToUse = 1;
	}
	else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		noiseFunctionToUse = 2;
	}
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
		noiseFunctionToUse = 3;
	}
	else if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
		noiseFunctionToUse = 4;
	}
	else if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {
		noiseFunctionToUse = 5;
	}
	if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {
		noiseFunctionToUse = 6;
	}
	else if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
		noiseFunctionToUse = 7;
	}
	else if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
		noiseFunctionToUse = 8;
	}

}

void LightsourceTestDemo::togglePipelineEffects() {
	toggleDither();
	toggleBlend();
	togglePolygonSmooth();	
}

void LightsourceTestDemo::toggleDither() {
	if ((frameDitherLastToggled + 20ull) >= frameNumber) {
		return;
	}
	else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		frameDitherLastToggled = frameNumber;
		useDither = !useDither;
		if (useDither) {
			glEnable(GL_DITHER);
			fprintf(MSGLOG, "Dither Enabled!\n");
		}
		else {
			glDisable(GL_DITHER);
			fprintf(MSGLOG, "Dither disabled!\n");
		}
	}
}

void LightsourceTestDemo::toggleBlend() {
	if ((frameBlendLastToggled + 20ull) >= frameNumber) {
		return;
	}
	else if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
		frameBlendLastToggled = frameNumber;
		useBlend = !useBlend;
		if (useBlend) {
			glEnable(GL_BLEND);
			//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
			fprintf(MSGLOG, "Blending Enabled!\n");
		}
		else {
			glDisable(GL_BLEND);
			fprintf(MSGLOG, "Blending Disabled!\n");
		}
	}
}

void LightsourceTestDemo::togglePolygonSmooth() {
	if ((framePolygonSmoothLastToggled + 20ull) >= frameNumber) {
		return;
	}
	else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		framePolygonSmoothLastToggled = frameNumber;
		usePolygonSmooth = !usePolygonSmooth;
		if (usePolygonSmooth) {
			glEnable(GL_POLYGON_SMOOTH);
			fprintf(MSGLOG, "Polygon Smoothing Enabled!\n");
		}
		else {
			glDisable(GL_POLYGON_SMOOTH);
			fprintf(MSGLOG, "Polygon Smoothing Disabled!\n");
		}
	}
}

void LightsourceTestDemo::toggleColorshift() { 
	//Frag shader has been updated so now all effects have a colorshift
	//if ((noiseFunctionToUse != 5) && (noiseFunctionToUse != 6) && (noiseFunctionToUse != 7) ) {
	//	return;
	//}
	if ((frameColorshiftLastToggled + 12ull) >= frameNumber) {
		return;
	}
	else if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
		frameColorshiftLastToggled = frameNumber;
		colorShift *= -1;
		if (colorShift < 0) {
			fprintf(MSGLOG, "Colorshift Set to Negative State\n");
		}
		else {
			fprintf(MSGLOG, "Colorshift Set to Positive State\n");
		}
	}
}

void LightsourceTestDemo::updateFrameClearColor() {

	glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, 1.0);

}

void LightsourceTestDemo::updateUniforms() {

	lightSourceShader->use();

	
	//lightSourceShader->uniforms->updateUniform1f("distanceToCamera", 0.5f + 0.45f*sin(counter));
	

	//glm::mat4 proj = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));


	//lightSourceShader->uniforms->updateUniformMat4x4("projection", &proj);

										
	lightSourceShader->uniforms->updateUniform1f("time", 2.0f*counter);
	lightSourceShader->uniforms->updateUniform1f("zoom", zoom);

	lightSourceShader->uniforms->updateUniform1i("noiseFunctionToUse", noiseFunctionToUse);
	lightSourceShader->uniforms->updateUniform1i("noiseResolution", noiseResolution);
	
	lightSourceShader->uniforms->updateUniform1i("colorShift", colorShift);
}

void LightsourceTestDemo::drawVerts() {

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	lightSourceShader->use();

	glDrawElements(GL_TRIANGLES, eboSize, GL_UNSIGNED_INT, 0);
	return;
}


void LightsourceTestDemo::prepareGLContextForNextFrame() {
	glBindVertexArray(0);
	glUseProgram(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
