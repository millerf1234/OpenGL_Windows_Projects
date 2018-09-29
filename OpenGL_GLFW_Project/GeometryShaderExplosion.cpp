//See header file for details
//This file will most likely closely resemble the file "RenderProject1.cpp"

#include "GeometryShaderExplosion.h"


void GeometryShaderExplosion::initialize() {
	error = false;
	window = nullptr;
	frameNumber = 0ull;
	frameUnpaused = 0ull;
	frameOfMostRecentColorRecording = 0ull;
	counter = 0.0f;
	zRotation = 0.0f;

	//Set initial background color
	backgroundColor = glm::vec3(0.0f, 0.5f, 0.75f);


	glEnable(GL_PROGRAM_POINT_SIZE);


}



GeometryShaderExplosion::GeometryShaderExplosion(std::shared_ptr<MonitorData> screenInfo) {
	initialize();
	//Make sure we have a monitor to render to
	if (!screenInfo || !screenInfo->activeMonitor) {
		error = true;
		return;
	}
	//Make sure the context is set to this monitor (and this thread [see glfw documentation])
	if (glfwGetCurrentContext() != screenInfo->activeMonitor) {
		std::ostringstream warning;
		warning << "\nWARNING!\n[In GeometryShaderExplosion's constructor]\n" <<
			"GeometryShaderExplosion detected that the glfw active context was set" <<
			"\nto a different monitor or different execution-thread then\n" <<
			"the one passed to GeometryShaderExplosion's contructor!\n";
		warning << "This means that running GeometryShaderExplosion will invalidate\n" <<
			"the previoud context by replacing it with this one, which\n" <<
			"could lead to errors! Please ensure that the correct context is\n" <<
			"being passed to GeometryShaderExplosion in the application code!\n";

		fprintf(WRNLOG, warning.str().c_str());
		glfwMakeContextCurrent(screenInfo->activeMonitor);
	}
	window = screenInfo->activeMonitor;
}



GeometryShaderExplosion::~GeometryShaderExplosion() {

}


void GeometryShaderExplosion::run() {
	if (error) {
		fprintf(ERRLOG, "An error occured while loading GeometryShaderExplosion\n");
		return;
	}
	fprintf(MSGLOG, "\nGeometryShaderExplosion demo project has loaded and will begin running!\n");


	fprintf(MSGLOG, "\n\tDemo Starting...!\n");


	fprintf(MSGLOG, "\nEntering Render Loop...\n");


	renderLoop();


}

void GeometryShaderExplosion::loadAssets() {
	loadShaders(); //load the GLSL shader code
	loadTeapot();  //have the GL context load the Teapot vertices to video memory 
}


void GeometryShaderExplosion::loadShaders() {
	fprintf(MSGLOG, "\nInitializing Shaders!\n");
	sceneShader = std::make_unique<ShaderProgram>();

	//There is just 1 pipeline here to build. Here is the pipeline
	
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
	sceneShader->attachVert("GeometryShaderExplosion.vert");
	//---------------------------

	//---------------------------
	//     (Geometry Stage) 
	// Attach the primary geometry shader to the pipeline. (This is where the explosion happens)
	fprintf(MSGLOG, "\nAttaching geometry shader!\n");
	sceneShader->attachGeom("GeometryShaderExplosion.geom");
	//--------------------------


	//--------------------------
	//     (Fragment Stage)
	// Attach the primary Fragment Shader to the pipeline
	fprintf(MSGLOG, "\nAttaching fragment shader!\n");
	sceneShader->attachFrag("GeometryShaderExplosion.frag");
	fprintf(MSGLOG, "\nAttempting to link program!\n");
	//---------------------------

	//    AT THIS POINT, ALL OF THE PIPELINE STAGES HAVE BEEN ATTACHED, SO LINK THE PIPELINE AND CHECK FOR ERRORS
	sceneShader->link();
	if (sceneShader->checkIfLinked()) {   
		fprintf(MSGLOG, "Program Successfully linked!\n");
	}
	else {
		fprintf(ERRLOG, "Shader Program was not successfully linked!\n");
		std::cin.get(); //Hold the window open if there was an error
	}

}

void GeometryShaderExplosion::loadTeapot() {

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

void GeometryShaderExplosion::renderLoop() {
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


bool GeometryShaderExplosion::checkToSeeIfShouldCloseWindow() const {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		return true;
	}
	return false;
}

bool GeometryShaderExplosion::checkIfShouldPause() const {
	if ((frameNumber >= (frameUnpaused + DELAY_LENGTH_OF_PAUSE_CHECKING_AFTER_UNPAUSE))
		&& (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)) {
		return true;
	}
	return false;
}

bool GeometryShaderExplosion::checkIfShouldRecordColor() const {
	if ((frameNumber >= (frameOfMostRecentColorRecording +
		DELAY_BETWEEN_SCREEN_COLOR_RECORDINGS_IN_RENDER_PROJECT_1))
		&& (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)) {
		return true;
	}
	return false;
}

void GeometryShaderExplosion::recordColorToLog() {
	frameOfMostRecentColorRecording = frameNumber;
	int colorDigits = 6; //Digits to print out for each color

						 //Syntax Note: With '%*f', the '*' means that the width will be provided as an additional parameter
	fprintf(MSGLOG, "\nThe background color of frame %llu is:\n\tRed: %*f,\tGreen: %*f,\tBlue: %*f\n",
		frameNumber, colorDigits, backgroundColor.r, colorDigits, backgroundColor.g, colorDigits, backgroundColor.b);

}

void GeometryShaderExplosion::pause() {
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

void GeometryShaderExplosion::updateFrameClearColor() {
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

void GeometryShaderExplosion::updateUniforms() {
	sceneShader->use();
	//Update uniform locations
	sceneShader->uniforms->updateUniform1f("zoom", 1.0f);
	sceneShader->uniforms->updateUniform1f("time", counter * 0.1f);
	
	//Uniforms for the geometry shader effect
	sceneShader->uniforms->updateUniform1i("level", 10);             //tweak this value as needed
	sceneShader->uniforms->updateUniform1f("gravity", -9.81f);       //tweak this value as needed
	sceneShader->uniforms->updateUniform1f("velocityScale", 3.0f);  //tweak this value as needed
	

	zRotation += 0.005f;
	//fprintf(MSGLOG, "zRot is %f\n", zRotation);
	//sceneShader->uniforms->updateUniform1f("xRotation", 0.0f);
	//sceneShader->uniforms->updateUniform1f("yRotation", 0.0f*zRotation * 4.0f);
	sceneShader->uniforms->updateUniform1f("zRotation", zRotation);
}

void GeometryShaderExplosion::drawVerts() {
	if (sceneShader)
		sceneShader->use();
	if (vertexAttributes)
		vertexAttributes->use();

	/*glDrawArrays(GL_TRIANGLES, 0, 3);*/
	glDrawArrays(GL_TRIANGLES, 0, teapot_count / 3);
}


void GeometryShaderExplosion::prepareGLContextForNextFrame() {
	glBindVertexArray(0);
	glUseProgram(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
