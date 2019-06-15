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
//  |              'g'                |                           Reverse Time Propogation                             |
//  |                                 |                                                                                |
//  +---------------------------------+--------------------------------------------------------------------------------+
//  |                                 |                                                                                |
//  |              'y'                |                             Decrease Rate of Time                              |
//  |                                 |                                                                                |
//  +---------------------------------+--------------------------------------------------------------------------------+
//  |                                 |                                                                                |
//  |              'u'                |                             Increase Rate of Time                              |
//  |                                 |                                                                                |
//  +---------------------------------+--------------------------------------------------------------------------------+
//  |                                 |                                                                                |
//  |              'r'                |                                  Reset Demo                                    |
//  |                                 | [see member function reset();  should set time to 0 and reset rotations/zoom]  |
//  +---------------------------------+--------------------------------------------------------------------------------+
//  |                                 |                                                                                |
//  |              'i'                |                           Toggle Instanced Rendering**                         |      **Note: Vertex shader must use gl_InstanceID in some way
//  |          '+' and '-'            |  (Requires Instanced Rendering Activated) Increment/Decrement instance count   |              or else all instances will be drawn in the same place.
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
//  |                                 |                                                                                |
//  |              'c'                |                              Toggle Depth Clamp                                |
//  |                                 |                                                                                |
//  +---------------------------------+--------------------------------------------------------------------------------+
//  |                                 |                                                                                |
//  |           '9' & '0'             |                            Increase/Decrease FOV                               |
//  |                                 |                                                                                |
//  +---------------------------------+--------------------------------------------------------------------------------+
//
//
//
//  Additional Notes and General Things to be Aware Of:
//                  -The rotation matrix used in this RenderDemo is built from Euler angles and thus
//                      quite easily runs into the issue of Gimbel lock.
//                  -The behavior of the models on the screen is highly variable dependent upon what the
//                      sceneShaders are doing ("AssetLoadingDemo.vert"  and  "AssetLoadingDemo.frag")
//                  -To allow for the use of a single shader program for rendering models that will not 
//                      necessarily have the same vertex information
//
//                  -There was a lot of experimenting that went into 
//                        this RenderDemo, so a lot of the private member functions of this 
//                        class ideally will make their way into future Utility classes.
//                   [What I mean by this is that there is a heck of a lot of implementation code
//                        that really should be buried into separate classes]
//                   [In fact, I have already started to do this in some places. Thus if you
//                        stumble across objects and/or functions used in the AssetLoadingDemo file that have 
//                        identical tasks/computations/algorithms implemented both in this file and in the
//                        files respective to those objects and/or functions, chances are the idea started 
//                        out in this file and I then moved that functionality to the files of these objects
//                        so that the functionality can be used beyond the scope of this class]
//                         
//
//Programmer:          Forrest Miller
//Date Created:        November 14, 2018
//Date Completed:      tbd...


//#include <cfloat>  //Possibly unused, try commenting out this include statement and building 
//#include <future>
#include "AssetLoadingDemo.h"


//The following 2 global variables can be used to define how models are to be loaded into the scene.
//The first model loaded is translated by the vector:
constexpr const glm::vec3 POSITION_FIRST_OBJECT_IN_SCENE(0.0f, 0.0f, 0.0f);
//Each object after the first has the following translation applied (Note 
// Z-translation is disabled for your own safety due to Z-Clipping hazards!):
constexpr const glm::vec2 CHANGE_BETWEEN_OBJECTS(0.39599f, 0.0439995f);

//Camera Parameters
const glm::vec3 CAMERA_POSITION = glm::vec3(0.0f, 0.0f, -8.5f);
const glm::vec3 CAMERA_LOOK_DIRECTION = glm::vec3(0.2f, 1.0f, 1.0f);
const glm::vec3 CAMERA_UP_DIRECTION = glm::vec3(0.0f, 1.0f, 0.0f);
constexpr const float CAMERA_DEFAULT_FOV = 1.5f;
constexpr const float CAMERA_MAXIMUM_FOV = 3.14159f;
constexpr const float CAMERA_MINIMUM_FOV = -3.14159f;
constexpr const float CAMERA_Z_PLANE_NEAR = 0.05f;
constexpr const float CAMERA_Z_PLANE_FAR = 100.0f;

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
    frameDepthClampLastToggled = 0ull;

    counter = 0.0f;
    timeTickRateModifier = 0.0f;
    vao = 0u;
    sceneBufferVBO = 0u;
    triangleOutlineEBO = 0u;


    //Set the starting input primitive type
    currentPrimitiveInputType = PIPELINE_PRIMITIVE_INPUT_TYPE::DISCRETE_TRIANGLES;

    //Set the variables regarding instanced drawing
    drawMultipleInstances = false;
    instanceCount = STARTING_INSTANCE_COUNT;
    

    freezeTimeToggle = false;
    reverseTimePropogation = false;
    enableBlending = false;
    enableDepthClamping = false;

    //Set values for screen projection 
    fov = CAMERA_DEFAULT_FOV;
    zNear = CAMERA_Z_PLANE_NEAR;
    zFar = CAMERA_Z_PLANE_FAR;

    //Set values for view matrix
    cameraPos = CAMERA_POSITION; //3.0f * 0.5f / tan(glm::radians(fov / 2.f)));
    lookAtOrgin = CAMERA_LOOK_DIRECTION;
    upDirection = CAMERA_UP_DIRECTION;
    //Compute view matrix
    view = glm::lookAt(cameraPos, lookAtOrgin, upDirection);

    xTranslation = 0.0f;
    yTranslation = 0.0f;

    //Set values for Rotation Uniforms
    head = 0.0f;
    pitch = 0.0f;
    roll = 0.0f;
    //Calculate rotation matrix
    rotation = glm::mat4(1.0f);    //Initialize the rotation matrix to 4x4 identity matrix (it will be set to a real rotation matrix later)

    //Keep an extra zoom parameter
    zoom = 1.0f; //Higher number means farther away

    backgroundColor = glm::vec3(0.0f, 0.0f, 0.0f); //The values set here have no impact on the actual background color, see the background-color-update function 

    glEnable(GL_PROGRAM_POINT_SIZE);

    //test
    if (glIsEnabled(GL_MULTISAMPLE))
        fprintf(MSGLOG, "\nMULTISAMPLING IS ENABLED!\n");
    else
        fprintf(MSGLOG, "\nMULTISAMPLING IS DISABLED!\n");
    glDisable(GL_MULTISAMPLE);
}


AssetLoadingDemo::AssetLoadingDemo(InitReport* initReport) : RenderDemoBase() {
	initialize();
    //Make sure we have a monitor to render to
    if (!initReport || !initReport->monitors.activeMonitor.activeMonitor) {
        error = true;
        return;
    }
    //Make sure the context is set to this monitor (and this thread [see glfw documentation])
    if (glfwGetCurrentContext() != initReport->windowContext.window.window) {
        std::ostringstream warning;
		warning << "\nWARNING!\n[In AssetLoadingDemo's constructor]\n" <<
			"AssetLoadingDemo detected that the GLFW active context was set" <<
			"\nto a different monitor or different execution-thread then\n" <<
			"the one passed to AssetLoadingDemo's constructor!\n";
		warning << "This means that running AssetLoadingDemo will invalidate\n" <<
			"the previous context by replacing it with this one, which\n" <<
			"could (probably) lead to errors! Please ensure that the correct context\n" <<
			"is being passed to AssetLoadingDemo in the application code!\n";

		fprintf(WRNLOG, warning.str().c_str());
        glfwMakeContextCurrent(initReport->windowContext.window.window);
    }
    mainRenderWindow = initReport->windowContext.window.window;
    recomputeProjectionMatrix(); //Well really we compute it here for the first time
}



AssetLoadingDemo::~AssetLoadingDemo() noexcept {

    if (vao != 0u) 
        glDeleteVertexArrays(1, &vao);
   
    if (sceneBufferVBO != 0)
        glDeleteBuffers(1, &sceneBufferVBO);

    if (triangleOutlineEBO != 0u)
        glDeleteBuffers(1, &triangleOutlineEBO);

}


void AssetLoadingDemo::run() {
	if (error) {
		fprintf(ERRLOG, "An error occurred while loading AssetLoadingDemo\n");
		return;
	}
	fprintf(MSGLOG, "\nAsset Loading Demo project has loaded and will begin running!\n");


	fprintf(MSGLOG, "\nDemo Starting...!\n");


	fprintf(MSGLOG, "\nEntering Render Loop...\n");


	renderLoop();


    fprintf(MSGLOG, "\nExited Render Loop!\n");

}




void AssetLoadingDemo::loadAssets() {
	loadShaders(); //load the GLSL shader code
	loadModels();  //have the GL context load the Teapot vertices to video memory 
	
	prepareScene();
}



void AssetLoadingDemo::loadShaders() { 
	const std::string shadersRFP = FILEPATH_TO_SHADERS;   //Relative Filepath to location of Shaders

#define USE_RUBYMINE
#ifdef USE_RUBYMINE
	/////////////////////////
	////    RubyMine Shader   (from the internet)
	/////////////////////////
	fprintf(MSGLOG, "\nInitializing Shaders!\n");

	sceneShader = std::make_unique<ShaderProgram>(); //Create the scene shader
	
    //Attach the main shader stages to the sceneShader
	sceneShader->attachVert(shadersRFP + "Sample\\RubyMine.vert"); //Attach Vertex shader to scene
	shaderSources.emplace_back(shadersRFP + "Sample\\RubyMine.vert", true, ShaderInterface::ShaderType::VERTEX);
	sceneShader->attachFrag(shadersRFP + "Sample\\RubyMine.frag"); //Attach Fragment shader to scene
	shaderSources.emplace_back(shadersRFP + "Sample\\RubyMine.frag", true, ShaderInterface::ShaderType::FRAGMENT);
	//Now after all the stages to the shader have been created and attached, it is time to link the sceneShader
	sceneShader->link();
	if (sceneShader->checkIfLinked()) {
		fprintf(MSGLOG, "Program Successfully linked!\n");
		return;
	}
	else {
		fprintf(ERRLOG, "Shader Program was not successfully linked!\n");
		fprintf(MSGLOG, "\t[Press 'ENTER' to attempt to continue program execution]\n");
		std::cin.get(); //Hold the mainRenderWindow open if there was an error
		markMainRenderWindowAsReadyToClose(); //Mark window for closing once error is acknowledged
		return;
	}


#else 

	/////////////////////////
	////    Normal Shader
	/////////////////////////
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
	vertexNoiseShader->makeSecondary();
	sceneShader->attachSecondaryVert(vertexNoiseShader.get());
	shaderSources.emplace_back(shadersRFP + "ShaderNoiseFunctions.glsl", false, ShaderInterface::ShaderType::VERTEX);
	///shaderSources.emplace_back(shadersRFP + "VoronoiNoise.glsl", false, ShaderInterface::ShaderType::VERTEX);

	//Create and attach a secondary fragment shader containing implementations for some noise functions 
	std::unique_ptr<ShaderInterface::FragmentShader> fragmentNoiseShader =
		std::make_unique<ShaderInterface::FragmentShader>(shadersRFP + std::string("ShaderNoiseFunctions.glsl"));
	fragmentNoiseShader->makeSecondary();
	sceneShader->attachSecondaryFrag(fragmentNoiseShader.get()); //the '.get()' function converts the unique_ptr to a raw pointer
	shaderSources.emplace_back(shadersRFP + "ShaderNoiseFunctions.glsl", false, ShaderInterface::ShaderType::FRAGMENT);
    ///shaderSources.emplace_back(shadersRFP + "VoronoiNoise.glsl", false, ShaderInterface::ShaderType::FRAGMENT);

	//Now after all the stages to the shader have been created and attached, it is time to link the sceneShader
	sceneShader->link();
	if (sceneShader->checkIfLinked()) {
		fprintf(MSGLOG, "Program Successfully linked!\n");
	}
	else {
		fprintf(ERRLOG, "Shader Program was not successfully linked!\n");
        //This next line is to give the user false hope
		fprintf(MSGLOG, "\t[Press 'ENTER' to attempt to continue program execution]\n");
		std::cin.get(); //Hold the mainRenderWindow open if there was an error
		markMainRenderWindowAsReadyToClose(); //Mark window for closing once error is acknowledged
		return;
	}


	fprintf(MSGLOG, "\nAll Shaders Successfully Built!\n");
#endif //ifdef USE_RUBYMINE
}


GLsizei AssetLoadingDemo::computeNumberOfVerticesInSceneBuffer(const std::vector<GLfloat>& sceneBuffer) const noexcept{
	static constexpr const GLsizei vertexSize = 4u + 2u + 3u;  //Since each vertex is {x,y,z,w, s,t, nx,ny,nz}  (i.e. 9 components total)
	return (sceneBuffer.size() / vertexSize);
}

void AssetLoadingDemo::generateTriangleOutlineElementOrdering() noexcept {

    //Start with a completely empty order
    std::vector<GLuint> emptyOrder;
    

    const size_t numberOfVerticesInSceneBuffer = computeNumberOfVerticesInSceneBuffer(sceneBuffer);

    //The number of elements required is always twice the number of vertices in the sceneBuffer
    const size_t elementsToGenerate = 2u * numberOfVerticesInSceneBuffer;
    try {
        emptyOrder.reserve(elementsToGenerate);
        for (size_t i = 0u; i < elementsToGenerate; i += 3) {//numberOfVerticesInSceneBuffer; i += TRIANGLE_SIDES_AMOUNTAGE) {

            //Triangle side 1
            emptyOrder.push_back(i);
            emptyOrder.push_back(i + 1);

            //Triangle side 2
            emptyOrder.push_back(i + 1);
            emptyOrder.push_back(i + 2);

            //Triangle side 3
            emptyOrder.push_back(i + 2);
            emptyOrder.push_back(i);
        }
    }
    catch (const std::exception& e) {
        fprintf(ERRLOG, "\nCaught Exception: %s!\n", e.what());
    }

    //Once generated, swap order out with AssetLoadingDemo's member
    triangleOutlineElementOrdering.swap(emptyOrder);
}



void AssetLoadingDemo::loadModels() {

	fprintf(MSGLOG, "\nAcquiring and parsing Model(s) data from file(s)...\n");

	//[RFP == Relative File Path]
	std::string modelsRFP = FILEPATH_TO_MODELS; //Set string to the executable-relative location of Model Files folder

	//Initial Scale values for the objects
	float blockThing_QuadsScale = 1.0f;
	float beveledCubeScale = 1.0f;
	float blockShipScale = 1.0f;
	float subdivisionCubeScale = 1.0f;
	float abstractShapeScale = 1.0f;

    ///////////////////////////////////////////////////////////////////////////////////////////// 
	//                             Load one or more models                                     //  
    ///////////////////////////////////////////////////////////////////////////////////////////// 

    /////////////////////
    //  TEST!
    /////////////
    ///sceneObjects.emplace_back(std::make_unique<QuickObj>(modelsRFP + "Test01_Pos.obj", 1.0f));  //Single triangle test file

    /////////////////////
    //  Well-Behaved models
    /////////////
	///sceneObjects.emplace_back(std::make_unique<QuickObj>(modelsRFP + "blockThing_Quads.obj", blockThing_QuadsScale));
	///sceneObjects.emplace_back(std::make_unique<QuickObj>(modelsRFP + "BeveledCube.obj", beveledCubeScale));
	///sceneObjects.emplace_back(std::make_unique<QuickObj>(modelsRFP + "BlockshipSampleExports\\BlockShipSample_01_3DCoatExport01.obj", blockShipScale));
	///sceneObjects.emplace_back(std::make_unique<QuickObj>(modelsRFP + "SubdivisionCube.obj", subdivisionCubeScale)); //Has no text coords
    sceneObjects.emplace_back(std::make_unique<QuickObj>(modelsRFP + "AbstractShape.obj", abstractShapeScale)); //Only position data
	
	///sceneObjects.emplace_back(std::make_unique<QuickObj>(modelsRFP + "AbstractShapeDecimated.obj", abstractShapeScale));

	///sceneObjects.emplace_back(std::make_unique<QuickObj>(modelsRFP + "NewOrderTie_Triangulated.obj", 1.0f));
	///sceneObjects.emplace_back(std::make_unique<QuickObj>(modelsRFP + "thing.obj", 1.0f));  
	///sceneObjects.emplace_back(std::make_unique<QuickObj>(modelsRFP + "ExperimentalEngine.obj", 1.0f));

    ///sceneObjects.emplace_back(std::make_unique<QuickObj>(modelsRFP + "RockThing.obj", 1.0f));

	///sceneObjects.emplace_back(std::make_unique<QuickObj>(modelsRFP + "ViperMKIV_Fighter.obj", 1.0f));

    ///sceneObjects.emplace_back(std::make_unique<QuickObj>(modelsRFP + "DrillThing00.obj", 1.0));


    //
    //for (float f0 = 0.001f; f0 < 9.001f; f0 += (5.14159f / 19.3f)) {
    //    sceneObjects.emplace_back(std::make_unique<QuickObj>(modelsRFP + "Spaceship.obj", 1.0f));
    //    float baseZoom = 1.0f;
    //    float amplitude = 1.75f;
    //    float zoom = baseZoom + (amplitude * cos(pow(f0, 2.0f / (amplitude))));
    //    sceneObjects.emplace_back(std::make_unique<QuickObj>(modelsRFP + "spiral.obj", zoom));
    //    //sceneObjects.emplace_back(std::make_unique<QuickObj>(modelsRFP + "Spaceship.obj", 1.0f));
    //   
    //   /*
    //    static float denominator = (1.0f - f0);
    //    denominator += (-1.0f * f0);
    //    if (abs(denominator < 0.8f))
    //       denominator = (std::abs(f0 - denominator)) / (f0 + denominator);
    //    if (denominator > 2.0f)
    //       denominator = 0.5f;
    //    sceneObjects.emplace_back(std::make_unique<QuickObj>(modelsRFP + "spiral.obj", (1.0f + f0) / denominator ));
    //    */
    //}

	///sceneObjects.emplace_back(std::make_unique<QuickObj>(modelsRFP + "2DTexturedQuadPlane.obj", 1.0f));

    //Several different objects were given a parent-child relationship in blender and then saved into the same file
    ///sceneObjects.emplace_back(std::make_unique<QuickObj>(modelsRFP + "ParentedPrimatives.obj", 1.0f));

    //////////////////////
    //  Less-Well-Behaved models\
    ////////////

    //File is defined in terms of splines instead of triangles. This may not be able to display properly.
     //for (int i = 0; i < 2; i++)
     //    sceneObjects.emplace_back(std::make_unique<QuickObj>(modelsRFP + "Splines.obj", 1.0f));
     

	//Crazy Engine (Takes several minutes to load, model is over 1,000,000 triangles)
	///sceneObjects.emplace_back(std::make_unique<QuickObj>(modelsRFP + "CrazyJetEngine.obj", 4.5f));


    //Report to console how many models were loaded
	const size_t loadedModlCount = sceneObjects.size();
	if (loadedModlCount == 0u) 
		fprintf(MSGLOG, "\nNo models were loaded!\n");
	else if (loadedModlCount == 1u) 
		fprintf(MSGLOG, "\n%u model has been loaded!\n", loadedModlCount);
	else 
		fprintf(MSGLOG, "\n%u models were loaded!\n", loadedModlCount);
	

}

void AssetLoadingDemo::prepareScene() {
	fprintf(MSGLOG, "\nCreating the primary scene from loaded assets...\n");
	buildSceneBufferFromLoadedSceneObjects();
    generateTriangleOutlineElementOrdering();
	fprintf(MSGLOG, "Primary scene creation complete!\n\n");

    createSceneVBO();
    createTriangleOutlineEBO();

	fprintf(MSGLOG, "Uploading scene buffer to GPU...\n");
	uploadSceneBufferToGPU(sceneBufferVBO, sceneBuffer);
	configureVertexArrayAttributes(); 
    uploadTriangleOutlineElementOrderingBufferToGPU(triangleOutlineEBO, triangleOutlineElementOrdering);
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
        
        if (checkIfShouldReverseDirectionOfTime())
            reverseTime();

        /* 
        zNear += 0.000025f;
        if (zNear >= 0.02f)
            zNear = 0.00008f;
        recomputeProjectionMatrix(); */

        if (checkIfShouldIncreasePassageOfTime())
            increasePassageOfTime();

        if (checkIfShouldDecreasePassageOfTime())
            decreasePassageToTime();

		if (checkIfShouldToggleBlending()) 
			toggleBlending();
		
        if (checkIfShouldToggleDepthClamping()) 
            toggleDepthClamping();
        
        if (checkIfShouldUpdateFieldOfView())
            updateFieldOfView();

		//More Input Checking
		
		changePrimitiveType();
		changeInstancedDrawingBehavior(); //Toggle on/off drawing instances
		rotate();
		translate();

		//Perform logic 

		performRenderDemoSharedInputLogic(); //This is the loop function of the base class

		if ((frameNumber % FRAMES_TO_WAIT_BEFORE_CHECKING_TO_UPDATE_SHADERS) ==
			(FRAMES_TO_WAIT_BEFORE_CHECKING_TO_UPDATE_SHADERS - 1ULL)) { //check every 59th frame (of a 60-frame cycle) for updated shaders
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
            float delta = (0.0125f * (1.0f + timeTickRateModifier));
            ((reverseTimePropogation) ? (counter += delta) : (counter -= delta)); //compute time propogation 
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




inline bool AssetLoadingDemo::checkToSeeIfShouldCloseWindow() const  noexcept {
	if (glfwGetKey(mainRenderWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		return true;
	}
	return false;
}

inline bool AssetLoadingDemo::checkIfShouldPause() const noexcept {
	if ((frameNumber >= (frameUnpaused + DELAY_LENGTH_OF_PAUSE_CHECKING_AFTER_UNPAUSE))
		&& (glfwGetKey(mainRenderWindow, GLFW_KEY_SPACE) == GLFW_PRESS)) {
		return true;
	}
	return false;
}


inline bool AssetLoadingDemo::checkIfShouldReset() const noexcept {
	if (glfwGetKey(mainRenderWindow, GLFW_KEY_R) == GLFW_PRESS)
		return true;
	return false;
}

inline bool AssetLoadingDemo::checkIfShouldFreezeTime() const  noexcept {
	if ((frameNumber - frameTimeFreezeLastToggled) > FRAMES_TO_WAIT_BETWEEN_INPUT_READS) {
		if (glfwGetKey(mainRenderWindow, GLFW_KEY_T) == GLFW_PRESS) {
			return true;
		}
	}
	return false;
}

inline bool AssetLoadingDemo::checkIfShouldReverseDirectionOfTime() const noexcept {
    static bool keyWasPressedOnPreviousFrame = false;

    if (glfwGetKey(mainRenderWindow, GLFW_KEY_G) == GLFW_PRESS) {
        if (!keyWasPressedOnPreviousFrame) {
            keyWasPressedOnPreviousFrame = true; //setup for the next frame
            return true;
        }
    }

    else
        keyWasPressedOnPreviousFrame = false;

    return false;
}

inline bool AssetLoadingDemo::checkIfShouldIncreasePassageOfTime() const noexcept {
    if (glfwGetKey(mainRenderWindow, GLFW_KEY_U) == GLFW_PRESS) {
        return true;
    }
    return false;
}

inline bool AssetLoadingDemo::checkIfShouldDecreasePassageOfTime() const noexcept {
    if (glfwGetKey(mainRenderWindow, GLFW_KEY_Y) == GLFW_PRESS) {
        return true;
    }
    return false;
}

inline bool AssetLoadingDemo::checkIfShouldToggleBlending() const  noexcept {
	if ((frameNumber - frameBlendOperationLastToggled) > FRAMES_TO_WAIT_BETWEEN_INPUT_READS) {
		if (glfwGetKey(mainRenderWindow, GLFW_KEY_B) == GLFW_PRESS) {
			return true;
		}
	}
	return false;	
}

inline bool AssetLoadingDemo::checkIfShouldToggleDepthClamping() const noexcept {
    //For a detailed discussion on depth clamping that is really useful, check out OpenGL SuperBible 7e pages 379-380.
    //Essentially, within the fragment shader stage of the OpenGL Pipeline, each fragment has a depth value that is
    //scaled between 0 to 1, with 0 being the near plane (right in your face) and 1 being the furthest representable depth.
    //Unfortunately there is no way to represent arbitrarily far away fragments using this scale, so to compensate depth
    //clamping can be enabled, which will "disable clipping against the near and far planes." I feel this description is
    //a bit misleading however, because a more accurate description is it will 'clamp' fragments of depth less than 0 to 0 
    //(causing fragments that should be behind your eye instead map flatly onto your eye) and arbitrarily far away values 
    //will bet clamped to 1, the furthest representable depth.
    if ((frameNumber - frameDepthClampLastToggled) > FRAMES_TO_WAIT_BETWEEN_INPUT_READS) {
        if (glfwGetKey(mainRenderWindow, GLFW_KEY_C) == GLFW_PRESS) {
            return true;
        }
    }
    return false;
}

inline bool AssetLoadingDemo::checkIfShouldUpdateFieldOfView() const noexcept {

    if (glfwGetKey(mainRenderWindow, GLFW_KEY_9) == GLFW_PRESS) 
        return true;
    if (glfwGetKey(mainRenderWindow, GLFW_KEY_0) == GLFW_PRESS) 
        return true;
    return false;

}

void AssetLoadingDemo::pause() {
	const auto begin = std::chrono::high_resolution_clock::now(); //Time measurement
	auto end = std::chrono::high_resolution_clock::now();
	fprintf(MSGLOG, "PAUSED!\n");
	//Upon first pausing, enter into the following loop for a short period of time before moving on to
	//the full pause loop. This will prevent unpausing from occuring directly after a pause is initiated. 
	while (std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count() < 300000000LL) {
		std::this_thread::sleep_for(std::chrono::nanoseconds(2000000LL));
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


void AssetLoadingDemo::reset() noexcept {
	fprintf(MSGLOG, "\nReseting Demo...\n");
	counter = 0.0f; //Reset time to 0
    timeTickRateModifier = 0.0f;
    fov = CAMERA_DEFAULT_FOV;
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
    frameDepthClampLastToggled = 0ull;
	zoom = 1.0f;
	if (drawMultipleInstances) {
		instanceCount = STARTING_INSTANCE_COUNT;
	}
    
    recomputeProjectionMatrix();
}

void AssetLoadingDemo::toggleTimeFreeze() noexcept {
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

void AssetLoadingDemo::reverseTime() noexcept {
    reverseTimePropogation = !reverseTimePropogation;
    fprintf(MSGLOG, "Time is now propogating %s\n",
        reverseTimePropogation ? "Backwards" : "Forwards");
}

void AssetLoadingDemo::increasePassageOfTime() noexcept {
    timeTickRateModifier += 0.005f;
    static auto frameUpdateMessageWasLastPrinted = frameNumber;
    if (frameNumber < frameUpdateMessageWasLastPrinted) //
        frameUpdateMessageWasLastPrinted = frameNumber;
    else if (frameNumber >= (15ull + frameUpdateMessageWasLastPrinted))
        frameUpdateMessageWasLastPrinted = frameNumber;
    else
        return;

    float delta = (1.0f + timeTickRateModifier) * 100.0f;
    if (reverseTimePropogation)
        delta *= -1.0f;
    fprintf(MSGLOG, "Time now operating at %%%f speed\n", delta);
}

void AssetLoadingDemo::decreasePassageToTime() noexcept {
    timeTickRateModifier -= 0.005f;
    static auto frameUpdateMessageWasLastPrinted = frameNumber;
    if (frameNumber < frameUpdateMessageWasLastPrinted) 
        frameUpdateMessageWasLastPrinted = frameNumber;
    else if (frameNumber >= (15ull + frameUpdateMessageWasLastPrinted))
        frameUpdateMessageWasLastPrinted = frameNumber;
    else
        return;
    fprintf(MSGLOG, "Time now operating at %%%f speed\n", (1.0f+timeTickRateModifier) * 100.0f);
}


void AssetLoadingDemo::toggleBlending() noexcept {
	//Note that it is vitally important that 'frameBlendOperationLastToggled' is updated to match the current 'frameNumber'
	frameBlendOperationLastToggled = frameNumber;

	enableBlending = !enableBlending;
	if (enableBlending) {
		fprintf(MSGLOG, "Blending Enabled!\tBlend Function set to \'ONE_MINUS_SOURCE_ALPHA\' \n");
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	else {
		fprintf(MSGLOG, "Blending Disabled!\n");
		glDisable(GL_BLEND);
	}
}

void AssetLoadingDemo::toggleDepthClamping() noexcept {
    //See: https://www.khronos.org/opengl/wiki/Vertex_Post-Processing#Depth_clamping
    if (!enableDepthClamping) {
        fprintf(MSGLOG, "Depth Clamping Enabled!\n");
        enableDepthClamping = true;
        glEnable(GL_DEPTH_CLAMP);
    }
    else {
        fprintf(MSGLOG, "Depth Clamping Disabled!\n");
        enableDepthClamping = false;
        glDisable(GL_DEPTH_CLAMP);
    }
    frameDepthClampLastToggled = frameNumber;
}

void AssetLoadingDemo::updateFieldOfView() noexcept {
    
    static constexpr const float FOV_DELTA_PER_FRAME = 0.00831f;

    static float lastPrintedFOVUpdate = fov; 

    if (glfwGetKey(mainRenderWindow, GLFW_KEY_9) == GLFW_PRESS)
        fov += FOV_DELTA_PER_FRAME;
    if (glfwGetKey(mainRenderWindow, GLFW_KEY_0) == GLFW_PRESS)
        fov -= FOV_DELTA_PER_FRAME;

    if (fov > CAMERA_MAXIMUM_FOV)
        fov = CAMERA_MAXIMUM_FOV;
    if (fov < CAMERA_MINIMUM_FOV)
        fov = CAMERA_MINIMUM_FOV;

    if (fabs(fov) < 0.01f)
        fov = 0.01f;
    //This logs fov updates whenever there is a change 
    if ( abs(lastPrintedFOVUpdate - fov) >= 0.04f ) {
            lastPrintedFOVUpdate = fov;
            fprintf(MSGLOG, "FOV is now %f\n", fov);
        
    }

    //Need to update the projection matrix with the new fov value
    recomputeProjectionMatrix();
}

void AssetLoadingDemo::recomputeProjectionMatrix() noexcept {
    
    //Must get value of the window's width and height
    float screenWidth = 1.0f;
    float screenHeight = 1.0f;
    if (mainRenderWindow) {
        int width = 1;
        int height = 1;
        glfwGetWindowSize(mainRenderWindow, &width, &height);
        screenWidth = static_cast<float>(width);
        screenHeight = static_cast<float>(height);
    }


    //Compute the screen -projection matrix
    //perspective = glm::mat4(1.0f); //Set projection matrix to 4x4 identity
    //see: https://gamedev.stackexchange.com/questions/98226/how-can-i-set-up-an-intuitive-perspective-projection-view-matrix-combination-in
    perspective = glm::perspectiveFov(fov, screenWidth, screenHeight, zNear, zFar );

}


void AssetLoadingDemo::changePrimitiveType() noexcept {
    static PIPELINE_PRIMITIVE_INPUT_TYPE previousPrimitiveInputType = currentPrimitiveInputType;

    if (glfwGetKey(mainRenderWindow, GLFW_KEY_1) == GLFW_PRESS)
        currentPrimitiveInputType = PIPELINE_PRIMITIVE_INPUT_TYPE::DISCRETE_TRIANGLES;

    else if (glfwGetKey(mainRenderWindow, GLFW_KEY_2) == GLFW_PRESS)
        currentPrimitiveInputType = PIPELINE_PRIMITIVE_INPUT_TYPE::TRIANGLE_STRIP;

    else if (glfwGetKey(mainRenderWindow, GLFW_KEY_3) == GLFW_PRESS)
        currentPrimitiveInputType = PIPELINE_PRIMITIVE_INPUT_TYPE::TRIANGLE_FAN;

    else if (glfwGetKey(mainRenderWindow, GLFW_KEY_4) == GLFW_PRESS)
        currentPrimitiveInputType = PIPELINE_PRIMITIVE_INPUT_TYPE::POINTS;
    
    //Pressing the '`' key will toggle between the 3 options for drawing line primatives
    if (glfwGetKey(mainRenderWindow, GLFW_KEY_GRAVE_ACCENT) == GLFW_PRESS) {
        if ((frameNumber - frameLineTypeLastSwitched) < 15ull) {
            frameLineTypeLastSwitched = frameNumber;
        }
        else {
            frameLineTypeLastSwitched = frameNumber;
            if (PIPELINE_PRIMITIVE_INPUT_TYPE::LINE == currentPrimitiveInputType)
                currentPrimitiveInputType = PIPELINE_PRIMITIVE_INPUT_TYPE::TRIANGLE_OUTLINE;
            else if (PIPELINE_PRIMITIVE_INPUT_TYPE::TRIANGLE_OUTLINE == currentPrimitiveInputType)
                currentPrimitiveInputType = PIPELINE_PRIMITIVE_INPUT_TYPE::LINE_STRIP;
            else 
                currentPrimitiveInputType = PIPELINE_PRIMITIVE_INPUT_TYPE::LINE;
        }
    }

    //Here we determine if we have changed primitive types 
    if (currentPrimitiveInputType != previousPrimitiveInputType) {
        previousPrimitiveInputType = currentPrimitiveInputType;
        printNameOfTheCurrentlyActivePrimitive();
    }
}

void AssetLoadingDemo::changeInstancedDrawingBehavior() noexcept {
	//Only allow toggling to happen every 11 frames
	if ((frameNumber - frameInstancedDrawingBehaviorLastToggled) > 11ull) {
		if (glfwGetKey(mainRenderWindow, GLFW_KEY_I) == GLFW_PRESS) {
			frameInstancedDrawingBehaviorLastToggled = frameNumber; //Mark the current frame as being the one when instanced drawing behavior was last toggled
			drawMultipleInstances = !drawMultipleInstances; //Perform Toggle
			fprintf(MSGLOG, "Instanced Rendering set to: %s",
				(drawMultipleInstances ? ("Enabled\n") : ("Disabled\n")));
		}
	}

	//Only allow the instance count to be modified every 10 frames (1/6th second)
	if ((frameNumber - frameInstancedDrawingCountLastModified) > 10ull) {
		if (drawMultipleInstances) {
			if (glfwGetKey(mainRenderWindow, GLFW_KEY_EQUAL) == GLFW_PRESS) {
				frameInstancedDrawingBehaviorLastToggled = frameNumber; 
				instanceCount++;
				fprintf(MSGLOG, "Rendered Instances increased to: %d\n", instanceCount);
			}
			else if (glfwGetKey(mainRenderWindow, GLFW_KEY_MINUS) == GLFW_PRESS) {
				if (instanceCount > 0u) { //Don't decrement unsigned value below 0
					frameInstancedDrawingBehaviorLastToggled = frameNumber;
					instanceCount--;
					fprintf(MSGLOG, "Rendered Instances decreased to: %d\n", instanceCount);
				}
			}
		}
	}
}

void AssetLoadingDemo::rotate() noexcept {

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

void AssetLoadingDemo::translate() noexcept {
	float turbo = 1.0f;
	const float xSpeed = 0.1f;
	const float ySpeed = 0.1f;
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
	//backupSceneShader = nullptr;
	backupSceneShader = std::make_unique<ShaderProgram>();
	for (auto shaderIterator = shaderSources.begin(); shaderIterator != shaderSources.end(); shaderIterator++) {
		
		switch (shaderIterator->type) {
		case (ShaderInterface::ShaderType::VERTEX):
			if (shaderIterator->primary)
				backupSceneShader->attachVert(shaderIterator->file.filepath().c_str());
			else {
				std::unique_ptr<ShaderInterface::VertexShader> secondaryVertexShader =
					std::make_unique<ShaderInterface::VertexShader>(shaderIterator->file.filepath().c_str());
				secondaryVertexShader->makeSecondary();
				backupSceneShader->attachSecondaryVert(secondaryVertexShader.get());
			}
			break;

		case (ShaderInterface::ShaderType::GEOMETRY):
			fprintf(MSGLOG, "\nDetected that a geometry shader was updated!\n"
				"Unfortunately that type of shader is not yet supported for dynamic updates!\n");
			break;

		case (ShaderInterface::ShaderType::TESSELLATION_CONTROL):
			fprintf(MSGLOG, "\nDetected that a Tessellation Control shader was updated!\n"
				"Unfortunately that type of shader is not yet supported for dynamic updates!\n");
			break;

		case (ShaderInterface::ShaderType::TESSELATION_EVALUATION):
			fprintf(MSGLOG, "\nDetected that a Tessellation Evaluation shader was updated!\n"
				"Unfortunately that type of shader is not yet supported for dynamic updates!\n");
			break;

		case (ShaderInterface::ShaderType::FRAGMENT):
			if (shaderIterator->primary)
				backupSceneShader->attachFrag(shaderIterator->file.filepath().c_str());
			else {
				std::unique_ptr<ShaderInterface::FragmentShader> secondaryFragmentShader =
					std::make_unique<ShaderInterface::FragmentShader>(shaderIterator->file.filepath().c_str());
				secondaryFragmentShader->makeSecondary();
				backupSceneShader->attachSecondaryFrag(secondaryFragmentShader.get());
			}
			break;

		case (ShaderInterface::ShaderType::COMPUTE):
			fprintf(MSGLOG, "\nDetected that a Compute shader was updated!\n"
				"Unfortunately that type of shader is not yet supported for dynamic updates!\n");
			break;

		default: // (Default should never happen so the message to be printed is a bit rediculous)
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
	if constexpr (true) {
		glClearColor(0.0132f, 0.024f, 0.0135f, 1.0f);
		//glClearColor(0.132f, 0.24f, 0.135f, 1.0f);
	}
	else if constexpr (false) {
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

	}
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
	const glm::mat4 userTranslation = glm::mat4(1.0f, 0.0f, 0.0f, 0.0f,             //Translation from user input
			                                    0.0f, 1.0f, 0.0f, 0.0f,
			                                    0.0f, 0.0f, 1.0f, 0.0f,
			                                    xTranslation, yTranslation, 0.0f, 1.0f);
    MVP *= userTranslation;//* MVP;

	
	sceneShader->uniforms.updateUniformMat4x4("MVP", &MVP);
	


}




void AssetLoadingDemo::drawVerts() {
    
    const GLsizei BUFFER_SIZE = computeNumberOfVerticesInSceneBuffer(sceneBuffer);

	if (sceneShader)
		sceneShader->use();

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, sceneBufferVBO);

    //The OpenGL draw commands used to draw in mode TRIANGLE_OUTLINE are different from 
    //the other draw modes in that an element array buffer is required. Thus we check for 
    //this draw mode first as a special case.

    /*
    
void glDrawElementsInstanced(	GLenum mode,
 	GLsizei count,
 	GLenum type,
 	const void * indices,
 	GLsizei instancecount);
    */
    if (currentPrimitiveInputType == PIPELINE_PRIMITIVE_INPUT_TYPE::TRIANGLE_OUTLINE) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triangleOutlineEBO);
        if (drawMultipleInstances)
            glDrawElementsInstanced(GL_LINES, 2*BUFFER_SIZE, GL_UNSIGNED_INT, 0, instanceCount);
        else
            glDrawElements(GL_LINES, 2*BUFFER_SIZE, GL_UNSIGNED_INT, 0); //Last param is offset into ebo to start with
        return;
    }




	if (currentPrimitiveInputType == PIPELINE_PRIMITIVE_INPUT_TYPE::DISCRETE_TRIANGLES) {
		if (drawMultipleInstances) 
			glDrawArraysInstanced(GL_TRIANGLES, 0, BUFFER_SIZE, instanceCount);
		else 
			glDrawArrays(GL_TRIANGLES, 0, BUFFER_SIZE);
	}

	else if (currentPrimitiveInputType == PIPELINE_PRIMITIVE_INPUT_TYPE::TRIANGLE_STRIP) {
		if (drawMultipleInstances) 
			glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, BUFFER_SIZE, instanceCount);
		else 
			glDrawArrays(GL_TRIANGLE_STRIP, 0, BUFFER_SIZE);
	}

	else if (currentPrimitiveInputType == PIPELINE_PRIMITIVE_INPUT_TYPE::TRIANGLE_FAN) {
		if (drawMultipleInstances) 
			glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, BUFFER_SIZE, instanceCount);
		else 
			glDrawArrays(GL_TRIANGLE_FAN, 0, BUFFER_SIZE);
	}

	else if (currentPrimitiveInputType == PIPELINE_PRIMITIVE_INPUT_TYPE::LINE) {
		if (drawMultipleInstances) 
			glDrawArraysInstanced(GL_LINES, 0, BUFFER_SIZE, instanceCount);
		else 
			glDrawArrays(GL_LINES, 0, BUFFER_SIZE);
	}

	else if (currentPrimitiveInputType == PIPELINE_PRIMITIVE_INPUT_TYPE::LINE_STRIP) {
		if (drawMultipleInstances) 
			glDrawArraysInstanced(GL_LINE_STRIP, 0, BUFFER_SIZE, instanceCount);
		else 
			glDrawArrays(GL_LINE_STRIP, 0, BUFFER_SIZE);
	}

    else if (currentPrimitiveInputType == PIPELINE_PRIMITIVE_INPUT_TYPE::POINTS) {
        if (drawMultipleInstances) 
            glDrawArraysInstanced(GL_POINTS, 0, BUFFER_SIZE, instanceCount);
        else 
            glDrawArrays(GL_POINTS, 0, BUFFER_SIZE);
    }
}


void AssetLoadingDemo::prepareGLContextForNextFrame() noexcept {
	glBindVertexArray(0);
	glUseProgram(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


void AssetLoadingDemo::printNameOfTheCurrentlyActivePrimitive() const noexcept {
    fprintf(MSGLOG, "Active Primitive Type Set To:  ");
    std::string primitiveType;
    switch (currentPrimitiveInputType) {
    case (PIPELINE_PRIMITIVE_INPUT_TYPE::POINTS):
        primitiveType = "POINT\n";
        break;
    case (PIPELINE_PRIMITIVE_INPUT_TYPE::LINE):
        primitiveType = "LINE\n";
        break;
    case (PIPELINE_PRIMITIVE_INPUT_TYPE::TRIANGLE_OUTLINE):
        primitiveType = "TRIANGLE OUTLINE\n";
        break;
    case (PIPELINE_PRIMITIVE_INPUT_TYPE::LINE_STRIP):
        primitiveType = "LINE STRIP\n";
        break;
    case (PIPELINE_PRIMITIVE_INPUT_TYPE::DISCRETE_TRIANGLES):
        primitiveType = "TRIANGLES\n";
        break;
    case (PIPELINE_PRIMITIVE_INPUT_TYPE::TRIANGLE_STRIP):
        primitiveType = "TRIANGLE STRIP\n";
        break;
    case (PIPELINE_PRIMITIVE_INPUT_TYPE::TRIANGLE_FAN):
        primitiveType = "TRIANGLE FAN\n";
        break;
    default:
        primitiveType = "[UNIDENTIFIED]\n";
        break;
    }

    fprintf(MSGLOG, "%s", primitiveType.c_str());
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
        sceneSize += (*objIter)->mVertices_.size();             
    }
	
	//Reserve that much space
	fprintf(MSGLOG, "\nCalculated the final scene size as being %u floating point values!\n\n", sceneSize);
	sceneBuffer.reserve(sceneSize); 

	//Iterate through each object in the sceneObjects vector
	for (auto sceneObjIter = sceneObjects.cbegin(); sceneObjIter != sceneObjects.cend(); sceneObjIter++) {
		objectCounter++;

		//Objects missing Normal and/or Texture Coordinates will need to have data generated for them
		//so that all vertices in the scene share the same format

		

		//Increment offset to prepare for the next object
		objectPositionOffset.x += CHANGE_BETWEEN_OBJECTS.x + MathFunc::getRandomInRangef(-12.0f, 12.0f);
        objectPositionOffset.y += CHANGE_BETWEEN_OBJECTS.y + MathFunc::getRandomInRangef(-6.0f, 6.0f);
        objectPositionOffset.z += MathFunc::getRandomInRangef(1.0f, 1.0f);

        //objectPositionOffset = glm::normalize(objectPositionOffset);

        fprintf(MSGLOG, "\tAdding Object %d to scene...\n", objectCounter);
        fprintf(MSGLOG, "\t\tPosition of Object %d:   <%3.3f, %3.3f, %3.3f>\n", objectCounter,
            objectPositionOffset.x, objectPositionOffset.y, objectPositionOffset.z);
	
        addObject(sceneObjIter, objectPositionOffset);
    }

    
}



//If the object already has both normals and texCoords, then all that is needed to be modified is 
//the object's position
void AssetLoadingDemo::addObject(std::vector<std::unique_ptr<QuickObj>>::const_iterator object,
	const glm::vec3& objPos) {

	int vertComponentCounter = -1; //variable will be incremented to '0' on start of first loop iteration

	auto vertsEnd = (*object)->mVertices_.cend(); //Create a variable for loop exit condition
	for (auto vertIter = (*object)->mVertices_.cbegin(); vertIter != vertsEnd; vertIter++) {
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


void AssetLoadingDemo::createSceneVBO() noexcept {

    glGenBuffers(1, &sceneBufferVBO);
    fprintf(MSGLOG, "\nCreated a vertex buffer object to store scene vertices. (BufferID = %u)\n\n", sceneBufferVBO);

}


void AssetLoadingDemo::createTriangleOutlineEBO() noexcept {

    glGenBuffers(1, &triangleOutlineEBO);
    fprintf(MSGLOG, "\nCreated an element buffer object to store the alternative\n"
        "vertice ordering required to properly draw lines. (BufferID = %u)\n\n",
        triangleOutlineEBO);
}


void AssetLoadingDemo::uploadSceneBufferToGPU(GLuint& targetVBO, const std::vector<float>& sceneBuf) noexcept {

    //if (sceneBuf.size() == 0u)
    //    return;
    if (0u == targetVBO) {
        fprintf(WRNLOG, "\nWARNING! An issue has occured while uploading a sceneBuffer\n"
            "to its Vertex Buffer Object because it appears as though the target VBO was never\n"
            "allocated from the context!\n");
        fprintf(WRNLOG, "\nThis function here will try to allocate a VBO to use instead...\n");
        glCreateBuffers(1, &targetVBO);
        if (0u == targetVBO) {
            fprintf(ERRLOG, "\n\n\nThe attempt to allocate a VBO to use has failed...\n");
            fprintf(ERRLOG, "         Press [ENTER] to crash...\n");
            std::cin.get();
            std::exit(EXIT_FAILURE);
        }
        else
            fprintf(WRNLOG, "A Vertex Buffer Object with ID=%u has been created!\n\n", targetVBO);
    }

	const GLsizei vertexCount = computeNumberOfVerticesInSceneBuffer(sceneBuf);


	glBindBuffer(GL_ARRAY_BUFFER, targetVBO);

	fprintf(MSGLOG, "\nInitiating transfer of the sceneBuffer data from the Application to the GPU.\n"
        "Target destination on GPU is set to use Array Buffer ID %u.\n", targetVBO);

    fprintf(MSGLOG, "  [TRANSFER STATISTICS]\n");
    fprintf(MSGLOG, "There are %u vertices total in the scene, or %u 32-bit floating point values\n\n", vertexCount, vertexCount * NUM_VERTEX_COMPONENTS);

	glBufferData(GL_ARRAY_BUFFER, sceneBuf.size() * sizeof(sceneBuf.data()), sceneBuf.data(), GL_STATIC_DRAW);

}

void AssetLoadingDemo::uploadTriangleOutlineElementOrderingBufferToGPU(GLuint& ebo, const std::vector<GLuint>& eboData) noexcept {

    if (0u == ebo) {
        fprintf(WRNLOG, "\n\tWARNING!\nUnable to Upload Element Ordering Buffer to GPU because EBO was\n"
            "never created with GL context!\n");
        return;
    }

    const GLsizei elementBufferSize = eboData.size();


    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    fprintf(MSGLOG, "\nTransfering Additional Data from the Application to the GPU.\n"
        "Target destination on GPU is set to use Element Array Buffer ID %u.\n", ebo);

    fprintf(MSGLOG, "  [TRANSFER STATISTICS]\n");
    fprintf(MSGLOG, "There are %u indices total in the scene, or %u GL_UNSIGNED_INT values\n\n", elementBufferSize, elementBufferSize);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * elementBufferSize, eboData.data(), GL_STATIC_DRAW);

}



void AssetLoadingDemo::configureVertexArrayAttributes() noexcept {

    if (vao == 0u)
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


    //Somewhat unrelated note on sending shader's integer values (from OpenGL Insights page 70) 
    //    "OpenGL is very flexible regarding the data types of the vertex array attributes
    // [i.e. the data stored in vao], as traditionally all types are cast by the hardware to 
    // floating-point values when using 'glVertexAttribPointer'. For example, if an array 
    // buffer stores RGB8 colors [i.e. 3 8-bit integers], the color will be exposed as a
    // vec3 by the corresponding vertex shader input variable: the buffer actually stores 
    // unsigned byte data, but at vertex attribute fetching, the values are converted on the
    // fly. 
    //     To escape from this flexibility [but performance-hit], we can use glVertexAttribIPointer,
    // which can only expose vertex arrays that store integers, GL_BYTE, GL_UNSIGNED_BYTE, GL_SHORT,
    // GL_UNSIGNED_SHORT, GL_INT, and GL_UNSIGNED_INT, with integer-based vertex input variables"
}


