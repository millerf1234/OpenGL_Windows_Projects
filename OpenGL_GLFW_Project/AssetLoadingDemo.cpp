//File:        AssetLoadingDemo.cpp
//
//Description:          What to say. This is the furthest along project (as of late December 2018)
//                        yet from among the RenderDemo projects. The idea of this project is to be able 
//                        to load '.obj' models and then render them.
//   Main Features:   -Supports instanced rendering. While this feature is initially disabled, it can
//                        be enabled with a single key press. Excellent way to have the demo draw 
//                        many millions of triangles at once without chugging (at least on my computer).
//                    -The main shaders for this project are able to be dynamically updated without
//                        requiring for the project to be closed, re-opened and re-loaded (just save
//                        any changes to the shader to attempt rebuild*).                                   *Note: Only a few of the shaders are currently supported 
//                    -Texturing is not yet supported, but it wouldn't take much                                 and they won't rebuild if they contain errors
//                        preperatory work to add, since uv-coordinates are loaded 
//                        for models that have them available. Models without UV coords 
//                        will have randomly generated texture coordinates, which could 
//                        be interesting if used to sample from one of the availble noise
//                        functions. 
//                     -Models that do not contain normal data will have it generated for them on a 
//                        triangle-by-triangle basis (i.e. all 3 vertices of each triangle are assigned
//                        the same normal). This is (I think) less-than-ideal compared with per-vertex
//                        normals, but it gets the job done just fine.
//                     -The load times can get pretty long for larger models, there is definitly
//                        work needed still for object loading. The current implementation is entirely
//                        single threaded and my algorithm performs several iterations over the data to 
//                        change it from its '.obj' storage (with Positions, Texture Coordinates and 
//                        Normals stored in seperate sections of the file) to interlaced vertices (in the 
//                        9-component ordering of {x,y,z,w,s,t,nx,ny,nz}  [with nx, ny, nz as the normal's
//                        xyz components]). With some rewriting the number of steps (i.e. copies and  
//                        allocations) performed should be reducable. I have also been investigating
//                        potential Task-based solutions for breaking up loading to be performed concurrently.
//                        To speed up the loading of multiple models, wrapping each model's loader object
//                        in a 'std::packaged_task' looks promising... 
//                     
//                  
// Instructions:        To change which model(s) get loaded, find the member function loadModels() and
//                        just follow the syntax of any of the sample models that are available, 
//                        the only difference being to change the filepath to match the model
//                        you want. If unsure of filepath of a model/file, click and drag+drop
//                        the model onto this RenderDemo's mainRenderWindow while it is running, there is
//                        a callback implemented that will print the filepath of any files that
//                        are dropped.
//                      In this project's current state, the only supported 3D model format is '.obj', 
//                        and even then not every feature define in the '.obj' file standard is supported.
//                      A work-around for misbehaving models is to first import their '.obj' into the free modeling
//                        software Blender ( https://www.blender.org/ ) and then use Blender's export feature to
//                        export the model as an '.obj'. Blender's exported '.obj's are typically well
//                        behaved and predictable. Triangulating faces is not necessary.   
//      
//
// Available User Input: 
//                           [Disclaimer]   
//  All of the user input for this RenderDemo is currently hard coded. While 
//  chances are the input bindings will not change, there is a chance the following 
//  controls get remapped to different inputs than documented here. Also chances are this
//  list is not an exhaustive list. 
//
//  +---------------------------------+--------------------------------------------------------------------------------+
//  |            Input                |                              Description of Action                             |
//  +---------------------------------+--------------------------------------------------------------------------------+
//  |                                 |                                                                                |
//  |           Arrow Keys            |           Modifies the rotations angles for Pitch and Head(Yaw)                |
//  |                                 |                                                                                |
//  +---------------------------------+--------------------------------------------------------------------------------+
//  |                                 |                                                                                |
//  |     Arrow Keys + L/R Shift      |          Modifies the rotations Roll and a sped-up Pitch                       |
//  |                                 |                                                                                |
//  +---------------------------------+--------------------------------------------------------------------------------+
//  |                                 |                                                                                |
//  |              Z, W               |                  Changes the value of the uniform 'zoom'                       |
//  |         Z, W  + L-/R-Shift      |                       (same as before, except faster)                          |
//  +---------------------------------+--------------------------------------------------------------------------------+
//  |                                 |                 Modifies input primitive for scene draw calls                  |
//  |     Tilde/'1'/'2'/'3'           |              [Press tilde once for LINES, twice for LINE_STRIP]                |
//  |                                 |     [Press '1' for triangles, '2' for TRIANGLE_STRIP, '3' for TRIANGLE_FAN]    |
//  +---------------------------------+--------------------------------------------------------------------------------+
//  |                                 |                                                                                |
//  |             Space               |                         Pause/Unpause Program Execution                        |
//  |                                 |                                                                                |
//  +---------------------------------+--------------------------------------------------------------------------------+
//  |                                 |                                                                                |
//  |              't'                |                                  Freeze time                                   |
//  |                                 |                                                                                |
//  +---------------------------------+--------------------------------------------------------------------------------+
//  |                                 |                                                                                |
//  |              'r'                |                                  Reset Demo                                    |
//  |                                 | [see member function reset();  should set time to 0 and reset rotations/zoom]  |
//  +---------------------------------+--------------------------------------------------------------------------------+
//  |                                 |                                                                                |
//  |              'i'                |                           Toggle Instanced Rendering**                         |      **Note: Vertex shader must use gl_InstanceID in some way
//  |           '+' and '-'           | (Requires Instanced Rendering Actived) Increment/Decrement rendered instances  |              or else all instances will be drawn in the same place.
//  +---------------------------------+--------------------------------------------------------------------------------+
//  |                                 |                                                                                |
//  |     'w', 'a', 's', 'd'          |                      Translates all models in the scene                        |
//  |                                 |                                                                                |
//  +---------------------------------+--------------------------------------------------------------------------------+
//  |                                 |                                                                                |
//  |             'esc'               |                                   Exit Demo                                    |
//  |                                 |                                                                                |
//  +---------------------------------+--------------------------------------------------------------------------------+
//  |                                 |                                                                                |
//  |              'b'                |                               Toggle Blending                                  |
//  |                                 |                                                                                |
//  +---------------------------------+--------------------------------------------------------------------------------+
//
//
//
//  Additional Notes and General Things to be Aware Of:
//                  -The rotation matrix used in this RenderDemo is built from Euler angles and thus
//                      quite easily runs into the issue of Gimble lock.
//                  -The behavior of the models on the screen is highly variable dependent upon what the
//                      sceneShaders are doing ("AssetLoadingDemo.vert"  and  "AssetLoadingDemo.frag")
//                  -To allow for the use of a single shader program for rendering models that will not 
//                      necessarily have the same vertex information
//
//                  -There was a lot of experiementing that went into 
//                        this RenderDemo, so a lot of the private member functions of this 
//                        class ideally will make their way into future Utility classes.
//                   [What I mean by this is that there is a heck of a lot of implementation code
//                        that really should be burried into seperate classes]
//
//Programmer:          Forrest Miller
//Date Created:        November 14, 2018
//Date Completed:      tbd...


#include "AssetLoadingDemo.h"

//The following 2 global variables can be used to define how models are to be loaded into the scene.
//The first model loaded is translated by the vector:
constexpr const glm::vec3 POSITION_FIRST_OBJECT_IN_SCENE(-0.0f, -0.0f, -0.0f);
//Each object after the first has the following translation applied (Note 
// Z-translation is disabled for your own safety due to Z-Clipping hazards!):
constexpr const glm::vec2 CHANGE_BETWEEN_OBJECTS(3.0f, 3.0f);



//This function is intended to be called only through this class's constructor and 
//is in charge of assigning every member field an initial value
void AssetLoadingDemo::initialize() {
	//Set error flag
	error = false;
	//Set FrameNumber-related variables (Note that these must all be reset in the 'reset()' function as well)
	frameNumber = 0ull;
	frameUnpaused = 0ull;
	frameLineTypeLastSwitched = 0ull;
	frameInstancedDrawingBehaviorLastToggled = 0ull;
	frameInstancedDrawingCountLastModified = 0ull;
	frameTimeFreezeLastToggled = 0ull;
	frameBlendOperationLastToggled = 0ull;

	counter = 0.0f;
	vao = vbo = 0u;

	//Set the starting input primitive type
	currentPrimativeInputType = PIPELINE_PRIMATIVE_INPUT_TYPE::DISCRETE_TRIANGLES;

	//Set the variables regarding instanced drawing
	drawMultipleInstances = false;
	instanceCount = STARTING_INSTANCE_COUNT;
	instanceSpiralPatternPeriod_x = STARTING_INSTANCE_SPIRAL_PATTERN_PERIOD_X;
	instanceSpiralPatternPeriod_y = STARTING_INSTANCE_SPIRAL_PATTERN_PERIOD_Y;

	freezeTimeToggle = false;
	enableBlending = false;

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
	cameraPos = glm::vec3(0.0f, 0.0f, 10.0f); //3.0f * 0.5f / tan(glm::radians(fov / 2.f)));
	lookAtOrgin = glm::vec3(0.0f, 0.0f, -1.0f);
	upDirection = glm::vec3(0.0f, 1.0f, 0.0f);

	xTranslation = 0.0f;
	yTranslation = 0.0f;
	//Compute view matrix
	view = glm::lookAt(cameraPos, lookAtOrgin, upDirection);

	//Set values for Rotation Uniforms
	head = 0.0f;
	pitch = 0.0f;
	roll = 0.0f;
	//Calculate rotation matrix
	rotation = glm::mat4(1.0f);    //Initialize the rotation matrix to 4x4 identity matrix (it will be set to a real rotation matrix later)

	//Keep an extra zoom parameter
	zoom = 1.0f; //Higher number means farther away


	//Set initial background color
	backgroundColor = glm::vec3(0.025f, 0.5f, 0.75f); //Based off some non-exhaustive testing thus far, I
	//                                                 //have reached the conjecture that these initial values matter
	//                                                 //not at all. See the function for the background color update

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
	mainRenderWindow = screenInfo->activeMonitor;
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
	sceneShader->attachVert(shadersRFP + "AssetLoadingDemo.vert"); //Attach Vertex shader to scene
	shaderSources.emplace_back(shadersRFP + "AssetLoadingDemo.vert", true, ShaderInterface::ShaderType::VERTEX);
	sceneShader->attachFrag(shadersRFP + "AssetLoadingDemo.frag"); //Attach Fragment shader to scene
	shaderSources.emplace_back(shadersRFP + "AssetLoadingDemo.frag", true, ShaderInterface::ShaderType::FRAGMENT);

	// [Each shader stage requires its own set of secondary functions]
	//Create and attach a secondary vertex shader containing implementations for some noise functions
	std::unique_ptr<ShaderInterface::VertexShader> vertexNoiseShader =
		std::make_unique<ShaderInterface::VertexShader>(shadersRFP + "ShaderNoiseFunctions.glsl");
	///shaderSources.emplace_back(shadersRFP + "VoronoiNoise.glsl", false, ShaderInterface::ShaderType::VERTEX);

	//Create and attach a secondary fragment shader containing implementations for some noise functions 
	std::unique_ptr<ShaderInterface::FragmentShader> fragmentNoiseShader =
		std::make_unique<ShaderInterface::FragmentShader>(shadersRFP + std::string("ShaderNoiseFunctions.glsl"));
	fragmentNoiseShader->makeSecondary();
	sceneShader->attachSecondaryFrag(fragmentNoiseShader.get()); //the '.get()' function converts the unique_ptr to a raw pointer
	///shaderSources.emplace_back(shadersRFP + "VoronoiNoise.glsl", false, ShaderInterface::ShaderType::FRAGMENT);

	//Now after all the stages to the shader have been created and attached, it is time to link the sceneShader
	sceneShader->link();
	if (sceneShader->checkIfLinked()) {
		fprintf(MSGLOG, "Program Successfully linked!\n");
	}
	else {
		fprintf(ERRLOG, "Shader Program was not successfully linked!\n");
		fprintf(MSGLOG, "\t[Press 'ENTER' to attempt to continue program execution]\n");
		std::cin.get(); //Hold the mainRenderWindow open if there was an error
		markMainRenderWindowAsReadyToClose(); //Mark it for closing once error is acknowledged
		return;
	}

	fprintf(MSGLOG, "\nAll Shaders Successfully Built!\n");
}


GLsizei AssetLoadingDemo::computeNumberOfVerticesInSceneBuffer() const {
	static constexpr const GLsizei vertexSize = 4u + 2u + 3u;  //Since each vertex is {x,y,z,w, s,t, nx,ny,nz}  (i.e. 9 components total)
	return (sceneBuffer.size() / vertexSize);
}


void AssetLoadingDemo::loadModels() {

	fprintf(MSGLOG, "\nAquiring and parsing Model(s) data from file(s)...\n");

	//[RFP == Relative File Path]
	std::string modelsRFP = FILEPATH_TO_MODELS; //Set string to the executable-relative location of Model Files folder

	//Initial Scale values for the objects
	float blockThing_QuadsScale = 2.2f;
	float beveledCubeScale = 2.3f;
	float blockShipScale = 4.5f;
	float subdivisionCubeScale = 4.9f;
	float abstractShapeScale = 2.0f;

	//Load some models
	///sceneObjects.emplace_back(std::make_unique<QuickObj>(modelsRFP + "blockThing_Quads.obj", blockThing_QuadsScale));
	///sceneObjects.emplace_back(std::make_unique<QuickObj>(modelsRFP + "BeveledCube.obj", beveledCubeScale));
	///sceneObjects.emplace_back(std::make_unique<QuickObj>(modelsRFP + "BlockshipSampleExports\\BlockShipSample_01_3DCoatExport01.obj", blockShipScale));
	///sceneObjects.emplace_back(std::make_unique<QuickObj>(modelsRFP + "SubdivisionCube.obj", subdivisionCubeScale)); //Has no text coords
	///sceneObjects.emplace_back(std::make_unique<QuickObj>(modelsRFP + "AbstractShape.obj", abstractShapeScale)); //Only position data

	//This one is abstract enough (with enough distinct triangle faces) to serve as a good example of how the shading calculations work
	///sceneObjects.emplace_back(std::make_unique<QuickObj>(modelsRFP + "AbstractShapeDecimated.obj", abstractShapeScale));

	sceneObjects.emplace_back(std::make_unique<QuickObj>(modelsRFP + "NewOrderTie_Triangulated.obj", 5.0f));

	///sceneObjects.emplace_back(std::make_unique<QuickObj>(modelsRFP + "thing.obj", 2.5f));
	///sceneObjects.emplace_back(std::make_unique<QuickObj>(modelsRFP + "ExperimentalEngine.obj", 4.5f));

	
	///sceneObjects.emplace_back(std::make_unique<QuickObj>(modelsRFP + "Spaceship.obj", 5.01f));
	
	///sceneObjects.emplace_back(std::make_unique<QuickObj>(modelsRFP + "2DTexturedQuadPlane.obj", 2.0f));
	///sceneObjects.emplace_back(std::make_unique<QuickObj>(modelsRFP + "ParentedPrimatives.obj", 3.2f));


	//Crazy Engine (Takes several minutes to load, model is over 1,000,000 triangles)
	///sceneObjects.emplace_back(std::make_unique<QuickObj>(modelsRFP + "CrazyJetEngine.obj", 4.5f));

	size_t loadedModlCount = sceneObjects.size();
	if (loadedModlCount == 0u) {
		fprintf(MSGLOG, "\nNo Models were loaded!\n");
	}
	else if (loadedModlCount == 1u) {
		fprintf(MSGLOG, "\n%u model has been loaded!\n", loadedModlCount);
	}
	else {
		fprintf(MSGLOG, "\n%u models were loaded!\n", loadedModlCount);
	}

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
	while (glfwWindowShouldClose(mainRenderWindow) == GLFW_FALSE) {
		//Check Input
		if (checkToSeeIfShouldCloseWindow()) {
			markMainRenderWindowAsReadyToClose();
			continue; //Skip the rest of this loop iteration to close mainRenderWindow quickly
		}
		if (checkIfShouldPause()) {
			pause();
			continue;
		}
		if (checkIfShouldReset()) {
			if (glfwGetKey(mainRenderWindow, GLFW_KEY_SPACE) == GLFW_PRESS) {
				pause();
				continue;
			}
			reset();
		}
		if (checkIfShouldFreezeTime()) { //'checkIfShouldFreezeTime()' relies on 'toggleFreezeTime()' to 
			toggleTimeFreeze(); //         update member field 'frameTimeFreezeLastToggled' to value of 'frameNumber' 
		}
		if (checkIfShouldToggleBlending()) {
			toggleBlending();
		}
		
		//More Input Checking
		
		changePrimitiveType();
		changeInstancedDrawingBehavior(); //Toggle on/off drawing instances
		rotate();
		translate();
		if (drawMultipleInstances) {  //If drawing instances
			modifyInstancedDrawingSpiralPattern();  //Change the parameters for how the instances are drawn
		}

		//Perform logic 
		if ((frameNumber % FRAMES_TO_WAIT_BEFORE_CHECKING_TO_UPDATE_SHADERS) ==
			(FRAMES_TO_WAIT_BEFORE_CHECKING_TO_UPDATE_SHADERS - 1ull)) {
			if (checkForUpdatedShaders()) {
				buildNewShader();
			}
		}


		//Set up to draw frame
		updateFrameClearColor(); //background color
		updateUniforms(); 

		//Draw frame
		drawVerts();

		if (!freezeTimeToggle) { //if time is not frozen
			counter += 0.0125f; //Increment time 
		}

		glfwSwapBuffers(mainRenderWindow); //Swap the buffer to present image to monitor

		glfwPollEvents();

		 //See RenderDemoBase for description, basically this function should be called once a frame to detect context-reset situations
		if (checkForContextReset()) {
			fprintf(MSGLOG, "\nContext Reset Required!\n");
			fprintf(MSGLOG, "\n\t[Press enter to crash]\n");
			std::cin.get();
			glfwSetWindowShouldClose(mainRenderWindow, GLFW_TRUE); //For now just close the mainRenderWindow
		}
		

		frameNumber++; //Increment the frame counter
		prepareGLContextForNextFrame(); 
	}

}




bool AssetLoadingDemo::checkToSeeIfShouldCloseWindow() const {
	if (glfwGetKey(mainRenderWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		return true;
	}
	return false;
}

bool AssetLoadingDemo::checkIfShouldPause() const {
	if ((frameNumber >= (frameUnpaused + DELAY_LENGTH_OF_PAUSE_CHECKING_AFTER_UNPAUSE))
		&& (glfwGetKey(mainRenderWindow, GLFW_KEY_SPACE) == GLFW_PRESS)) {
		return true;
	}
	return false;
}


bool AssetLoadingDemo::checkIfShouldReset() const {
	if (glfwGetKey(mainRenderWindow, GLFW_KEY_R) == GLFW_PRESS)
		return true;
	return false;
}

bool AssetLoadingDemo::checkIfShouldFreezeTime() const {
	if ((frameNumber - frameTimeFreezeLastToggled) > 15ull) {
		if (glfwGetKey(mainRenderWindow, GLFW_KEY_T) == GLFW_PRESS) {
			return true;
		}
	}
	return false;
}


bool AssetLoadingDemo::checkIfShouldToggleBlending() const {

	if ((frameNumber - frameBlendOperationLastToggled) > 15ull) {
		if (glfwGetKey(mainRenderWindow, GLFW_KEY_B) == GLFW_PRESS) {
			return true;
		}
	}
	return false;
	
}

void AssetLoadingDemo::pause() {
	auto begin = std::chrono::high_resolution_clock::now(); //Time measurement
	auto end = std::chrono::high_resolution_clock::now();
	fprintf(MSGLOG, "PAUSED!\n");
	//Upon first pausing, enter into the following loop for a short period of time before moving on to
	//the full pause loop. This will prevent unpausing from occuring directly after a pause is initiated. 
	while (std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count() < 300000000ll) {
		std::this_thread::sleep_for(std::chrono::nanoseconds(2000000ll));
		end = std::chrono::high_resolution_clock::now();
	}

	//Enter an infinite loop checking for the unpause key (or exit key) to be pressed
	while (true) {
		glfwPollEvents();
		if (glfwGetKey(mainRenderWindow, GLFW_KEY_SPACE) == GLFW_PRESS) {
			frameUnpaused = frameNumber;
			fprintf(MSGLOG, "UNPAUSED!\n");
			return;
		}
		else if (glfwGetKey(mainRenderWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			glfwSetWindowShouldClose(mainRenderWindow, GLFW_TRUE);
			return;
		}
		else { //wait for a little bit before polling again
			std::this_thread::sleep_for(std::chrono::nanoseconds(3333333ll));
		}
	}
}


void AssetLoadingDemo::reset() {
	fprintf(MSGLOG, "\nReseting Demo...\n");
	counter = 0.0f; //Reset time to 0
	head = 0.0f; //Reset rotation
	pitch = 0.0f;
	roll = 0.0f;
	xTranslation = 0.0f;
	yTranslation = 0.0f;
	backgroundColor = glm::vec3(0.15f, 0.5f, 0.75f);
	frameNumber = 0ull;
	frameUnpaused = 0ull;
	frameLineTypeLastSwitched = 0ull;
	frameInstancedDrawingBehaviorLastToggled = 0ull;
	frameInstancedDrawingCountLastModified = 0ull;
	frameTimeFreezeLastToggled = 0ull;
	frameBlendOperationLastToggled = 0ull;
	zoom = 1.0f;
	if (drawMultipleInstances) {
		instanceCount = STARTING_INSTANCE_COUNT;
		instanceSpiralPatternPeriod_x = STARTING_INSTANCE_SPIRAL_PATTERN_PERIOD_X;
		instanceSpiralPatternPeriod_y = STARTING_INSTANCE_SPIRAL_PATTERN_PERIOD_Y;
	}
}

void AssetLoadingDemo::toggleTimeFreeze() {
	//Note that it is vitally important that 'frameTimeFreezeLastToggled' is updated to match the current 'frameNumber'
	frameTimeFreezeLastToggled = frameNumber;
	freezeTimeToggle = !freezeTimeToggle;
	if (freezeTimeToggle) {
		fprintf(MSGLOG, "Time Frozen!\n");
	}
	else {
		fprintf(MSGLOG, "Time Unfrozen!\n");
	}
}

void AssetLoadingDemo::toggleBlending() {
	//Note that it is vitally important that 'frameBlendOperationLastToggled' is updated to match the current 'frameNumber'
	frameBlendOperationLastToggled = frameNumber;

	enableBlending = !enableBlending;
	if (enableBlending) {
		fprintf(MSGLOG, "Blending Enabled!\tBlend Function set to 'ONE_MINUS_SOURCE_ALPHA' \n");
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	else {
		fprintf(MSGLOG, "Blending Disabled!\n");
		glDisable(GL_BLEND);
	}
}

void AssetLoadingDemo::changeInstancedDrawingBehavior() {
	//Only allow toggling to happen every 11 frames
	if ((frameNumber - frameInstancedDrawingBehaviorLastToggled) > 11ull) {
		if (glfwGetKey(mainRenderWindow, GLFW_KEY_I) == GLFW_PRESS) {
			frameInstancedDrawingBehaviorLastToggled = frameNumber; //Mark the current frame as being the one when instanced drawing behavior was last toggled
			drawMultipleInstances = !drawMultipleInstances; //Perform Toggle
			fprintf(MSGLOG, "Instanced Rendering set to: %s",
				(drawMultipleInstances ? ("Enabled\n") : ("Disabled\n")));
		}
	}

	//Only allow the instance count to be modified every 12 frames (1/5th second)
	if ((frameNumber - frameInstancedDrawingCountLastModified) > 12ull) {
		if (drawMultipleInstances) {
			if (glfwGetKey(mainRenderWindow, GLFW_KEY_EQUAL) == GLFW_PRESS) {
				frameInstancedDrawingBehaviorLastToggled = frameNumber; 
				instanceCount++;
				fprintf(MSGLOG, "Rendered Instances increased to: %u\n", instanceCount);
			}
			else if (glfwGetKey(mainRenderWindow, GLFW_KEY_MINUS) == GLFW_PRESS) {
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
	GLfloat xChangeRate = 0.0015f;
	GLfloat yChangeRate = 0.0015f;
	GLfloat leftShiftFactor = 5.0f; //Holding down left shift will cause value to change leftShiftFactor times faster
	GLfloat rightShiftFactor = 25.0f; //Holding down right shift will cause values to change rightShiftFactor times faster

	//Check for updates to X period
	if (glfwGetKey(mainRenderWindow, GLFW_KEY_LEFT_BRACKET) == GLFW_PRESS) {
		if (glfwGetKey(mainRenderWindow, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) {
			instanceSpiralPatternPeriod_x -= xChangeRate * rightShiftFactor;
		}
		else if (glfwGetKey(mainRenderWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
			instanceSpiralPatternPeriod_x -= xChangeRate * leftShiftFactor;
		}
		else {
			instanceSpiralPatternPeriod_x -= xChangeRate;
		}
	}
	if (glfwGetKey(mainRenderWindow, GLFW_KEY_RIGHT_BRACKET) == GLFW_PRESS) {
		if (glfwGetKey(mainRenderWindow, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) {
			instanceSpiralPatternPeriod_x += xChangeRate * rightShiftFactor;
		}
		else if (glfwGetKey(mainRenderWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
			instanceSpiralPatternPeriod_x += xChangeRate * leftShiftFactor;
		}
		else {
			instanceSpiralPatternPeriod_x += xChangeRate;
		}
	}


	//Check for updates to Y period
	if (glfwGetKey(mainRenderWindow, GLFW_KEY_SEMICOLON) == GLFW_PRESS) {
		if (glfwGetKey(mainRenderWindow, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) {
			instanceSpiralPatternPeriod_y -= yChangeRate * rightShiftFactor;
		}
		else if (glfwGetKey(mainRenderWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
			instanceSpiralPatternPeriod_y -= yChangeRate * leftShiftFactor;
		}
		else {
			instanceSpiralPatternPeriod_y -= yChangeRate;
		}
	}
	if (glfwGetKey(mainRenderWindow, GLFW_KEY_APOSTROPHE) == GLFW_PRESS) {
		if (glfwGetKey(mainRenderWindow, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) {
			instanceSpiralPatternPeriod_y += yChangeRate * rightShiftFactor;
		}
		else if (glfwGetKey(mainRenderWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
			instanceSpiralPatternPeriod_y += yChangeRate * leftShiftFactor;
		}
		else {
			instanceSpiralPatternPeriod_y += yChangeRate;
		}
	}
}

void AssetLoadingDemo::changePrimitiveType() {
	
	if (glfwGetKey(mainRenderWindow, GLFW_KEY_1) == GLFW_PRESS)
		currentPrimativeInputType = PIPELINE_PRIMATIVE_INPUT_TYPE::DISCRETE_TRIANGLES;

	else if (glfwGetKey(mainRenderWindow, GLFW_KEY_2) == GLFW_PRESS)
		currentPrimativeInputType = PIPELINE_PRIMATIVE_INPUT_TYPE::TRIANGLE_STRIP;

	else if (glfwGetKey(mainRenderWindow, GLFW_KEY_3) == GLFW_PRESS)
		currentPrimativeInputType = PIPELINE_PRIMATIVE_INPUT_TYPE::TRIANGLE_FAN;

	if (glfwGetKey(mainRenderWindow, GLFW_KEY_GRAVE_ACCENT) == GLFW_PRESS) {
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

	if ((glfwGetKey(mainRenderWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) ||
		(glfwGetKey(mainRenderWindow, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)) {

		if (glfwGetKey(mainRenderWindow, GLFW_KEY_UP) == GLFW_PRESS) {
			pitch += 0.05f;
		}
		if (glfwGetKey(mainRenderWindow, GLFW_KEY_DOWN) == GLFW_PRESS) {
			pitch -= 0.05f;
		}

		if (glfwGetKey(mainRenderWindow, GLFW_KEY_Z) == GLFW_PRESS) {
			zoom += 0.095f;
		}
		if (glfwGetKey(mainRenderWindow, GLFW_KEY_X) == GLFW_PRESS) {
			zoom -= 0.095f;
		}
		if (glfwGetKey(mainRenderWindow, GLFW_KEY_LEFT) == GLFW_PRESS) {
			roll += 0.3f;
		}
		if (glfwGetKey(mainRenderWindow, GLFW_KEY_RIGHT) == GLFW_PRESS) {
			roll -= 0.3f;
		}
	}
	else {
		if (glfwGetKey(mainRenderWindow, GLFW_KEY_UP) == GLFW_PRESS) {
			head += 0.05f;
		}
		if (glfwGetKey(mainRenderWindow, GLFW_KEY_DOWN) == GLFW_PRESS) {
			head -= 0.05f;
		}

		if (glfwGetKey(mainRenderWindow, GLFW_KEY_LEFT) == GLFW_PRESS) {
			roll += 0.05f;
		}
		if (glfwGetKey(mainRenderWindow, GLFW_KEY_RIGHT) == GLFW_PRESS) {
			roll -= 0.05f;
		}


		//Sneak scaling in here as well..
		if (glfwGetKey(mainRenderWindow, GLFW_KEY_Z) == GLFW_PRESS) {
			zoom += 0.025f;
		}
		if (glfwGetKey(mainRenderWindow, GLFW_KEY_X) == GLFW_PRESS) {
			zoom -= 0.025f;
		}
	}
}

void AssetLoadingDemo::translate() {
	float turbo = 1.0f;
	float xSpeed = 0.1f;
	float ySpeed = 0.1f;
	if (glfwGetKey(mainRenderWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		turbo = 5.0f;
	}
	if (glfwGetKey(mainRenderWindow, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) {
		turbo *= 4.0f;
	}

	//UP/Down
	if (glfwGetKey(mainRenderWindow, GLFW_KEY_W) == GLFW_PRESS) {
		yTranslation += turbo * ySpeed;
	}
	if (glfwGetKey(mainRenderWindow, GLFW_KEY_S) == GLFW_PRESS) {
		yTranslation -= turbo * ySpeed;
	}


	//Left/Right
	if (glfwGetKey(mainRenderWindow, GLFW_KEY_A) == GLFW_PRESS) {
		xTranslation += turbo * xSpeed;
	}
	if (glfwGetKey(mainRenderWindow, GLFW_KEY_D) == GLFW_PRESS) {
		xTranslation -= turbo * xSpeed;
	}

}

bool AssetLoadingDemo::checkForUpdatedShaders() {
	for (auto iter = shaderSources.begin(); iter != shaderSources.end(); iter++) {
		if (iter->file.hasUpdatedFileAvailable()) {
			fprintf(MSGLOG, "\nDetected that shader %s has been updated. Rebuilding Shaders...\n", iter->file.filepath().c_str());
			return true;
		}
	}
	return false;
}

void AssetLoadingDemo::buildNewShader() {
	std::string shadersRFP = FILEPATH_TO_SHADERS;
	backupSceneShader = nullptr;
	backupSceneShader = std::make_unique<ShaderProgram>();
	for (auto shaderIterator = shaderSources.begin(); shaderIterator != shaderSources.end(); shaderIterator++) {
		
		switch (shaderIterator->type) {
		case (ShaderInterface::ShaderType::VERTEX):
			if (shaderIterator->primary)
				backupSceneShader->attachVert(shaderIterator->file.filepath().c_str());
			else {
				std::unique_ptr<ShaderInterface::VertexShader> vertexNoiseShader =
					std::make_unique<ShaderInterface::VertexShader>(shadersRFP + "ShaderNoiseFunctions.glsl");
				vertexNoiseShader->makeSecondary();
				backupSceneShader->attachSecondaryVert(vertexNoiseShader.get());
			}
			break;

		case (ShaderInterface::ShaderType::GEOMETRY):
			fprintf(MSGLOG, "\nDetected that a geometry shader was updated!\n"
				"Unfortuantly that type of shader is not yet supported for dynamic updates!\n");
			break;

		case (ShaderInterface::ShaderType::TESSELATION_CONTROL):
			fprintf(MSGLOG, "\nDetected that a Tesselation Control shader was updated!\n"
				"Unfortuantly that type of shader is not yet supported for dynamic updates!\n");
			break;

		case (ShaderInterface::ShaderType::TESSELATION_EVALUATION):
			fprintf(MSGLOG, "\nDetected that a Tesselation Evaluation shader was updated!\n"
				"Unfortuantly that type of shader is not yet supported for dynamic updates!\n");
			break;

		case (ShaderInterface::ShaderType::FRAGMENT):
			if (shaderIterator->primary)
				backupSceneShader->attachFrag(shaderIterator->file.filepath().c_str());
			else {
				std::unique_ptr<ShaderInterface::FragmentShader> fragmentNoiseShader =
					std::make_unique<ShaderInterface::FragmentShader>(shadersRFP + "ShaderNoiseFunctions.glsl");
				fragmentNoiseShader->makeSecondary();
				backupSceneShader->attachSecondaryFrag(fragmentNoiseShader.get());
			}
			break;

		case (ShaderInterface::ShaderType::COMPUTE):
			fprintf(MSGLOG, "\nDetected that a Compute shader was updated!\n"
				"Unfortuantly that type of shader is not yet supported for dynamic updates!\n");
			break;

		default:
			fprintf(ERRLOG, "\nERROR!!!!!!!!!!!!!!!!  What the heck type of shader are you updating?!?!\n");
			return;
		}
	}
		//Now after all the stages to the shader have been created and attached, it is time to link the sceneShader
		backupSceneShader->link();
		if (backupSceneShader->checkIfLinked()) {
			fprintf(MSGLOG, "New Program Successfully linked!\n");
			sceneShader.release();
			sceneShader = std::move(backupSceneShader);
		}
		else {
			fprintf(ERRLOG, "New Shader Program was not successfully linked!\n");
			//fprintf(MSGLOG, "\t[Press 'ENTER' to attempt to continue program execution]\n"); //
			//std::cin.get(); //Hold the mainRenderWindow open if there was an error
		}
	
}



void AssetLoadingDemo::updateFrameClearColor() {
	//if (false) {
	//	glClearColor(0.32f, 0.4f, 0.35f, 1.0f);
	//}
	//else if (true) {
		glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, 1.0);
		backgroundColor.x = abs(sin(counter + backgroundColor.x));
		backgroundColor.y = abs(sin(counter + backgroundColor.y + PI / 3.0f));
		backgroundColor.z = abs(sin(counter + backgroundColor.y + PI / 2.0f));

		
		backgroundColor /= backgroundColor.length(); //normalize backgroundColor
		//backgroundColor = glm::normalize(backgroundColor); //Keep background color normalized

		/*
		//Experiment:
		glm::vec3 divideCalculation = backgroundColor;
		fprintf(MSGLOG, "Background is:   Red=%f,  Green=%f,   Blue=%f\n", divideCalculation.r, divideCalculation.b, divideCalculation.g);
		
		//A second experiment:
		//glm::vec3 bckgrndColorByGLMNormalization = glm::normalize(backgroundColor);
		//fprintf(MSGLOG, "\nExperiment:  background color is (%f, %f, %f) and glm::normalize(background) is:  (%f, %f, %f)\n"
		//	"The difference between the two is thus: (%f, %f, %f) \n", backgroundColor.r, backgroundColor.g, backgroundColor.b, bckgrndColorByGLMNormalization.r, bckgrndColorByGLMNormalization.g, bckgrndColorByGLMNormalization.b, backgroundColor.r - bckgrndColorByGLMNormalization.r, backgroundColor.g - bckgrndColorByGLMNormalization.g, backgroundColor.b - bckgrndColorByGLMNormalization.b);
		//fprintf(MSGLOG, "For reference, length(background) is: %f\n", backgroundColor.length());
	    */

	//}
}




void AssetLoadingDemo::updateUniforms() {
	if (!sceneShader)
		return;

	sceneShader->use(); 

	//glm::mat4 proj = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	//sceneShader->uniforms->updateUniformMat4x4("projection", &proj);  //;(const float*)glm::value_ptr(transform));

	sceneShader->uniforms.updateUniform1f("time", counter);
	sceneShader->uniforms.updateUniform1f("zoom", zoom);

	rotation = MathFunc::computeRotationMatrix4x4(head, pitch, roll);
	sceneShader->uniforms.updateUniformMat4x4("rotation", &rotation);

	glm::mat4 MVP; //Model-View-Projection matrix 
	MVP = perspective * (view * (rotation));
	glm::mat4 userTranslation = glm::mat4(1.0f, 0.0f, 0.0f, 0.0f,             //Translation from user input
			                              0.0f, 1.0f, 0.0f, 0.0f,
			                              0.0f, 0.0f, 1.0f, 0.0f,
			                              xTranslation, yTranslation, 0.0f, 1.0f);
	MVP *= userTranslation;

	
	sceneShader->uniforms.updateUniformMat4x4("MVP", &MVP);
	

	//if (drawMultipleInstances) {
		sceneShader->uniforms.updateUniform1f("instanceSpiralPatternPeriod_x", instanceSpiralPatternPeriod_x);
		sceneShader->uniforms.updateUniform1f("instanceSpiralPatternPeriod_y", instanceSpiralPatternPeriod_y);
	//}

}




void AssetLoadingDemo::drawVerts() {

	if (sceneShader)
		sceneShader->use();

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);


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

	
	glm::vec3 objectPositionOffset = POSITION_FIRST_OBJECT_IN_SCENE;
	int objectCounter = 0;

	//Compute the scene size
	size_t sceneSize = 0u;
	for (auto objIter = sceneObjects.begin(); objIter != sceneObjects.end(); objIter++) {
		if (((*objIter)->hasNormals())) {
			if ((*objIter)->hasTexCoords()) {
				sceneSize += (*objIter)->mVertices_.size();
			}
			else {
				sceneSize += (((*objIter)->mVertices_.size() / (4u+3u)) * (4u + 2u + 3u)); //Divide by 'position-normal' vertex size (7u) and then multiply by 'position-texCoord-normal' vertex size (9u)
			}
		}
		else if (((*objIter)->hasTexCoords())) {
			sceneSize += (((*objIter)->mVertices_.size() / (4u+2u)) * (4u + 2u + 3u));  //Divide by 'position-texCoord' vertex size and then multiply by 'position-texCoord-normal' vertex size
		}
		else {
			sceneSize += (((*objIter)->mVertices_.size() / 4u) * (4u + 2u + 3u)); //Divide by 'position' vertex size and then multiply by 'position-texCoord-normal' vertex size
		}
	}

	//Reserve that much space
	fprintf(MSGLOG, "\nCalculated the final scene size as being %u floating point values!\n\n", sceneSize);
	sceneBuffer.reserve(sceneSize); 

	//Iterate through each object in the sceneObjects vector
	for (auto sceneObjIter = sceneObjects.begin(); sceneObjIter != sceneObjects.end(); sceneObjIter++) {
		objectCounter++;
		fprintf(MSGLOG, "\tAdding Object %d to scene...\n", objectCounter);
		fprintf(MSGLOG, "\t\tPosition of Object %d:   <%3.3f, %3.3f, %3.3f>\n", objectCounter,
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
		objectPositionOffset.x += CHANGE_BETWEEN_OBJECTS.x;
		objectPositionOffset.y += CHANGE_BETWEEN_OBJECTS.y;
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


