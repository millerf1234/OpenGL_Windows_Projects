//See header file for details
//This file will most likely closely resemble the file "RenderProject1.cpp"

#include "TeapotExplosion.h"

#include "AsciiAsset.h"

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

	//Set initial background color
	backgroundColor = glm::vec3(0.25f, 0.5f, 0.75f);

	//Set the starting input primitive type
	currentTriangleInputType = PIPELINE_TRIANGLE_INPUT_TYPE::NORMAL;

	colorChangeThreshold = 0.12f;


	//AssetLoadingInternal::AsciiAsset tst("obj/BeveledCube.mtl");
	//fprintf(MSGLOG, "\nLine 4 is %u long.\n", tst.getLineLength(3));
	//std::cin.get();

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
	sceneShader = std::make_unique<ShaderProgram>();

	//There is just 1 pipeline here to build. Here is the pipeline
	fprintf(MSGLOG, "Assembling the main shader for the scene...\n");
	//---------------------------
	//    (VERTEX STAGE)
	// Attach a helper shader with just some useful functions
	fprintf(MSGLOG, "\nAttaching secondary helper vertex shader!\n");
	std::unique_ptr<ShaderInterface::VertexShader> vertHelper = std::make_unique<ShaderInterface::VertexShader>("VertMath.vert");
	if (!vertHelper)
		return;
	vertHelper->makeSecondary();
	sceneShader->attachSecondaryVert(vertHelper.get());
	//Attach the primary vertex shader
	fprintf(MSGLOG, "\nAttaching main vertex shader!\n");
	//sceneShader->attachVert("TeapotExplosion.vert");
	std::unique_ptr<ShaderInterface::VertexShader> mainVert = std::make_unique<ShaderInterface::VertexShader>("TeapotExplosion.vert");
	if (!mainVert)
		return;
	sceneShader->attachVert(mainVert.get());

	//---------------------------

	//---------------------------
	//     (Geometry Stage) 
	// Attach the primary geometry shader to the pipeline. (This is where the explosion happens)
	fprintf(MSGLOG, "\nAttaching geometry shader!\n");
	sceneShader->attachGeom("TeapotExplosion.geom");
	//--------------------------


	//--------------------------
	//     (Fragment Stage)
	// Attach the primary Fragment Shader to the pipeline
	fprintf(MSGLOG, "\nAttaching fragment shader!\n");
	sceneShader->attachFrag("TeapotExplosion.frag");
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

	//Psych
	//Build the sceneShaderLine as a second graphics pipeline
	fprintf(MSGLOG, "\nCreating a second Graphics Pipeline...\n");
	sceneShaderLine = std::make_unique<ShaderProgram>();
	fprintf(MSGLOG, "\nReusing the same two Vertex Shaders as used in the first pipeline\n");
	sceneShaderLine->attachSecondaryVert(vertHelper.get());
	fprintf(MSGLOG, "\tHelper Vertex Shader Attached!\n");
	sceneShaderLine->attachVert(mainVert.get());
	fprintf(MSGLOG, "\tPrimary Vertex Shader Attached!\n");

	
	fprintf(MSGLOG, "\nAttaching a different Geometry Shader as from the first pipeline...\n");
	sceneShaderLine->attachGeom("TeapotOutline.geom");
	fprintf(MSGLOG, "\tGeometry Shader Attached!\n");

	fprintf(MSGLOG, "\nAttaching a different Fragment Shader as well for this alternate pipeline...\n");
	sceneShaderLine->attachFrag("TeapotOutline.frag");
	fprintf(MSGLOG, "\tFragment Shader Attached!\n");

	fprintf(MSGLOG, "\nTime to link the second pipeline...\n");
	if (!sceneShaderLine->readyToLink()) {
		fprintf(WRNLOG, "\n!!!WARNING!!! The Second Graphics Pipeline is not ready to link for some reason!\n");
		fprintf(ERRLOG, "\t[PRESS ENTER TO CONTINUE]\n");
		std::cin.get();
		return;
	}
	else {
		sceneShaderLine->link();
		if (sceneShaderLine->checkIfLinked()) {
			fprintf(MSGLOG, "Second Pipeline Program Successfully linked!\n");
		}
		else {
			fprintf(ERRLOG, "Shader Program Line was not successfully linked!\n");
			fprintf(MSGLOG, "\t[Press 'ENTER' to attempt to continue program execution]\n");
			std::cin.get(); //Hold the window open if there was an error
		}
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
	vertexAttributes = std::make_unique<GenericVertexAttributeSet>(1);

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


		updateFrameClearColor();

		updateUniforms();


		drawVerts();

		counter += 0.000125f;

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
		DELAY_BETWEEN_SCREEN_COLOR_RECORDINGS_IN_RENDER_PROJECT_1))
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
			std::this_thread::sleep_for(std::chrono::nanoseconds(3333333));
		}
	}
}

void TeapotExplosion::recordColorToLog() {
	frameOfMostRecentColorRecording = frameNumber;
	int colorDigits = 6; //Digits to print out for each color

						 //Syntax Note: With '%*f', the '*' means that the width will be provided as an additional parameter
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
		currentTriangleInputType = PIPELINE_TRIANGLE_INPUT_TYPE::NORMAL;

	else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) 
		currentTriangleInputType = PIPELINE_TRIANGLE_INPUT_TYPE::STRIP;

	else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) 
		currentTriangleInputType = PIPELINE_TRIANGLE_INPUT_TYPE::FAN;
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

void TeapotExplosion::updateFrameClearColor() {
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

	glClearColor(*red, *green, *blue, 1.0f);
}

void TeapotExplosion::updateUniforms() {
	sceneShader->use();
	//Update uniform locations
	sceneShader->uniforms->updateUniform1f("zoom", 1.7f + counter);
	sceneShader->uniforms->updateUniform1f("time", 0.725f*counter);

	//Uniforms for the geometry shader effect
	sceneShader->uniforms->updateUniform1i("level", 1);             //tweak this value as needed
	sceneShader->uniforms->updateUniform1f("gravity", -0.91f /*-29.81f*/);       //tweak this value as needed
	sceneShader->uniforms->updateUniform1f("velocityScale", 1.0f);  //tweak this value as needed

	xRotation += 0.00000f;
	yRotation += 0.013575f;    //0.009625f; // 0.012375f;  //0.015125f;
	zRotation += 0.0012375035f;

	//fprintf(MSGLOG, "xRot is %f, yRot is %s, zRot is %f\n", xRotation, yRotation, zRotation);
	sceneShader->uniforms->updateUniform1f("xRotation", xRotation);
	sceneShader->uniforms->updateUniform1f("yRotation", yRotation);
	sceneShader->uniforms->updateUniform1f("zRotation", zRotation);

	sceneShader->uniforms->updateUniform1f("colorShiftThreshhold", colorChangeThreshold);

	sceneShader->uniforms->updateUniform1f("redRotationTheta", redRotationTheta);
	sceneShader->uniforms->updateUniform1f("greenRotationTheta", greenRotationTheta);
	sceneShader->uniforms->updateUniform1f("blueRotationTheta", blueRotationTheta);

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------

	/*
	sceneShaderLine->use();
	//Update uniform locations
	sceneShaderLine->uniforms->updateUniform1f("zoom", 1.75f);
	sceneShaderLine->uniforms->updateUniform1f("time", 0.725f*counter);

	//Uniforms for the geometry shader effect
	sceneShaderLine->uniforms->updateUniform1i("level", 1);             //tweak this value as needed
	sceneShaderLine->uniforms->updateUniform1f("gravity", -29.81f);       //tweak this value as needed
	sceneShaderLine->uniforms->updateUniform1f("velocityScale", 1.0f);  //tweak this value as needed

	xRotation += 0.0f;
	yRotation += 0.0045f;
	zRotation += 0.00035f;

	//fprintf(MSGLOG, "xRot is %f, yRot is %s, zRot is %f\n", xRotation, yRotation, zRotation);
	sceneShaderLine->uniforms->updateUniform1f("xRotation", xRotation + counter);
	sceneShaderLine->uniforms->updateUniform1f("yRotation", 2.75f * yRotation);
	sceneShaderLine->uniforms->updateUniform1f("zRotation", 2.75f*zRotation);
	*/

}

void TeapotExplosion::drawVerts() {
	if (sceneShader)
		sceneShader->use();
	if (vertexAttributes)
		vertexAttributes->use();

	if (currentTriangleInputType == PIPELINE_TRIANGLE_INPUT_TYPE::NORMAL) 
		glDrawArrays(GL_TRIANGLES, 0, teapot_count / 3);
	
	if (currentTriangleInputType == PIPELINE_TRIANGLE_INPUT_TYPE::STRIP) 
		glDrawArrays(GL_TRIANGLE_STRIP, 0, teapot_count / 3);
	
	if (currentTriangleInputType == PIPELINE_TRIANGLE_INPUT_TYPE::FAN)
		glDrawArrays(GL_TRIANGLE_FAN, 0, teapot_count / 3);

	/*
	if (sceneShaderLine)
		sceneShaderLine->use();
	glDrawArrays(GL_TRIANGLES, 0, teapot_count / 3);
	*/
}


void TeapotExplosion::prepareGLContextForNextFrame() {
	glBindVertexArray(0);
	glUseProgram(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
