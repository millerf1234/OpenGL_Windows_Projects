//See header file for details
//This file will most likely closely resemble the file "RenderProject1.cpp"

//  INPUT:  -Press the arrow keys to do rotations on the colors
//          -Press '1', '2', and '3' to toggle Pipeline Input Type 
//          -Press 'Minus' and 'Equal' keys to change the velocity factor
//          -Press 'A' and 'S' to toggle strength of an alternate color conidition ('A' is turn on, 'S' is turn off)
//          -Press 'R' to reset 
//          -Press 'P' to print to the console the RGB values for the background of the current frame
//          -Presss 'ESC' to exit
//          -Press 'SPACE' to pause 
//          -Press 'TAB' to reset 
//        

#include "TeapotExplosion.h"

void TeapotExplosion::initialize() {
	error = false;
	frameNumber = 0ull;
	frameUnpaused = 0ull;
	frameOfMostRecentColorRecording = 0ull;
	counter = 0.0f;
	xRotation = 90.0f;
	yRotation = 120.0f;
	zRotation = 0.0f;
	redRotationTheta = 0.0f;
	greenRotationTheta = 0.0f;
	blueRotationTheta = 0.0f;
	velocity = 1.0f;

	//Set initial background color
	backgroundColor = glm::vec3(0.25f, 0.5f, 0.75f);

	colorModificationValues[0] = 1.0f; //The first value is a special value
	for (int i = 1; i < COLOR_MOD_VALUES_COUNT; i++) {
		colorModificationValues[i] = 0.0f;
	}

	//Set the starting input primitive type
	currentTriangleInputType = PIPELINE_PRIMATIVE_INPUT_TYPE::DISCRETE_TRIANGLES;

	colorChangeThreshold = 0.12f;

	glEnable(GL_PROGRAM_POINT_SIZE);
}



TeapotExplosion::TeapotExplosion(std::shared_ptr<MonitorData> screenInfo) : RenderDemoBase() {
	initialize();
	//Make sure we have a monitor to render to
	if (!screenInfo || !screenInfo->activeMonitor) {
		error = true;
		return;
	}
	//Make sure the context is set to this monitor (and this thread [see glfw documentation])
	if (glfwGetCurrentContext() != screenInfo->activeMonitor) {
		std::ostringstream warning;
		warning << "\nWARNING!\n[In TeapotExplosions's constructor]\n" <<
			"TeapotExplosion detected that the GLFW active context was set" <<
			"\nto a different monitor or different execution-thread then\n" <<
			"the one passed to TeapotExplosion's contructor!\n";
		warning << "This means that running TeapotExplosion will invalidate\n" <<
			"the previous context by replacing it with this one, which\n" <<
			"could (probably) lead to errors! Please ensure that the correct context\n" <<
			"is being passed to TeapotExplosion in the application code!\n";

		fprintf(WRNLOG, warning.str().c_str());
		glfwMakeContextCurrent(screenInfo->activeMonitor);
	}
	window = screenInfo->activeMonitor;
}



TeapotExplosion::~TeapotExplosion() {

}


void TeapotExplosion::run() {
	if (error) {
		fprintf(ERRLOG, "An error occured while loading TeapotExplosion\n");
		return;
	}
	fprintf(MSGLOG, "\nTeapotExplosion demo project has loaded and will begin running!\n");


	fprintf(MSGLOG, "\n\tDemo Starting...!\n");


	fprintf(MSGLOG, "\nEntering Render Loop...\n");


	renderLoop();

}

void TeapotExplosion::loadAssets() {
	loadShaders(); //load the GLSL shader code
	loadTeapot();  //have the GL context load the Teapot vertices to video memory 
}


void TeapotExplosion::loadShaders() {
	
	fprintf(MSGLOG, "\nInitializing Shaders!\n");
	std::string shadersRFP = FILEPATH_TO_SHADERS;  //shadersRelativeFilePath

	sceneShader = std::make_unique<ShaderProgram>();

	//There is just 1 pipeline here to build. Here is the pipeline
	fprintf(MSGLOG, "Assembling the main shader for the scene...\n");


	//---------------------------
	//    (VERTEX STAGE)
	// Attach a helper shader containing some useful functions
	fprintf(MSGLOG, "\nAttaching secondary helper vertex shader!\n");
	std::unique_ptr<ShaderInterface::VertexShader> vertHelper = std::make_unique<ShaderInterface::VertexShader>(shadersRFP + std::string("VertMath.vert"));
	if (!vertHelper)
		return;
	vertHelper->makeSecondary();
	sceneShader->attachSecondaryVert(vertHelper.get());
	//Attach the primary vertex shader
	fprintf(MSGLOG, "\nAttaching main vertex shader!\n");
	std::unique_ptr<ShaderInterface::VertexShader> mainVert = std::make_unique<ShaderInterface::VertexShader>(shadersRFP + std::string("TeapotExplosion.vert"));
	if (!mainVert)
		return;
	sceneShader->attachVert(mainVert.get());
	//---------------------------

	//---------------------------
	//     (Geometry Stage) 
	// Attach the primary geometry shader to the pipeline. (This is where the explosion happens)
	fprintf(MSGLOG, "\nAttaching geometry shader!\n");
	sceneShader->attachGeom(shadersRFP + std::string("TeapotExplosion.geom"));
	//--------------------------


	//--------------------------
	//     (Fragment Stage)
	// Attach the primary Fragment Shader to the pipeline
	fprintf(MSGLOG, "\nAttaching fragment shader!\n");
	sceneShader->attachFrag(shadersRFP + std::string("TeapotExplosion.frag"));
	fprintf(MSGLOG, "\nAttempting to link program!\n");
	//---------------------------

	//    AT THIS POINT, ALL OF THE PIPELINE STAGES HAVE BEEN ATTACHED, SO LINK THE PIPELINE AND CHECK FOR ERRORS
	sceneShader->link();
	if (sceneShader->checkIfLinked()) {
		fprintf(MSGLOG, "Program Successfully linked!\n");
	}
	else {
		fprintf(ERRLOG, "Shader Program was not successfully linked!\n");
		fprintf(MSGLOG, "\t[Press 'ENTER' to attempt to continue program execution]\n");
		std::cin.get(); //Hold the window open if there was an error
	}
}




void TeapotExplosion::loadTeapot() {

	fprintf(MSGLOG, "\nLoading Teapot Vertices\n");
	std::vector<GLfloat> teapotVertices;
	for (int i = 0; i < teapot_count; i++) {
		teapotVertices.push_back(static_cast<GLfloat>(teapot[i]));
	}
	fprintf(MSGLOG, "\nTeapot vertices loaded to application.\n"
		"Application will now load teapot vertices to video memory on GPU.\n");


	//Make a vertex attribute set to handle organizing the data for the graphics context
	vertexAttributes = std::make_unique<TeapotExplosionDemo_GenericVertexAttributeSet>(1);

	if (!vertexAttributes)
		return;
	vertexAttributes->sendDataToVertexBuffer(0, teapotVertices, 3, 0);

	fprintf(MSGLOG, "\nTeapot Has Been Successfully Loaded To Video Memory!\n");
}

void TeapotExplosion::renderLoop() {
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

		if (checkIfShouldReset()) {
			if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
				pause();
				continue;
			}
			reset();
		}

		changePrimitiveType();
		modifyColorThreshhold();
		rotateColor();
		updateColorModificationValues();
		updateVelocity();

		updateFrameClearColor();

		updateUniforms();


		drawVerts();

		counter += 0.000125f;
		//counter += 0.00000002f*0.000125f;

		glfwSwapBuffers(window);
		
		glfwPollEvents();
		frameNumber++; //Increment the frame counter
		prepareGLContextForNextFrame();
	}
}


bool TeapotExplosion::checkToSeeIfShouldCloseWindow() const {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		return true;
	}
	return false;
}

bool TeapotExplosion::checkIfShouldPause() const {
	if ((frameNumber >= (frameUnpaused + DELAY_LENGTH_OF_PAUSE_CHECKING_AFTER_UNPAUSE))
		&& (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)) {
		return true;
	}
	return false;
}

bool TeapotExplosion::checkIfShouldRecordColor() const {
	if ((frameNumber >= (frameOfMostRecentColorRecording +
		DELAY_BETWEEN_SCREEN_COLOR_RECORDINGS_IN_RENDER_PROJECTS))
		&& (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)) {
		return true;
	}
	return false;
}

bool TeapotExplosion::checkIfShouldReset() const {
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
		return true;
	return false;

}

void TeapotExplosion::pause() {
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

void TeapotExplosion::recordColorToLog() {
	frameOfMostRecentColorRecording = frameNumber;
	int colorDigits = 6; //Digits to print out for each color

						 //Syntax Note: Within '%*f', the '*' means that the width will be provided as an additional parameter
	fprintf(MSGLOG, "\nThe background color of frame %llu is:\n\tRed: %*f,\tGreen: %*f,\tBlue: %*f\n",
		frameNumber, colorDigits, backgroundColor.r, colorDigits, backgroundColor.g, colorDigits, backgroundColor.b);
}

void TeapotExplosion::reset() {
	fprintf(MSGLOG, "\nReseting Demo...\n");
	counter = 0.0f; //Reset time to 0
	zRotation = 0.0f; //Reset rotation
	backgroundColor = glm::vec3(0.0f, 0.5f, 0.75f);
	frameNumber = 0ull;
}

void TeapotExplosion::changePrimitiveType() {

	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) 
		currentTriangleInputType = PIPELINE_PRIMATIVE_INPUT_TYPE::DISCRETE_TRIANGLES;

	else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) 
		currentTriangleInputType = PIPELINE_PRIMATIVE_INPUT_TYPE::TRIANGLE_STRIP;

	else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) 
		currentTriangleInputType = PIPELINE_PRIMATIVE_INPUT_TYPE::TRIANGLE_FAN;
}

void TeapotExplosion::modifyColorThreshhold() {
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		if (colorChangeThreshold < 1.0f)
			colorChangeThreshold += 0.005f;
	}
	else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		if (colorChangeThreshold > 0.0f)
			colorChangeThreshold -= 0.005f;
	}
}

void TeapotExplosion::rotateColor() {
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) 
		blueRotationTheta += 0.025f;
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		blueRotationTheta -= 0.025f;

	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		greenRotationTheta += 0.025f;
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		greenRotationTheta -= 0.025f;
}

void TeapotExplosion::updateColorModificationValues() {
	
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		colorModificationValues[0] += 0.33f * COLOR_MOD_VALUE_CHANGE_SPEED;
	}
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
		colorModificationValues[0] -= 0.33f * COLOR_MOD_VALUE_CHANGE_SPEED;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		colorModificationValues[1] += COLOR_MOD_VALUE_CHANGE_SPEED;
	}
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
		colorModificationValues[1] -= COLOR_MOD_VALUE_CHANGE_SPEED;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		colorModificationValues[2] += COLOR_MOD_VALUE_CHANGE_SPEED;
	}
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
		colorModificationValues[2] -= COLOR_MOD_VALUE_CHANGE_SPEED;
	}
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
		colorModificationValues[3] += COLOR_MOD_VALUE_CHANGE_SPEED;
	}
	if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) {
		colorModificationValues[3] -= COLOR_MOD_VALUE_CHANGE_SPEED;
	}
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
		colorModificationValues[4] += COLOR_MOD_VALUE_CHANGE_SPEED;
	}
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
		colorModificationValues[4] -= COLOR_MOD_VALUE_CHANGE_SPEED;
	}
	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {
		colorModificationValues[5] += COLOR_MOD_VALUE_CHANGE_SPEED;
	}
	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) {
		colorModificationValues[5] -= COLOR_MOD_VALUE_CHANGE_SPEED;
	}
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
		colorModificationValues[6] += COLOR_MOD_VALUE_CHANGE_SPEED;
	}
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
		colorModificationValues[6] -= COLOR_MOD_VALUE_CHANGE_SPEED;
	}

	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
		for (int i = 0; i < COLOR_MOD_VALUES_COUNT; i++) {
			colorModificationValues[i] = MathFunc::getRandomInRangef(-110.0f, 110.0f);
		}
		colorModificationValues[0] = 1.0f;
	}
	else if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
		for (int i = 0; i < COLOR_MOD_VALUES_COUNT; i++) {
			colorModificationValues[i] = colorModificationValues[i] * 0.8f;
		}
	}
}

void TeapotExplosion::updateVelocity() {
	if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS) {
		velocity *= 1.05f;
	}
	if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS) {
		velocity /= 1.05f;
	}
	
	//Ensure there is a minimum velocity
	if (velocity < 0.05f) {
		velocity = 0.05f;
	}
}


void TeapotExplosion::updateFrameClearColor() {
	

	float * red = &(backgroundColor.x);
	float * green = &(backgroundColor.y);
	float * blue = &(backgroundColor.z);

	*red = glm::min(1.0f, (*red + *green + *blue) / 3.0f);
	*green = glm::abs(sin(glm::min(1.0f, (*red) + ((*blue) * (*blue) / (*red)))));
	*blue = 0.5f + 0.25f*cos(counter);

	if (abs(ceilf(counter) - counter) < 0.0049f) {
		float temp = *red;
		*red = *green;
		*green = *blue;
		*blue = temp;
	}

	glClearColor(*red, *green, *blue, 1.0f);
}

void TeapotExplosion::updateUniforms() {
	sceneShader->use();
	//Update uniform locations
	sceneShader->uniforms.updateUniform1f("zoom", 1.7f + counter);
	sceneShader->uniforms.updateUniform1f("time", 0.725f*counter);

	//Uniforms for the geometry shader effect
	sceneShader->uniforms.updateUniform1i("level", 1);                          //tweak this value as needed
	sceneShader->uniforms.updateUniform1f("gravity", -0.91f /*-29.81f*/);       //tweak this value as needed
	sceneShader->uniforms.updateUniform1f("velocityScale", velocity);               //tweak this value as needed

	xRotation += 0.0000f;
	yRotation += 0.013575f;    //0.009625f; // 0.012375f;  //0.015125f;
	zRotation += 0.0012375035f;

	//fprintf(MSGLOG, "xRot is %f, yRot is %s, zRot is %f\n", xRotation, yRotation, zRotation);
	sceneShader->uniforms.updateUniform1f("xRotation", xRotation);
	sceneShader->uniforms.updateUniform1f("yRotation", yRotation);
	sceneShader->uniforms.updateUniform1f("zRotation", zRotation);

	sceneShader->uniforms.updateUniform1f("colorShiftThreshhold", colorChangeThreshold);

	sceneShader->uniforms.updateUniform1f("redRotationTheta", redRotationTheta);
	sceneShader->uniforms.updateUniform1f("greenRotationTheta", greenRotationTheta);
	sceneShader->uniforms.updateUniform1f("blueRotationTheta", blueRotationTheta);

	/* 
	sceneShader->uniforms.updateUniform1f("colorModificationValue0", colorModificationValues[0]);
	sceneShader->uniforms.updateUniform1f("colorModificationValue1", colorModificationValues[1]);
	sceneShader->uniforms.updateUniform1f("colorModificationValue2", colorModificationValues[2]);
	sceneShader->uniforms.updateUniform1f("colorModificationValue3", colorModificationValues[3]);
	sceneShader->uniforms.updateUniform1f("colorModificationValue4", colorModificationValues[4]);
	sceneShader->uniforms.updateUniform1f("colorModificationValue5", 8.0f*colorModificationValues[5]);
	sceneShader->uniforms.updateUniform1f("colorModificationValue6", colorModificationValues[6]);
	*/
}

void TeapotExplosion::drawVerts() {
	if (sceneShader)
		sceneShader->use();
	if (vertexAttributes)
		vertexAttributes->use();

	if (currentTriangleInputType == PIPELINE_PRIMATIVE_INPUT_TYPE::DISCRETE_TRIANGLES) 
		glDrawArrays(GL_TRIANGLES, 0, teapot_count / 3); 
	
	if (currentTriangleInputType == PIPELINE_PRIMATIVE_INPUT_TYPE::TRIANGLE_STRIP) 
		glDrawArrays(GL_TRIANGLE_STRIP, 0, teapot_count / 3);
	
	if (currentTriangleInputType == PIPELINE_PRIMATIVE_INPUT_TYPE::TRIANGLE_FAN)
		glDrawArrays(GL_TRIANGLE_FAN, 0, teapot_count / 3);

}


void TeapotExplosion::prepareGLContextForNextFrame() {
	glBindVertexArray(0);
	glUseProgram(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
