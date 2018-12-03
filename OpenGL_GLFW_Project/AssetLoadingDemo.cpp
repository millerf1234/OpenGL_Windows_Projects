//File:        AssetLoadingDemo.cpp
//
//
//Programmer:  Forrest Miller
//Date:        November 14, 2018



#include "AssetLoadingDemo.h"


void AssetLoadingDemo::initialize() {
	error = false;
	frameNumber = 0ull;
	frameUnpaused = 0ull;
	frameLineTypeLastSwitched = 0ull;
	frameInstancedDrawingBehaviorLastToggled = 0ull;
	frameInstancedDrawingCountLastModified = 0ull;
	counter = 0.0f;
	vao = vbo = 0u;

	//Set the starting input primitive type
	currentPrimativeInputType = PIPELINE_PRIMATIVE_INPUT_TYPE::DISCRETE_TRIANGLES;

	//Set the variables regarding instanced drawing
	drawMultipleInstances = true;
	instanceCount = STARTING_INSTANCE_COUNT;
	instanceSpiralPatternPeriod_x = STARTING_INSTANCE_SPIRAL_PATTERN_PERIOD_X;
	instanceSpiralPatternPeriod_y = STARTING_INSTANCE_SPIRAL_PATTERN_PERIOD_Y;

	//Set values for screen projection 
	fov = 56.0f;
	screenHeight = 2160.0f;
	screenWidth = 3840.0f;
	zNear = 0.5f;
	zFar = 100.0f;
	//Compute the screen -projection matrix
	//perspective = glm::mat4(1.0f); //Set projection matrix to 4x4 identity
	//see: https://gamedev.stackexchange.com/questions/98226/how-can-i-set-up-an-intuitive-perspective-projection-view-matrix-combination-in
	perspective = glm::perspectiveFov(fov, screenWidth, screenHeight, zNear, zFar);

	//Set values for view matrix
	cameraPos = glm::vec3(0.0f, 0.0f, 5.0f); //3.0f * 0.5f / tan(glm::radians(fov / 2.f)));
	lookAtOrgin = glm::vec3(0.0f, 0.0f, -1.0f);
	upDirection = glm::vec3(0.0f, 1.0f, 0.0f);
	//Compute view matrix
	view = glm::lookAt(cameraPos, lookAtOrgin, upDirection);

	//Set values for Rotation Uniforms
	head = 0.0f;
	pitch = 0.0f;
	roll = 0.0f;
	//Calculate rotation matrix
	rotation = glm::mat4(1.0f);    //Set rotation matrix to 4x4 identity matrix

	//Keep an extra zoom parameter
	zoom = 1.0f; //Higher number means farther away


	//Set initial background color
	backgroundColor = glm::vec3(0.25f, 0.5f, 0.75f);

}


AssetLoadingDemo::AssetLoadingDemo(std::shared_ptr<MonitorData> screenInfo) : RenderDemoBase() {
	initialize();
	//Make sure we have a monitor to render to
	if (!screenInfo || !screenInfo->activeMonitor) {
		error = true;
		return;
	}
	//Make sure the context is set to this monitor (and this thread [see glfw documentation])
	if (glfwGetCurrentContext() != screenInfo->activeMonitor) {
		std::ostringstream warning;
		warning << "\nWARNING!\n[In AssetLoadingDemo's constructor]\n" <<
			"AssetLoadingDemo detected that the GLFW active context was set" <<
			"\nto a different monitor or different execution-thread then\n" <<
			"the one passed to AssetLoadingDemo's contructor!\n";
		warning << "This means that running AssetLoadingDemo will invalidate\n" <<
			"the previous context by replacing it with this one, which\n" <<
			"could (probably) lead to errors! Please ensure that the correct context\n" <<
			"is being passed to AssetLoadingDemo in the application code!\n";

		fprintf(WRNLOG, warning.str().c_str());
		glfwMakeContextCurrent(screenInfo->activeMonitor);
	}
	window = screenInfo->activeMonitor;
}



AssetLoadingDemo::~AssetLoadingDemo() {

}


void AssetLoadingDemo::run() {
	if (error) {
		fprintf(ERRLOG, "An error occured while loading AssetLoadingDemo\n");
		return;
	}
	fprintf(MSGLOG, "\nAsset Loading Demo project has loaded and will begin running!\n");


	fprintf(MSGLOG, "\n\tDemo Starting...!\n");


	fprintf(MSGLOG, "\nEntering Render Loop...\n");


	renderLoop();

}




void AssetLoadingDemo::loadAssets() {
	loadShaders(); //load the GLSL shader code
	loadModels();  //have the GL context load the Teapot vertices to video memory 

	prepareScene();
}



void AssetLoadingDemo::loadShaders() { 

	std::string shadersRFP = FILEPATH_TO_SHADERS;   //Relative Filepath to location of Shaders

	fprintf(MSGLOG, "\nInitializing Shaders!\n");

	sceneShader = std::make_unique<ShaderProgram>(); //Create the scene shader

	//Attach the main shader stages to the sceneShader
	sceneShader->attachVert(shadersRFP + std::string("AssetLoadingDemo.vert")); //Attach Vertex shader to scene
	sceneShader->attachFrag(shadersRFP + std::string("AssetLoadingDemo.frag")); //Attach Fragment shader to scene


	// [Each shader stage requires its own set of secondary functions]
	//Create and attach a secondary vertex shader containing implementations for some noise functions
	std::unique_ptr<ShaderInterface::VertexShader> vertexNoiseShader =
		std::make_unique<ShaderInterface::VertexShader>(shadersRFP + std::string("VoronoiNoise.glsl"));
	vertexNoiseShader->makeSecondary();
	sceneShader->attachSecondaryVert(vertexNoiseShader.get()); //the '.get()' function converts the unique_ptr to a raw pointer


	//Create and attach a secondary fragment shader containing implementations for some noise functions 
	std::unique_ptr<ShaderInterface::FragmentShader> fragmentNoiseShader =
		std::make_unique<ShaderInterface::FragmentShader>(shadersRFP + std::string("VoronoiNoise.glsl"));
	fragmentNoiseShader->makeSecondary();
	sceneShader->attachSecondaryFrag(fragmentNoiseShader.get()); //the '.get()' function converts the unique_ptr to a raw pointer


	//Now after all the stages to the shader have been created and attached, it is time to link the sceneShader
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




GLsizei AssetLoadingDemo::computeNumberOfVerticesInSceneBuffer() const {
	static constexpr const GLsizei vertexSize = 4u + 2u + 3u;  //Since each vertex is {x,y,z,w, s,t, nx,ny,nz}  (i.e. 9 components total)
	return (sceneBuffer.size() / vertexSize);
}


void AssetLoadingDemo::loadModels() {

	std::string modelsRFP = FILEPATH_TO_MODELS; //Set string to location of Model Files

	//Initial Scale values for the objects
	float blockThing_QuadsScale = 4.2f;
	float beveledCubeScale = 5.0f;
	float blockShipScale = 4.5f;
	float subdivisionCubeScale = 4.9f;
	float abstractShapeScale = 2.0f;

	//Load some models
	//sceneObjects.emplace_back(std::make_unique<QuickObj>(modelsRFP + "blockThing_Quads.obj", blockThing_QuadsScale));
	//sceneObjects.emplace_back(std::make_unique<QuickObj>(modelsRFP + "BeveledCube.obj", beveledCubeScale));
	//sceneObjects.emplace_back(std::make_unique<QuickObj>(modelsRFP + "BlockshipSampleExports\\BlockShipSample_01_3DCoatExport01.obj", blockShipScale));
	//sceneObjects.emplace_back(std::make_unique<QuickObj>(modelsRFP + "SubdivisionCube.obj", subdivisionCubeScale)); //Has no text coords
	sceneObjects.emplace_back(std::make_unique<QuickObj>(modelsRFP + "AbstractShape.obj", abstractShapeScale)); //Only position data

	//sceneObjects.emplace_back(std::make_unique<QuickObj>(modelsRFP + "ParentedPrimatives.obj", 3.2f));

	fprintf(MSGLOG, "\n%u models have been loaded.\n", sceneObjects.size());

}

void AssetLoadingDemo::prepareScene() {
	fprintf(MSGLOG, "\nCreating the scene from loaded assets...\n");
	buildSceneBufferFromLoadedSceneObjects();
	fprintf(MSGLOG, "Scene creation complete!\n");

	fprintf(MSGLOG, "Uploading scene to GPU...\n");
	uploadSceneBufferToGPU();
	configureVertexArrayAttributes(); 
}



void AssetLoadingDemo::renderLoop() {
	while (glfwWindowShouldClose(window) == GLFW_FALSE) {
		//Check Input
		if (checkToSeeIfShouldCloseWindow()) {
			glfwSetWindowShouldClose(window, GLFW_TRUE);
			continue; //Skip the rest of this loop iteration to close window quickly
		}
		if (checkIfShouldPause()) {
			pause();
			continue;
		}
		if (checkIfShouldReset()) {
			if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
				pause();
				continue;
			}
			reset();
		}
		//More Input Checking
		changePrimitiveType();
		changeInstancedDrawingBehavior();
		rotate();
		if (drawMultipleInstances) {
			modifyInstancedDrawingSpiralPattern();
		}

		//Set up to draw frame
		updateFrameClearColor(); //background color
		updateUniforms(); 

		//Draw frame
		drawVerts();

		counter += 0.0125f; //Increment time 

		glfwSwapBuffers(window); //Swap the buffer to present image to monitor

		glfwPollEvents();

		frameNumber++; //Increment the frame counter
		prepareGLContextForNextFrame(); 
	}

}


bool AssetLoadingDemo::checkToSeeIfShouldCloseWindow() const {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		return true;
	}
	return false;
}

bool AssetLoadingDemo::checkIfShouldPause() const {
	if ((frameNumber >= (frameUnpaused + DELAY_LENGTH_OF_PAUSE_CHECKING_AFTER_UNPAUSE))
		&& (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)) {
		return true;
	}
	return false;
}


bool AssetLoadingDemo::checkIfShouldReset() const {
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
		return true;
	return false;

}

void AssetLoadingDemo::pause() {
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


void AssetLoadingDemo::reset() {
	fprintf(MSGLOG, "\nReseting Demo...\n");
	counter = 0.0f; //Reset time to 0
	head = 0.0f; //Reset rotation
	pitch = 0.0f;
	roll = 0.0f;
	backgroundColor = glm::vec3(0.15f, 0.5f, 0.75f);
	frameNumber = 0ull;
	frameUnpaused = 0ull;
	frameLineTypeLastSwitched = 0ull;
	frameInstancedDrawingBehaviorLastToggled = 0ull;
	frameInstancedDrawingCountLastModified = 0ull;
	zoom = 1.0f;
	if (drawMultipleInstances) {
		instanceCount = STARTING_INSTANCE_COUNT;
		instanceSpiralPatternPeriod_x = STARTING_INSTANCE_SPIRAL_PATTERN_PERIOD_X;
		instanceSpiralPatternPeriod_y = STARTING_INSTANCE_SPIRAL_PATTERN_PERIOD_Y;
	}
}



void AssetLoadingDemo::changeInstancedDrawingBehavior() {
	//Only allow toggling to happen every 11 frames
	if ((frameNumber - frameInstancedDrawingBehaviorLastToggled) > 11ull) {
		if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
			frameInstancedDrawingBehaviorLastToggled = frameNumber; //Mark the current frame as being the one when instanced drawing behavior was last toggled
			drawMultipleInstances = !drawMultipleInstances; //Perform Toggle
			fprintf(MSGLOG, "Instanced Rendering set to: %s",
				(drawMultipleInstances ? ("Enabled\n") : ("Disabled\n")));
		}
	}

	//Only allow the instance count to be modified every 12 frames (1/5th second)
	if ((frameNumber - frameInstancedDrawingCountLastModified) > 12ull) {
		if (drawMultipleInstances) {
			if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS) {
				frameInstancedDrawingBehaviorLastToggled = frameNumber; 
				instanceCount++;
				fprintf(MSGLOG, "Rendered Instances increased to: %u\n", instanceCount);
			}
			else if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS) {
				if (instanceCount > 0u) { //Don't decrement unsigned value below 0
					frameInstancedDrawingBehaviorLastToggled = frameNumber;
					instanceCount--;
					fprintf(MSGLOG, "Rendered Instances decreased to: %u\n", instanceCount);
				}
			}
		}
	}
}

void AssetLoadingDemo::modifyInstancedDrawingSpiralPattern() {
	GLfloat xChangeRate = 0.045f;
	GLfloat yChangeRate = 0.045f;
	GLfloat leftShiftFactor = 5.0f; //Holding down left shift will cause value to change leftShiftFactor times faster
	GLfloat rightShiftFactor = 25.0f; //Holding down right shift will cause values to change rightShiftFactor times faster

	//Check for updates to X period
	if (glfwGetKey(window, GLFW_KEY_LEFT_BRACKET) == GLFW_PRESS) {
		if (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) {
			instanceSpiralPatternPeriod_x -= xChangeRate * rightShiftFactor;
		}
		else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
			instanceSpiralPatternPeriod_x -= xChangeRate * leftShiftFactor;
		}
		else {
			instanceSpiralPatternPeriod_x -= xChangeRate;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT_BRACKET) == GLFW_PRESS) {
		if (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) {
			instanceSpiralPatternPeriod_x += xChangeRate * rightShiftFactor;
		}
		else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
			instanceSpiralPatternPeriod_x += xChangeRate * leftShiftFactor;
		}
		else {
			instanceSpiralPatternPeriod_x += xChangeRate;
		}
	}


	//Check for updates to Y period
	if (glfwGetKey(window, GLFW_KEY_SEMICOLON) == GLFW_PRESS) {
		if (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) {
			instanceSpiralPatternPeriod_y -= yChangeRate * rightShiftFactor;
		}
		else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
			instanceSpiralPatternPeriod_y -= yChangeRate * leftShiftFactor;
		}
		else {
			instanceSpiralPatternPeriod_y -= yChangeRate;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_APOSTROPHE) == GLFW_PRESS) {
		if (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) {
			instanceSpiralPatternPeriod_y += yChangeRate * rightShiftFactor;
		}
		else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
			instanceSpiralPatternPeriod_y += yChangeRate * leftShiftFactor;
		}
		else {
			instanceSpiralPatternPeriod_y += yChangeRate;
		}
	}
}

void AssetLoadingDemo::changePrimitiveType() {
	
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		currentPrimativeInputType = PIPELINE_PRIMATIVE_INPUT_TYPE::DISCRETE_TRIANGLES;

	else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		currentPrimativeInputType = PIPELINE_PRIMATIVE_INPUT_TYPE::TRIANGLE_STRIP;

	else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
		currentPrimativeInputType = PIPELINE_PRIMATIVE_INPUT_TYPE::TRIANGLE_FAN;

	if (glfwGetKey(window, GLFW_KEY_GRAVE_ACCENT) == GLFW_PRESS) {
		if ((frameNumber - frameLineTypeLastSwitched) < 15ull) {
			frameLineTypeLastSwitched = frameNumber;
		}
		else {
			frameLineTypeLastSwitched = frameNumber;
			if (currentPrimativeInputType == PIPELINE_PRIMATIVE_INPUT_TYPE::LINE) {
				currentPrimativeInputType = PIPELINE_PRIMATIVE_INPUT_TYPE::LINE_STRIP;
			}
			else {
				currentPrimativeInputType = PIPELINE_PRIMATIVE_INPUT_TYPE::LINE;
			}
		}
	}	
	
}


void AssetLoadingDemo::rotate() {

	if ((glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) ||
		(glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)) {

		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
			pitch += 0.1f;
		}
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
			pitch -= 0.1f;
		}

		if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
			zoom += 0.095f;
		}
		if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
			zoom -= 0.095f;
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
			roll += 0.51f;
		}
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
			roll -= 0.51f;
		}
	}
	else {
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
			head += 0.1f;
		}
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
			head -= 0.1f;
		}

		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
			roll += 0.1f;
		}
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
			roll -= 0.1f;
		}


		//Sneak scaling in here as well..
		if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
			zoom += 0.025f;
		}
		if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
			zoom -= 0.025f;
		}
	}
}



void AssetLoadingDemo::updateFrameClearColor() {
	glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, 1.0);
	backgroundColor.x = abs(sin(counter + backgroundColor.x));
	backgroundColor.y = abs(sin(counter + backgroundColor.y + PI/3.0f));
	backgroundColor.z = abs(sin(counter + backgroundColor.y + PI / 2.0f));

	backgroundColor /= backgroundColor.length(); //normalize backgroundColor
}

void AssetLoadingDemo::updateUniforms() {
	if (!sceneShader)
		return;

	sceneShader->use(); 

	//glm::mat4 proj = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	//sceneShader->uniforms->updateUniformMat4x4("projection", &proj);  //;(const float*)glm::value_ptr(transform));

	sceneShader->uniforms->updateUniform1f("time", counter);
	sceneShader->uniforms->updateUniform1f("zoom", zoom);

	rotation = MathFunc::computeRotationMatrix4x4(head, pitch, roll);
	sceneShader->uniforms->updateUniformMat4x4("rotation", &rotation);

	glm::mat4 MVP;
	//if (frameNumber % 120ull < 60ull) {
		MVP = perspective * (view * (rotation));
	//}
	//else {
	//	MVP = rotation;
	//}
	sceneShader->uniforms->updateUniformMat4x4("MVP", &MVP);
	

	if (drawMultipleInstances) {
		sceneShader->uniforms->updateUniform1f("instanceSpiralPatternPeriod_x", instanceSpiralPatternPeriod_x);
		sceneShader->uniforms->updateUniform1f("instanceSpiralPatternPeriod_y", instanceSpiralPatternPeriod_y);
	}

}

void AssetLoadingDemo::drawVerts() {

	if (sceneShader)
		sceneShader->use();

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);


	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);



	if (currentPrimativeInputType == PIPELINE_PRIMATIVE_INPUT_TYPE::DISCRETE_TRIANGLES) {
		if (drawMultipleInstances) {
			glDrawArraysInstanced(GL_TRIANGLES, 0, computeNumberOfVerticesInSceneBuffer(), instanceCount);
		}
		else {
			glDrawArrays(GL_TRIANGLES, 0, computeNumberOfVerticesInSceneBuffer());
		}
	}

	else if (currentPrimativeInputType == PIPELINE_PRIMATIVE_INPUT_TYPE::TRIANGLE_STRIP) {
		if (drawMultipleInstances) {
			glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, computeNumberOfVerticesInSceneBuffer(), instanceCount);
		}
		else {
			glDrawArrays(GL_TRIANGLE_STRIP, 0, computeNumberOfVerticesInSceneBuffer());
		}
	}
	else if (currentPrimativeInputType == PIPELINE_PRIMATIVE_INPUT_TYPE::TRIANGLE_FAN) {
		if (drawMultipleInstances) {
			glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, computeNumberOfVerticesInSceneBuffer(), instanceCount);
		}
		else {
			glDrawArrays(GL_TRIANGLE_FAN, 0, computeNumberOfVerticesInSceneBuffer());
		}
	}
	else if (currentPrimativeInputType == PIPELINE_PRIMATIVE_INPUT_TYPE::LINE) {
		if (drawMultipleInstances) {
			glDrawArraysInstanced(GL_LINES, 0, computeNumberOfVerticesInSceneBuffer(), instanceCount);
		}
		else {
			glDrawArrays(GL_LINES, 0, computeNumberOfVerticesInSceneBuffer());
		}
	}
	else if (currentPrimativeInputType == PIPELINE_PRIMATIVE_INPUT_TYPE::LINE_STRIP) {
		if (drawMultipleInstances) {
			glDrawArraysInstanced(GL_LINE_STRIP, 0, computeNumberOfVerticesInSceneBuffer(), instanceCount);
		}
		else {
			glDrawArrays(GL_LINE_STRIP, 0, computeNumberOfVerticesInSceneBuffer());
		}
	}
}


void AssetLoadingDemo::prepareGLContextForNextFrame() {
	glBindVertexArray(0);
	glUseProgram(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}




///////////////////////////////////////////////////////////////////////////////////////////////////
////    The remaining functions are utility functions that are called by the setup functions   ////
///////////////////////////////////////////////////////////////////////////////////////////////////


void AssetLoadingDemo::buildSceneBufferFromLoadedSceneObjects() {

	const glm::vec3 POSITION_FIRST_OBJECT_IN_SCENE(-0.0f, -0.0f, -0.0f);
	const glm::vec3 CHANGE_BETWEEN_OBJECTS(0.5f, 0.5f, 0.00f);
	glm::vec3 objectPositionOffset = POSITION_FIRST_OBJECT_IN_SCENE;
	int objectCounter = 0;

	sceneBuffer.reserve(20000u * sceneObjects.size());  //This is just a rough guess as to the size of the scene data. 

														//Iterate through each object in the sceneObjects vector
	for (auto sceneObjIter = sceneObjects.begin(); sceneObjIter != sceneObjects.end(); sceneObjIter++) {
		objectCounter++;
		fprintf(MSGLOG, "\tAdding Object %d to scene...\n", objectCounter);
		fprintf(MSGLOG, "\t\tPosition of Object %d:   <%.2f, %.2f, %.2f>\n", objectCounter,
			objectPositionOffset.x, objectPositionOffset.y, objectPositionOffset.z);

		//Objects missing Normal and/or Texture Coordinates will need to have data generated for them
		//so that all vertices in the scene share the same format

		if ((*sceneObjIter)->hasNormals()) {
			if ((*sceneObjIter)->hasTexCoords()) {
				addObject(sceneObjIter, objectPositionOffset);
			}
			else { //The object has normals but does not have texture coordinates
				addObjectWithMissingTexCoords(sceneObjIter, objectPositionOffset);
			}
		}
		else { //Else the object did not come with loaded normals
			if ((*sceneObjIter)->hasTexCoords()) { 
			    addObjectWithMissingNormals(sceneObjIter, objectPositionOffset);
			}
			else { //The object consisted entirely of position data (no normals or texture coords)
				addObjectWithMissingTexCoordsAndNormals(sceneObjIter, objectPositionOffset);
			}
		}

		//Increment offset to prepare for the next object
		objectPositionOffset += CHANGE_BETWEEN_OBJECTS;
		if (objectPositionOffset.z > 1.0f) {
			objectPositionOffset.z = -0.05f;
		}
	}
}



//If the object already has both normals and texCoords, then all that is needed to be modified is 
//the object's position
void AssetLoadingDemo::addObject(std::vector<std::unique_ptr<QuickObj>>::iterator object,
	const glm::vec3& objPos) {

	int vertComponentCounter = -1; //variable will be incremented to '0' on start of first loop iteration

	auto vertsEnd = (*object)->mVertices_.end(); //Create a variable for loop exit condition
	for (auto vertIter = (*object)->mVertices_.begin(); vertIter != vertsEnd; vertIter++) {
		vertComponentCounter = ((vertComponentCounter + 1) % 9);
		if (vertComponentCounter == 0) {
			//Add x offset then add to vector
			sceneBuffer.push_back(objPos.x + (*vertIter));
		}
		else if (vertComponentCounter == 1) {
			//Add y offset then add to vector
			sceneBuffer.push_back(objPos.y + (*vertIter));
		}
		else if (vertComponentCounter == 2) {
			//Add z offset then add to vector
			sceneBuffer.push_back(objPos.z + (*vertIter));
		}
		else {
			//add the vertex data straight to vector
			sceneBuffer.push_back(*vertIter);
		}
	}
}

//If the object already has normal data but is missing texture coords, generate some made-up coordinates while 
//adding the object to the sceneBuffer
void AssetLoadingDemo::addObjectWithMissingTexCoords(std::vector<std::unique_ptr<QuickObj>>::iterator object,
	const glm::vec3& objPos) {
	//Setup the function to generate the texture coordinates	
	std::function<glm::vec2(void)> genTexCoord;
	if (ASSIGN_TEXTURE_COORDS_RANDOMLY) {
		genTexCoord = AssetLoadingDemo::generateRandomTexCoords;
	}
	else {
		genTexCoord = AssetLoadingDemo::generateConstantTexCoords;
	}


	int vertComponentCounter = -1; //variable will be incremented to '0' on start of first loop iteration
	glm::vec2 uvCoord;

	auto vertsEnd = (*object)->mVertices_.end(); //Create a variable for loop exit condition
	for (auto vertIter = (*object)->mVertices_.begin(); vertIter != vertsEnd; vertIter++) {
		vertComponentCounter = ((vertComponentCounter + 1) % 7);
		if (vertComponentCounter == 0) {
			//Add x offset then add to vector
			sceneBuffer.push_back(objPos.x + (*vertIter));
		}
		else if (vertComponentCounter == 1) {
			//Add y offset then add to vector
			sceneBuffer.push_back(objPos.y + (*vertIter));
		}
		else if (vertComponentCounter == 2) {
			//Add z offset then add to vector
			sceneBuffer.push_back(objPos.z + (*vertIter));
		}
		else if (vertComponentCounter == 3) {
			//Add the fourth position component to vector
			sceneBuffer.push_back(*vertIter);
			//Then generate the 2 texture components and add them to vector as well
			uvCoord = genTexCoord();
			sceneBuffer.push_back(uvCoord.s);
			sceneBuffer.push_back(uvCoord.t);
		}
		else { //Add the normal component to the vector
			sceneBuffer.push_back(*vertIter);
		}
	}
}


//If the object already has Texture Coords but is missing Vertex Normals, compute a normal for each triangle (note that 
//computing a triangle's normal will require position data for all 3 corners)
void AssetLoadingDemo::addObjectWithMissingNormals(std::vector<std::unique_ptr<QuickObj>>::iterator object,
	const glm::vec3& objPos) { 

	glm::vec3 v0, v1, v2, computedNormal;

	//Count the number of triangles for the object
	size_t numberOfTriangles = ((*object)->mVertices_.size() / 18u);

	//Loop through the object's data triangle by triangle
	for (size_t i = 0u; i < numberOfTriangles; i++) {
		auto triangleStart = ((*object)->mVertices_.begin() + (i * 18u));
		
		v0 = glm::vec3(*(triangleStart      ), *(triangleStart +  1u), *(triangleStart +  2u));
		v1 = glm::vec3(*(triangleStart +  6u), *(triangleStart +  7u), *(triangleStart +  8u));
		v2 = glm::vec3(*(triangleStart + 12u), *(triangleStart + 13u), *(triangleStart + 14u));

		computedNormal = MeshFunc::computeNormalizedVertexNormalsForTriangle(v0, v1, v2);


		for (size_t i = 0u; i < 3u; i++) {
			sceneBuffer.push_back(objPos.x + *(triangleStart + (i*6u)));         //x
			sceneBuffer.push_back(objPos.y + *(triangleStart + ((i*6u) + 1u)));  //y
			sceneBuffer.push_back(objPos.z + *(triangleStart + ((i*6u) + 2u)));  //z
			sceneBuffer.push_back(*(triangleStart + ((i*6u) + 3u)));             //w
			sceneBuffer.push_back(*(triangleStart + ((i*6u) + 4u)));             //s
			sceneBuffer.push_back(*(triangleStart + ((i*6u) + 5u)));             //t
			sceneBuffer.push_back(computedNormal.x);
			sceneBuffer.push_back(computedNormal.y);
			sceneBuffer.push_back(computedNormal.z);
		}
	}
}

//If both texture coordinates and normals are missing, use this funciton.
void AssetLoadingDemo::addObjectWithMissingTexCoordsAndNormals(std::vector<std::unique_ptr<QuickObj>>::iterator object,
	const glm::vec3& objPos) {

	//Setup the function to generate the texture coordinates	
	std::function<glm::vec2(void)> genTexCoord;
	if (ASSIGN_TEXTURE_COORDS_RANDOMLY) {
		genTexCoord = AssetLoadingDemo::generateRandomTexCoords;
	}
	else {
		genTexCoord = AssetLoadingDemo::generateConstantTexCoords;
	}

	glm::vec2 uvCoord;
	glm::vec3 v0, v1, v2, computedNormal;

	//Count the number of triangles for the object
	size_t numberOfTriangles = ((*object)->mVertices_.size() / 12u); 

	//Loop through the object's data triangle by triangle
	for (size_t i = 0u; i < numberOfTriangles; i++) {
		auto triangleStart = ((*object)->mVertices_.begin() + (i * 12u)); //triangleStart is type iterator for vector<float>

		v0 = glm::vec3(*(triangleStart     ), *(triangleStart + 1u), *(triangleStart +  2u));
		v1 = glm::vec3(*(triangleStart + 4u), *(triangleStart + 5u), *(triangleStart +  6u));
		v2 = glm::vec3(*(triangleStart + 8u), *(triangleStart + 9u), *(triangleStart + 10u));

		computedNormal = MeshFunc::computeNormalizedVertexNormalsForTriangle(v0, v1, v2);

		for (size_t i = 0u; i < 3u; i++) {
			sceneBuffer.push_back(objPos.x + *(triangleStart + (i*4u)));        //x
			sceneBuffer.push_back(objPos.y + *(triangleStart + ((i*4u) + 1u)));  //y
			sceneBuffer.push_back(objPos.z + *(triangleStart + ((i*4u) + 2u)));  //z
			sceneBuffer.push_back(*(triangleStart + ((i*4u) + 3u)));             //w
			uvCoord = genTexCoord();  //Generate the uv Coords on the fly
			sceneBuffer.push_back(uvCoord.s);                               //s
			sceneBuffer.push_back(uvCoord.t);                               //t
			sceneBuffer.push_back(computedNormal.x);
			sceneBuffer.push_back(computedNormal.y);
			sceneBuffer.push_back(computedNormal.z);
		}
	}
}




void AssetLoadingDemo::uploadSceneBufferToGPU() {

	auto vertexCount = computeNumberOfVerticesInSceneBuffer();

	glCreateBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	fprintf(MSGLOG, "\nGPU Buffer Created with ID %u. Uploading data to GPU Buffer!\n", vbo);
	fprintf(MSGLOG, "There are %u vertices total in the scene, or %u floating-point values\n", vertexCount, vertexCount * 9u);

	glBufferData(GL_ARRAY_BUFFER, sceneBuffer.size() * sizeof(sceneBuffer.data()), sceneBuffer.data(), GL_STATIC_DRAW);

}




void AssetLoadingDemo::configureVertexArrayAttributes() {
	glCreateVertexArrays(1, &vao);
	glBindVertexArray(vao);

	///////////////////////////////////////////////////////////////////////
	//              //Specify the vertex attribute layout//              //
	///////////////////////////////////////////////////////////////////////

	//Location 0    Position       4-components 
	glEnableVertexArrayAttrib(vao, 0); //Requires OpenGl 4.5 or newer, allows VAO to be specified as param (even though that is not necessary here)
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, (9u) * sizeof(GLfloat), static_cast<GLvoid*>(0u));

	//Location 1    TexCoords      2-components
	glEnableVertexArrayAttrib(vao, 1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, (9u) * sizeof(GLfloat), (GLvoid*)(4u * sizeof(GLfloat)));

	//Location 2    Normals        3-components
	glEnableVertexArrayAttrib(vao, 2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, (9u) * sizeof(GLfloat), (GLvoid*)(6u * sizeof(GLfloat)));

}


