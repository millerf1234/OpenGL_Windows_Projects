//File:        AssetLoadingDemo.cpp
//
//
//Programmer:  Forrest Miller
//Date:        November 14, 2018



#include "AssetLoadingDemo.h"

#ifndef ASSET_DEMO_NEEDS_FIXING_AND_REFACTORING  //Remove this once I fix AssetLoadingDemo to actually load assets properly

void AssetLoadingDemo::initialize() {
	error = false;
	frameNumber = 0ull;
	frameUnpaused = 0ull;
	frameOfMostRecentColorRecording = 0ull;
	frameLineTypeLastSwitched = 0ull;
	counter = 0.0f;
	vao = vbo = 0u;

	//Set the starting input primitive type
	currentPrimativeInputType = PIPELINE_PRIMATIVE_INPUT_TYPE::DISCRETE_TRIANGLES;

	xRotation = 90.0f;
	yRotation = 120.0f;
	zRotation = 0.0f;
	zoom = 1.0f;
	mvpMatrixNeedsUpdating = false;

	noiseFunctionToUse = 0;
	noiseResolution = 5;

	//redRotationTheta = 0.0f;
	//greenRotationTheta = 0.0f;
	//blueRotationTheta = 0.0f;

	

	//Set initial background color
	backgroundColor = glm::vec3(0.25f, 0.5f, 0.75f);

	

	glEnable(GL_DEPTH_TEST);
	//glDepthMask(GL_FALSE);
	//glDepthFunc(GL_LEQUAL);
	//glEnable(GL_PROGRAM_POINT_SIZE); //Doesn't matter as of right now
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
}


void AssetLoadingDemo::loadShaders() {
	fprintf(MSGLOG, "\nInitializing Shaders!\n");

	lightSourceShader = std::make_unique<ShaderProgram>();

	lightSourceShader->attachVert("Lightsource.vert");


	/*std::unique_ptr<ShaderInterface::GeometryShader> noiseShader = std::make_unique<ShaderInterface::GeometryShader>("ShaderNoiseFunctions.glsl");
	noiseShader->makeSecondary();*/

	//lightSourceShader->attachSecondaryGeom(noiseShader.get());

	//lightSourceShader->attachGeom("Lightsource.geom");

	std::unique_ptr<ShaderInterface::FragmentShader> noiseShader = std::make_unique<ShaderInterface::FragmentShader>("ShaderNoiseFunctions.frag");
	noiseShader->makeSecondary();
	
	lightSourceShader->attachSecondaryFrag(noiseShader.get());

	lightSourceShader->attachFrag("Lightsource.frag");
	
	lightSourceShader->link();
	if (lightSourceShader->checkIfLinked()) {
		fprintf(MSGLOG, "Program Successfully linked!\n");
	}
	else {
		fprintf(ERRLOG, "Shader Program was not successfully linked!\n");
		fprintf(MSGLOG, "\t[Press 'ENTER' to attempt to continue program execution]\n");
		std::cin.get(); //Hold the window open if there was an error
	}



	/*
	//sceneShader = std::make_unique<ShaderProgram>();

	//Create and attach helper vertex shaders
	std::unique_ptr<ShaderInterface::VertexShader> perspectiveProj = std::make_unique<ShaderInterface::VertexShader>("PerspectiveProjection.vert");
	if (!perspectiveProj)
		return;
	perspectiveProj->makeSecondary();
	sceneShader->attachSecondaryVert(perspectiveProj.get());

	std::unique_ptr<ShaderInterface::VertexShader> vertHelper = std::make_unique<ShaderInterface::VertexShader>("VertMath.vert");
	if (!vertHelper)
		return;
	vertHelper->makeSecondary();
	sceneShader->attachSecondaryVert(vertHelper.get());

	//Attach the primary vertex shader
	fprintf(MSGLOG, "\nAttaching main vertex shader!\n");
	//sceneShader->attachVert("TeapotExplosion.vert");
	std::unique_ptr<ShaderInterface::VertexShader> mainVert = std::make_unique<ShaderInterface::VertexShader>("AssetLoadingDemo.vert");
	if (!mainVert)
		return;
	sceneShader->attachVert(mainVert.get());

	sceneShader->attachFrag("AssetLoadingDemo.frag");
	sceneShader->link();
	if (sceneShader->checkIfLinked()) {
		fprintf(MSGLOG, "Program Successfully linked!\n");
	}
	else {
		fprintf(ERRLOG, "Shader Program was not successfully linked!\n");
		fprintf(MSGLOG, "\t[Press 'ENTER' to attempt to continue program execution]\n");
		std::cin.get(); //Hold the window open if there was an error
	}

	////There is just 1 pipeline here to build. Here is the pipeline
	//fprintf(MSGLOG, "Assembling the main shader for the scene...\n");
	////---------------------------
	////    (VERTEX STAGE)
	//// Attach a helper shader with just some useful functions
	//fprintf(MSGLOG, "\nAttaching secondary helper vertex shader!\n");
	//std::unique_ptr<ShaderInterface::VertexShader> vertHelper = std::make_unique<ShaderInterface::VertexShader>("VertMath.vert");
	//if (!vertHelper)
	//	return;
	//vertHelper->makeSecondary();
	//sceneShader->attachSecondaryVert(vertHelper.get());
	////Attach the primary vertex shader
	//fprintf(MSGLOG, "\nAttaching main vertex shader!\n");
	////sceneShader->attachVert("TeapotExplosion.vert");
	//std::unique_ptr<ShaderInterface::VertexShader> mainVert = std::make_unique<ShaderInterface::VertexShader>("TeapotExplosion.vert");
	//if (!mainVert)
	//	return;
	//sceneShader->attachVert(mainVert.get());

	////---------------------------

	////---------------------------
	////     (Geometry Stage) 
	//// Attach the primary geometry shader to the pipeline. (This is where the explosion happens)
	//fprintf(MSGLOG, "\nAttaching geometry shader!\n");
	//sceneShader->attachGeom("TeapotExplosion.geom");
	////--------------------------


	////--------------------------
	////     (Fragment Stage)
	//// Attach the primary Fragment Shader to the pipeline
	//fprintf(MSGLOG, "\nAttaching fragment shader!\n");
	//sceneShader->attachFrag("TeapotExplosion.frag");
	//fprintf(MSGLOG, "\nAttempting to link program!\n");
	////---------------------------

	////    AT THIS POINT, ALL OF THE PIPELINE STAGES HAVE BEEN ATTACHED, SO LINK THE PIPELINE AND CHECK FOR ERRORS
	//sceneShader->link();
	//if (sceneShader->checkIfLinked()) {
	//	fprintf(MSGLOG, "Program Successfully linked!\n");
	//}
	//else {
	//	fprintf(ERRLOG, "Shader Program was not successfully linked!\n");
	//	fprintf(MSGLOG, "\t[Press 'ENTER' to attempt to continue program execution]\n");
	//	std::cin.get(); //Hold the window open if there was an error
	//}
	*/
}

GLsizei AssetLoadingDemo::computeSceneObjectPtrsTotalIndices() const {
	return 0;
	/*
	GLsizei indices = 0u;
	GLsizei vertexSize;
	if (sceneObjectPtrs.data()[0]->hasTexCoords()) {
		if (sceneObjectPtrs.data()[0]->hasNormals()) {
			vertexSize = 4u + 2u + 3u;
		}
		else {
			vertexSize = 4u + 2u;
		}
	}
	else if (sceneObjectPtrs.data()[0]->hasNormals()) {
		vertexSize = 4u + 3u;
		//GOing to insert an extra 2 padding here to go with my hack to fix non-texture-coord models to run with the code that expects texture coords
		//vertexSize += 2u;
	}
	else {
		vertexSize = 4u;
	}
	
	for (auto sceneObjectIter = sceneObjectPtrs.begin(); sceneObjectIter != sceneObjectPtrs.end(); sceneObjectIter++) {
		indices += ((*sceneObjectIter)->mVertices_.size() / vertexSize);
	}
	return indices;
	*/
}

void AssetLoadingDemo::loadModels() {
	//sceneObjectPtrs.emplace_back(std::make_unique<QuickObj>("obj/Pokemon.obj")); //Doesn't load? Spend time in debugger on this one
	//sceneObjectPtrs.emplace_back(std::make_unique<QuickObj>("obj/spaceship.obj"));
	//sceneObjectPtrs.emplace_back(std::make_unique<QuickObj>("obj/SubdivisionCube.obj"));
	//sceneObjectPtrs.emplace_back(std::make_unique<QuickObj>("obj/BlockThing_Triangulated01.obj"));
	//sceneObjectPtrs.emplace_back(std::make_unique<QuickObj>("obj/BeveledCube.obj"));
	//sceneObjectPtrs.emplace_back(std::make_unique<QuickObj>("obj/ExperimentalEngine.obj"));
	//sceneObjectPtrs.emplace_back(std::make_unique<QuickObj>("obj/ExperimentalEngineUV_ToGOWithAlbedo.obj", 3.0f));
	//sceneObjectPtrs.emplace_back(std::make_unique<QuickObj>("obj/BlockShipSample_01.obj", 6.5f));


	////Make a vertex attribute set to handle organizing the data for the graphics context
	//vertexAttributes = std::make_unique<GenericVertexAttributeSet>(3);

	//if (!vertexAttributes)
	//	return;




	//Going to wing something together...
	std::array<float, 3> pos = { 0.0f, 0.25f, 0.0f };
	std::array<float, 3> col = { 0.55f, 0.75f, 11.0f };
	std::unique_ptr<LightEmitterSource> test = std::make_unique<LightEmitterSource>(pos, col, 15u, 12u, 0.65f, 1.05f);

	std::vector<float> lightData = test->getData();
	std::vector<float> combinedSceneObjects;
	combinedSceneObjects.swap(lightData);


	std::vector<GLuint> elementArray = test->getElemOrder();
	eboSize = elementArray.size();
	glCreateBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, elementArray.size() * sizeof(GLuint), &elementArray.data()[0], GL_STATIC_DRAW);

	//std::vector<float> combinedSceneObjects = { 0.0f, 0.5000f, 0.000f, 0.9f, 0.55f, 0.525f };// , -0.60025f, -0.3025f, 0.025f, 0.1f, 0.975f, 0.9f

	
	
	
	// 0.0f, 0.0f, 0.0f
	//for (auto sceneObjIter = sceneObjectPtrs.begin(); sceneObjIter != sceneObjectPtrs.end(); sceneObjIter++) {
	//	combinedSceneObjects.insert(combinedSceneObjects.end(), (*sceneObjIter)->mVertices_.begin(), (*sceneObjIter)->mVertices_.end());
	//	int posCounter = 0;
	//	for (auto iter = combinedSceneObjects.begin(); iter != combinedSceneObjects.end(); iter++) {
	//		if (posCounter == 0) {
	//			(*iter) += 0.65f;
	//		}
	//		posCounter = (posCounter + 1) % 9;
	//	}
	//}
	
	/////////////////////////////////////////////////////////////////////////////////
	//DO it manually   (FAILED ATTEMPT 1)
	/////////////////////////////////////////////////////////////////////////////////
	//See pages 115-116 of OpenGL Super Bible   (THIS IS WRONG !!! IT NEEDS A glGEN*() call)
	//
	//static GLuint staticVAO = 0u;
	//static GLuint staticBuffer = 0u; 

	//glCreateVertexArrays(1, &staticVAO);
	//glCreateBuffers(1, &staticBuffer);
	//glNamedBufferStorage(staticBuffer, sizeof(combinedSceneObjects.data()), combinedSceneObjects.data(), 0);

	////Set up positions
	//glVertexArrayAttribBinding(staticVAO, 0, staticBuffer);
	//glVertexArrayAttribFormat(staticVAO, 0, 4, GL_FLOAT, GL_FALSE, 7);
	//glEnableVertexAttribArray(0u);

	//glVertexArrayAttribBinding(staticVAO, 1, staticBuffer);
	//glVertexArrayAttribFormat(staticVAO, 1, 3, GL_FLOAT, GL_FALSE, 7);
	//glEnableVertexAttribArray(0u);
	//////////////////////////////////////////////////////////////////////////////////
	
	//ATTEMPT 2
	glCreateVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glCreateBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	
	fprintf(MSGLOG, "\nReality Check: sizeof(combinedSceneObjects.data() is: %u\n", sizeof(combinedSceneObjects.data()));
	fprintf(MSGLOG, "ANd combinedSceneObjects.size() * sizeof(combinedSceneObjects.data()) is: %u\n", combinedSceneObjects.size() * sizeof(combinedSceneObjects.data()));

	glBufferData(GL_ARRAY_BUFFER, combinedSceneObjects.size() * sizeof(combinedSceneObjects.data()), combinedSceneObjects.data(), GL_STATIC_DRAW);

	glEnableVertexArrayAttrib(vao, 0); //Requires OpenGl 4.5 or newer, allows VAO to be specified as param
	//glEnableVertexAttribArray(0);  //Since OpenGl 2.0, will use whatever VAO is bound globally to context
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, (6u) * sizeof(GLfloat), (GLvoid*)0u);


	glEnableVertexArrayAttrib(vao, 1);
	//glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, (6u) * sizeof(GLfloat), (GLvoid*)(3u * sizeof(GLfloat)));
	
	



	/*
	//Positions (expects "layout (location=0)" in shader)
	glEnableVertexArrayAttrib(vao, 0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, (4u + 2u + 3u) * sizeof(GLfloat), (GLvoid*)0u);
	//glEnableVertexAttribArray(0);

	
	//Texture Coords (expects "layout (location = 1)" in shader)
	glEnableVertexArrayAttrib(vao, 1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, (4u + 2u + 3u) * sizeof(GLfloat), (GLvoid*)(5u*sizeof(GLfloat)));
	//glEnableVertexAttribArray(1);

	//Normals (expects "layout (location = 2)" in shader)
	glEnableVertexArrayAttrib(vao, 2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, (4u + 2u + 3u) * sizeof(GLfloat), (GLvoid*)(7u * sizeof(GLfloat)));
	//glEnableVertexAttribArray(2);
	
	*/


	//



	//vertexAttributes->sendDataToVertexBuffer(0, combinedSceneObjects, 4, 7, static_cast<GLvoid*>(0));
	//vertexAttributes->sendDataToVertexBuffer(2, combinedSceneObjects, 3, 7, (GLvoid*)(static_cast<GLintptr>((4u * sizeof(float)))));
	

	//fprintf(MSGLOG, "\n%u Objects have been successfully loaded to Video Memory!\n", sceneObjectPtrs.size());


	//fprintf(MSGLOG, "\nLoading Teapot Vertices\n");
	//std::vector<GLfloat> teapotVertices;
	//for (int i = 0; i < teapot_count; i++) {
	//	teapotVertices.push_back(static_cast<GLfloat>(teapot[i]));
	//}
	//fprintf(MSGLOG, "\nTeapot vertices loaded to application.\n"
	//	"Application will now load teapot vertices to video memory on GPU.\n");


	////Make a vertex attribute set to handle organizing the data for the graphics context
	//vertexAttributes = std::make_unique<GenericVertexAttributeSet>(1);

	//if (!vertexAttributes)
	//	return;
	//vertexAttributes->sendDataToVertexBuffer(0, teapotVertices, 3, 0);

	//fprintf(MSGLOG, "\nTeapot Has Been Successfully Loaded To Video Memory!\n");
}

void AssetLoadingDemo::renderLoop() {
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
		rotate();
		changeNoiseType();
		

		updateFrameClearColor();

		updateUniforms();

		drawVerts();

		counter += 0.0125f;
		//counter += 0.00000002f*0.000125f;

		glfwSwapBuffers(window);

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

bool AssetLoadingDemo::checkIfShouldRecordColor() const {
	if ((frameNumber >= (frameOfMostRecentColorRecording +
		DELAY_BETWEEN_SCREEN_COLOR_RECORDINGS_IN_RENDER_PROJECTS))
		&& (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)) {
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

void AssetLoadingDemo::recordColorToLog() {
	frameOfMostRecentColorRecording = frameNumber;
	static constexpr const int colorDigits = 6; //Digits to print out for each color

						 //Syntax Note: Within '%*f', the '*' means that the width will be provided as an additional parameter
	fprintf(MSGLOG, "\nThe background color of frame %llu is:\n\tRed: %*f,\tGreen: %*f,\tBlue: %*f\n",
		frameNumber, colorDigits, backgroundColor.r, colorDigits, backgroundColor.g, colorDigits, backgroundColor.b);
}

void AssetLoadingDemo::reset() {
	fprintf(MSGLOG, "\nReseting Demo...\n");
	counter = 0.0f; //Reset time to 0
	zRotation = 0.0f; //Reset rotation
	xRotation = 0.0f;
	yRotation = 0.0f;
	backgroundColor = glm::vec3(0.0f, 0.5f, 0.75f);
	frameNumber = 0ull;
	frameUnpaused = 0ull;
	frameOfMostRecentColorRecording = 0ull;
	frameLineTypeLastSwitched = 0ull;
	zoom = 1.0f;
}

void AssetLoadingDemo::changeNoiseType() {

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		noiseResolution++;
	}
	else if ((glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)) {
		noiseResolution--;
		if (noiseResolution == 0)
			noiseResolution = 1;
	}
	
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

void AssetLoadingDemo::changePrimitiveType() {
	/*
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
	*/
}

//void AssetLoadingDemo::modifyColorThreshhold() {
//	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
//		if (colorChangeThreshold < 1.0f)
//			colorChangeThreshold += 0.005f;
//	}
//	else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
//		if (colorChangeThreshold > 0.0f)
//			colorChangeThreshold -= 0.005f;
//	}
//}
//
void AssetLoadingDemo::rotate() {

	if ((glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) ||
		(glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)) {

		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
			yRotation += 0.105f;
		}
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
			yRotation -= 0.105f;
		}

		if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
			zoom += 0.095f;
		}
		if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
			zoom -= 0.095f;
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
			zRotation += 0.51f;
			//greenRotationTheta += 0.025f;
		}
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
			zRotation -= 0.51f;
			//greenRotationTheta -= 0.025f;
		}
	}
	else {


	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		xRotation += 0.1f;
		//blueRotationTheta += 0.025f;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		xRotation -= 0.1f;
		//blueRotationTheta -= 0.025f;
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		zRotation += 0.1f;
		//greenRotationTheta += 0.025f;
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		zRotation -= 0.1f;
		//greenRotationTheta -= 0.025f;
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
	
	/*
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

	//*red = 0.25f * (*red);
	//*blue *= 0.25f;
	//*green *= 0.25f;

	glClearColor(*red, *green, *blue, 1.0f);
	*/
}

void AssetLoadingDemo::updateUniforms() {

	lightSourceShader->use();

	//Vertex Shader Uniforms:
	lightSourceShader->uniforms->updateUniform1f("distanceToCamera", 0.5f + 0.45f*sin(counter));
	//glm::mat4 identity(1.0);
	//counter *= 25.0f;
	//glm::mat4 transform(cos(counter), sin(counter), 0.0f, 0.0f, -sin(counter), cos(counter), 0.0f, 0.0f*sin(counter), 0.0f, 0.0f, 0.0f, 0.0f, 0.0, 0.0f, 0.0f, 1.0f);
	
	
	//counter = counter / 24.0f;

	glm::mat4 proj = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f ), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));


	lightSourceShader->uniforms->updateUniformMat4x4("projection", &proj);  //;(const float*)glm::value_ptr(transform));

	//Geometry Shader Uniforms:
	lightSourceShader->uniforms->updateUniform1f("time", 3.0f*counter);
	lightSourceShader->uniforms->updateUniform1f("zoom", 1.0f);
	//lightSourceShader->uniforms->updateUniform1i("noiseFunctionToUse", 0);
	//lightSourceShader->uniforms->updateUniform1i("noiseResolution", 500);
	//lightSourceShader->uniforms->updateUniform1f("instabilityFactor", 2.0f);

	//Fragment Shader Uniforms:
	//lightSourceShader->uniforms->updateUniform1i("", );
	lightSourceShader->uniforms->updateUniform1i("noiseFunctionToUse", noiseFunctionToUse);
	lightSourceShader->uniforms->updateUniform1i("noiseResolution", noiseResolution);



	
	

	//sceneShader->use();
	//Update uniform locations
	//sceneShader->uniforms->updateUniform1f("zoom", zoom);//1.7f + counter);
	//sceneShader->uniforms->updateUniform1f("time", 0.725f*counter);

	////Uniforms for the geometry shader effect
	//sceneShader->uniforms->updateUniform1i("level", 1);                    //tweak this value as needed
	//sceneShader->uniforms->updateUniform1f("gravity", -0.91f /*-29.81f*/);  //tweak this value as needed
	//sceneShader->uniforms->updateUniform1f("velocityScale", 1.0f);        //tweak this value as needed

	//sceneShader->uniforms->updateUniform1f("xRotation", xRotation);
	//sceneShader->uniforms->updateUniform1f("yRotation", yRotation);
	//sceneShader->uniforms->updateUniform1f("zRotation", zRotation);
}

void AssetLoadingDemo::drawVerts() {

	//if (sceneShader)
		//sceneShader->use();
	//if (vertexAttributes)
	//	vertexAttributes->use();
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	lightSourceShader->use();
	//glDrawArrays(GL_POINTS, 0, 1);

	glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	glDrawElements(GL_TRIANGLES, eboSize, GL_UNSIGNED_INT, 0);
	return;

	if (currentPrimativeInputType == PIPELINE_PRIMATIVE_INPUT_TYPE::DISCRETE_TRIANGLES) {
		//fprintf(MSGLOG, "\nTotalIndices are: %u\n", computeSceneObjectPtrsTotalIndices());
		glDrawArrays(GL_TRIANGLES, 0, computeSceneObjectPtrsTotalIndices());
	}

	else if (currentPrimativeInputType == PIPELINE_PRIMATIVE_INPUT_TYPE::TRIANGLE_STRIP) {
		//glDrawArrays(GL_TRIANGLE_STRIP, 0, teapot_count / 3);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, computeSceneObjectPtrsTotalIndices());
	}
	else if (currentPrimativeInputType == PIPELINE_PRIMATIVE_INPUT_TYPE::TRIANGLE_FAN) {
		//glDrawArrays(GL_TRIANGLE_FAN, 0, teapot_count / 3);
		glDrawArrays(GL_TRIANGLE_FAN, 0, computeSceneObjectPtrsTotalIndices());
	}
	else if (currentPrimativeInputType == PIPELINE_PRIMATIVE_INPUT_TYPE::LINE) {
		glDrawArrays(GL_LINES, 0, computeSceneObjectPtrsTotalIndices());
	}
	else if (currentPrimativeInputType == PIPELINE_PRIMATIVE_INPUT_TYPE::LINE_STRIP) {
		glDrawArrays(GL_LINE_STRIP, 0, computeSceneObjectPtrsTotalIndices());
	}
	/*
	if (sceneShaderLine)
	sceneShaderLine->use();
	glDrawArrays(GL_TRIANGLES, 0, teapot_count / 3);
	*/
}


void AssetLoadingDemo::prepareGLContextForNextFrame() {
	glBindVertexArray(0);
	glUseProgram(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}




#endif //ASSET_DEMO_NEEDS_FIXING_AND_REFACTORING
