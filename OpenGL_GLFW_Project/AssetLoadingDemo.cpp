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
//                        preparatory work to add, since uv-coordinates are loaded 
//                        for models that have them available. Models without UV coords 
//                        will have randomly generated texture coordinates, which could 
//                        be interesting if used to sample from one of the available noise
//                        functions. 
//                     -Models that do not contain normal data will have it generated for them on a 
//                        triangle-by-triangle basis (i.e. all 3 vertices of each triangle are assigned
//                        the same normal). This is (I think) less-than-ideal compared with per-vertex
//                        normals, but it gets the job done just fine.
//                     -The load times can get pretty long for larger models, there is definitely
//                        work needed still for object loading. The current implementation is entirely
//                        single threaded and my algorithm performs several iterations over the data to 
//                        change it from its '.obj' storage (with Positions, Texture Coordinates and 
//                        Normals stored in separate sections of the file) to interlaced vertices (in the 
//                        9-component ordering of {x,y,z,w,s,t,nx,ny,nz}  [with nx, ny, nz as the normal's
//                        xyz components]). With some rewriting the number of steps (i.e. copies and  
//                        allocations) performed should be reducible. I have also been investigating
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
//  |             Input               |                              Description of Action                             |
//  +---------------------------------+--------------------------------------------------------------------------------+
//  ===~~~~~~~~~~~~~~~~~~~~~~~~~~~~~=====~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~===
//  +---------------------------------+--------------------------------------------------------------------------------+
//  |                                 |                                                                                |
//  |           Arrow Keys            |            Modifies the rotation angles for Pitch and Head(Yaw)                |
//  |                                 |                                                                                |
//  +---------------------------------+--------------------------------------------------------------------------------+
//  |                                 |                                                                                |
//  |          'Q' and 'E'            |                        Modifies the Roll rotation angle                        | //Rotations are all just Euler rotations so expect gimbal lock
//  |                                 |                                                                                |
//  +---------------------------------+--------------------------------------------------------------------------------+
//  |                                 |                                                                                |
//  |              Z, W               |                    Changes the value of the uniform 'zoom'                     |
//  |                                 |                                                                                |
//  +---------------------------------+--------------------------------------------------------------------------------+
//  |                                 |                 Modifies input primitive for scene draw calls                  |
//  |        Tilde/'1'/'2'/'3'        |              [Press tilde once for LINES, twice for LINE_STRIP]                |
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
//  |              'g'                |                           Reverse Time Propagation                             |
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
//  |                                 |                                                                                |
//  |          CNTRL + 'S'            |             Enabled/Disabled Performance Reporting to Console                  |
//  |                                 |                                                                                |
//  +---------------------------------+--------------------------------------------------------------------------------+
//  ===~~~~~~~~~~~~~~~~~~~~~~~~~~~~~=====~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~===
//  +---------------------------------+--------------------------------------------------------------------------------+
//  | [GENERAL PATTERN]               |                                                                                |
//  |            L/R Shift            |             Increases the speed of many of the available actions               |
//  |                                 |   [Some actions treat L-Shift, R-Shift or both L/R-Shifts as unique inputs]    |
//  ===~~~~~~~~~~~~~~~~~~~~~~~~~~~~~=====~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~===
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

//ProjectWide Header File Defining Asset Data Directories
#include "RelativeFilepathsToResources.h"

//#include "TGAImage.h" //For testing purposes
#include "ImageData_UByte.h"

//The following 2 global variables can be used to define how models are to be loaded into the scene.
//The first model loaded is translated by the vector:
constexpr const glm::vec3 POSITION_FIRST_OBJECT_IN_SCENE(0.0f, 0.0f, 0.0f);
//Each object after the first has the following translation applied (Note 
// Z-translation is disabled for your own safety due to Z-Clipping hazards!):
constexpr const glm::vec2 CHANGE_BETWEEN_OBJECTS(0.139599f, 0.1439995f);

//Camera Parameters
const glm::vec3 CAMERA_POSITION = glm::vec3(0.0f, 0.0f, 8.0f);
const glm::vec3 CAMERA_LOOK_DIRECTION = glm::vec3(0.0f, 0.0f, 1.0f);
const glm::vec3 CAMERA_UP_DIRECTION = glm::vec3(0.0f, 1.0f, 0.0f);
constexpr const float CAMERA_DEFAULT_FOV = 1.342f;//65.0f * 3.14159f / 180.0f;//1.5f;
constexpr const float CAMERA_MAXIMUM_FOV = 3.14159f*10.0f;
constexpr const float CAMERA_MINIMUM_FOV = -3.14159f*10.0f;
constexpr const float CAMERA_Z_PLANE_NEAR = 0.05f;
constexpr const float CAMERA_Z_PLANE_FAR = 1000.0f;

constexpr const unsigned long long FRAMES_BETWEEN_PERFORMANCE_REPORT = 180ULL;


//This function is intended to be called only through this class's constructor and 
//is in charge of assigning every member field an initial value
void AssetLoadingDemo::initialize() {
    //Set error flag
    error = false;

    //Set FrameCounter-related variables (Note that these must all be reset in the 'reset()' function as well)
    frameCounter = 0ULL;
    frameUnpaused = 0ULL;
    frameLineTypeLastSwitched = 0ULL;  
    frameInstancedDrawingBehaviorLastToggled = 0ULL;
    frameInstancedDrawingCountLastModified = 0ULL;
    frameTimeFreezeLastToggled = 0ULL;
    frameBlendOperationLastToggled = 0ULL;
    frameDepthClampLastToggled = 0ULL;
    frameThatTimePropogationWasLastReversed = 0ULL;
    frameThatCustomShaderParameter1LastModified = 0ULL;
    frameThatCustomShaderParameter2LastModified = 0ULL;
    frameThatCustomShaderParameter3LastModified = 0ULL;
    framePerformanceReportingLastToggled = 0ULL;
    counter = 0.0f;
    timeTickRateModifier = 0.0f;
    vao = 0U;
    sceneBufferVBO = 0U;
    triangleOutlineEBO = 0U;
    practiceTexture = 0u;


    //Set the starting input primitive type
    currentPrimitiveInputType = PIPELINE_PRIMITIVE_INPUT_TYPE::DISCRETE_TRIANGLES;

    //Set the variables regarding instanced drawing
    drawMultipleInstances = false;
    instanceCount = STARTING_INSTANCE_COUNT;
    
    reportPerformance = false;
    freezeTimeToggle = false;
    reverseTimePropogation = false;
    enableBlending = false;
    enableDepthClamping = false;

    customShaderParameter1 = 0U;
    customShaderParameter2 = 0U;
    customShaderParameter3 = 0U;


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
    zTranslation = 0.0f;

    //Set values for Rotation Uniforms
    head = 0.0f;
    pitch = 0.0f;
    roll = 0.0f;
    //Calculate initial rotation matrix
    rotation = glm::mat4(1.0f);   //Initialize the rotation matrix to 4x4 identity matrix
    //                            //(it will be set to a real rotation matrix later)


    zoom = 1.0f; //Higher number means farther away


     //INITIALIZATION-ONLY -- The values set here have no impact on the actual background color, instead
    backgroundColor = glm::vec3(0.0f, 0.0f, 0.0f); //see the background-color-update function

}



AssetLoadingDemo::AssetLoadingDemo(InitReport* initReport) : RenderDemoBase() {
    
    initialize();

    //Set the initial OpenGL context state (basically consists of calls to glEnable())
    setAssetLoadingDemoSpecificGlobalGLContextState();



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

    if (practiceTexture)
        glDeleteTextures(1, &practiceTexture);
}


void AssetLoadingDemo::run() {
    OPTICK_CATEGORY("PreparingToEnterRenderLoop", Optick::Category::Audio);
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


void AssetLoadingDemo::setAssetLoadingDemoSpecificGlobalGLContextState() const {
    OPTICK_EVENT();

    glEnable(GL_PROGRAM_POINT_SIZE);

    glEnable(GL_DEPTH_CLAMP);
    enableDepthClamping = true;

    //test
    if (glIsEnabled(GL_MULTISAMPLE))
        fprintf(MSGLOG, "\nMULTISAMPLING IS ENABLED!\n");
    else
        fprintf(MSGLOG, "\nMULTISAMPLING IS DISABLED!\n");

    //glDisable(GL_MULTISAMPLE);
}


void AssetLoadingDemo::loadAssets() {
    OPTICK_EVENT();
    try {
        if (!loadShaders()) //load the GLSL shader code
            return;
        loadModels();  //have the GL context load the Teapot vertices to video memory 
        prepareScene();
    }
    catch (const std::system_error& sysErr) {
        error = true;
        fprintf(ERRLOG, "\nCaught System Error [%#08x] while loading assets!\n"
            "  ~~> [ERROR CODE MEANING: %s]\n"
            "\n                                        <<  Error Message  >>\n"
            "%s\n\n",
            sysErr.code().value(),  sysErr.code().message().c_str(),  sysErr.what());
    }
    catch (const std::exception& e) {
        error = true;
        fprintf(ERRLOG, "\nCaught Exception while loading assets!\n"
            "\n                                        <<  Exception Message  >>\n"
            "%s\n\n", e.what());
    }
    catch (...) { //Gotta catch 'em all
        error = true;
        fprintf(ERRLOG, "\nCaught an unidentified exception while loading assets "
            "in AssetLoadingDemo!\n");
    }
}



bool AssetLoadingDemo::loadShaders() { 
    OPTICK_EVENT();
    const std::string shadersRFP = FILEPATH_TO_SHADERS;   //Relative Filepath to location of Shaders

    fprintf(MSGLOG, "\nInitializing Shaders!\n");

    if (!buildQuadTextureTestShader()) {
        fprintf(ERRLOG, "\nError occurred building the Quad Texture Test shader!\n");
        std::exit(EXIT_FAILURE);
    }
    else {
        if (!loadTexture2DFromImageFile()) {
            fprintf(ERRLOG, "\nError loading texture from image file!\n");
            //std::exit(EXIT_FAILURE);
        }
    }

    sceneShader = std::make_unique<ShaderProgram>(); //Create the scene shader


#define USE_RUBYMINE

#ifdef USE_RUBYMINE
    /////////////////////////
    ////    RubyMine Shader   (from the Internet)
    /////////////////////////
    
    //Attach the main shader stages to the sceneShader
    sceneShader->attachVert(shadersRFP + "Sample\\RubyMine.vert"); //Attach Vertex shader to scene
    shaderSources.emplace_back(shadersRFP + "Sample\\RubyMine.vert", true, ShaderInterface::ShaderType::VERTEX);
    sceneShader->attachFrag(shadersRFP + "Sample\\RubyMine.frag"); //Attach Fragment shader to scene
    shaderSources.emplace_back(shadersRFP + "Sample\\RubyMine.frag", true, ShaderInterface::ShaderType::FRAGMENT);
    


#else 

    /////////////////////////
    ////    Normal Shader
    /////////////////////////

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

#endif //ifdef USE_RUBYMINE or NORMAL


    //Now after all the stages to the shader have been created and attached, it is time to link the sceneShader
    sceneShader->link();
    if (sceneShader->checkIfLinked()) {
        fprintf(MSGLOG, "Program Successfully linked!\n");
        fprintf(MSGLOG, "\nAll Shaders Successfully Built!\n");
        return true;
    }
    //else 
    //Hide the window so user can see error message about shader
    GLFWwindow* applicationWindow = glfwGetCurrentContext();
    if (applicationWindow)
        glfwIconifyWindow(applicationWindow);

    fprintf(ERRLOG, "Shader Program was not successfully linked!\n");
    //This next line is to give the user false hope
    fprintf(MSGLOG, "\t[Press 'ENTER' to attempt to continue program execution]\n");
    std::cin.get(); //Hold the mainRenderWindow open if there was an error
    markMainRenderWindowAsReadyToClose(); //Mark window for closing once error is acknowledged

    //Return Window from Iconification?
    if (applicationWindow)
        glfwRestoreWindow(applicationWindow);

    return false;
}


///Random note on differences between traditional and bindless OpenGL APIs. 
///All texture functions in the traditional API use the word 'tex' in their
///name, while the newer bindless API uses the word 'texture' instead.
bool AssetLoadingDemo::loadTexture2DFromImageFile() {
    assert(quadTextureTestShader);

    auto pathToImages = FILEPATH_TO_IMAGES;

    const Timepoint imageLoadStart("Image Load Start!\n");

    //ImageData_UByte testDefaultImage(R"(C:\Users\Forrest\source\repos\OpenGL_GLFW_Project\OpenGL_GLFW_Project\Images\Samples\LandsatTestImages\VolcanicPlateausInArgentina\pasodeindioszm_oli_2018232.jpg)");


    //ImageData_UByte testDefaultImage(R"(C:\Users\Forrest\source\repos\OpenGL_GLFW_Project\OpenGL_GLFW_Project\Images\Samples\LandsatTestImages\EtnaAwakensOnItsSide\etna_oli_2018362_wide.jpg)");

    //ImageData_UByte testDefaultImage(R"(C:\Users\Forrest\source\repos\OpenGL_GLFW_Project\OpenGL_GLFW_Project\Images\Samples\LandsatTestImages\EtnaAwakensOnItsSide\etna_olitir_2018362_lrg.jpg)");


    //ImageData_UByte testDefaultImage(R"(Images\2DTexture\BlockShip_UvMap_albedo.png)");
    //ImageData_UByte testDefaultImage(R"(Images\2DTexture\BlockShip_UvMap_WorldNrmlMap.png)");
    //ImageData_UByte testDefaultImage(R"(Images\2DTexture\BlockShip_UvMap_diffuse.png)");
    //ImageData_UByte testDefaultImage(R"(Images\2DTexture\BlockShip_UvMap_WorldNrmlMap.png)");

    ///ImageData_UByte testDefaultImage(R"(obj\BeveledCube.png)");

    ///ImageData_UByte testDefaultImage(R"(Images\Cubemap\green\green_ft.tga)");

    //ImageData_UByte testDefaultImage(R"(Images\Spaceship03_albedo.png)"); //Thia file no longer exists
    //ImageData_UByte testDefaultImage(R"(Images\Spaceship02_color.png)");

    //ImageData_UByte testDefaultImage(R"(Images\OuterSpaceScreenshots\scr00004.tga)");
    //ImageData_UByte testDefaultImage(R"(Images\OuterSpaceScreenshots\scr00020.jpg)"); 
    //ImageData_UByte testDefaultImage(R"(Images\OuterSpaceScreenshots\scr00022.jpg)"); //THIS ONE IS COOL!
    //ImageData_UByte testDefaultImage(R"(Images\OuterSpaceScreenshots\scr00028.jpg)");
    //ImageData_UByte testDefaultImage(R"(Images\OuterSpaceScreenshots\scr00043.jpg)");
    ImageData_UByte testDefaultImage(R"(Images\OuterSpaceScreenshots\scr00045.jpg)");
    //ImageData_UByte testDefaultImage(R"(Images\OuterSpaceScreenshots\scr00088.jpg)"); //[Dark and Blue]Star On Horizon Of Wide Angle Shot Above Blue Planet 
    //ImageData_UByte testDefaultImage(R"(Images\OuterSpaceScreenshots\scr00111.jpg)");
    //ImageData_UByte testDefaultImage(R"(Images\OuterSpaceScreenshots\scr00163.jpg)"); //Green Planet Surface
    //ImageData_UByte testDefaultImage(R"(Images\OuterSpaceScreenshots\scr00173.jpg)");
    //ImageData_UByte testDefaultImage(R"(Images\OuterSpaceScreenshots\scr00173.jpg)");
    //ImageData_UByte testDefaultImage(R"(Images\OuterSpaceScreenshots\scr00176.jpg)");
    //ImageData_UByte testDefaultImage(R"(Images\OuterSpaceScreenshots\scr00207.jpg)");
    //ImageData_UByte testDefaultImage(R"(Images\OuterSpaceScreenshots\scr00253.jpg)");
    //ImageData_UByte testDefaultImage(R"(obj\2DTexturedQuadPlaneTexture.png)");


    //ImageData_UByte testDefaultImage(R"(Images\Samples\LandsatTestImages\SevernayaZemlyaArchipelago\SevernayaZemlya_map_2018.png)");

    Timepoint imageLoadEnd("Image Load End!\n");

    fprintf(MSGLOG, "\n\nTime to load image: %f seconds\n", imageLoadEnd - imageLoadStart);





    glCreateTextures(GL_TEXTURE_2D, 1, &practiceTexture);
    //Specify Storage To Be Used For The Texture
    glTextureStorage2D(practiceTexture,
                       1,
                       testDefaultImage.internalFormat(),
                       testDefaultImage.width(),
                       testDefaultImage.height());
    glBindTexture(GL_TEXTURE_2D, practiceTexture);





    //           //////////////////////////////////////////////
    //                        Texture Wrap Behavior 
    //           //////////////////////////////////////////////
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);




    //           //////////////////////////////////////////////
    //                         Texture Filtering
    //           //////////////////////////////////////////////

    // Note that Texture Filtering in the sRGB color space may not be sRGB correct.
    // "Generally speaking, all GL 3.x+ hardware will do filtering correctly."
    // Quote from https://www.khronos.org/opengl/wiki/Sampler_Object under the section 
    // titled 'Filtering'

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    //Note that there are also:
    //   -) GL_NEAREST_MIPMAP_NEAREST
    //   -) GL_LINEAR_MIPMAP_NEAREST
    //   -) GL_NEAREST_MIPMAP_LINEAR
    //   -) GL_LINEAR_MIPMAP_LINEAR





    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);



    /*
    glTextureSubImage2D(practiceTexture,
                        0,
                        0,
                        0,
                        testDefaultImage.width(),
                        testDefaultImage.height(),
                        testDefaultImage.externalFormat(),
                        testDefaultImage.dataRepresentation(),
                        testDefaultImage.data());
    */
    //testDefaultImage.swapRedAndBlueChannels();
    testDefaultImage.uploadDataTo2DTexture(practiceTexture);
    return true;

}




void AssetLoadingDemo::loadModels() {
    OPTICK_EVENT();
    fprintf(MSGLOG, "\nAcquiring and parsing Model(s) data from file(s)...\n");

    //[RFP == Relative File Path]
    std::string modelsRFP = FILEPATH_TO_MODELS; //Set string to the executable-relative location of Model Files folder

    //              Initial Scale values for the objects
    //[These should all be 1.0f always to preserve scale between models]
    constexpr float blockThing_QuadsScale = 1.0f;
    constexpr float beveledCubeScale = 1.0f;
    constexpr float blockShipScale = 1.0f;
    constexpr float subdivisionCubeScale = 1.0f;
    constexpr float abstractShapeScale = 1.0f;

    ///////////////////////////////////////////////////////////////////////////////////////////// 
    //                             Load one or more models                                     //  
    ///////////////////////////////////////////////////////////////////////////////////////////// 



    /////////////////////
    //  TEST!
    /////////////
    ///sceneObjects.emplace_back(std::make_unique<QuickObj>(modelsRFP + "Test01_Pos.obj", 1.0f));  //Single triangle test file



    /////////////////////
    //  World Meshes
    /////////////
    std::string worldMeshName("");
    
    //An Irregular Cube Which The Scene Will Take Place Inside Of. Has Some 
    //Primitives Inside The Cube To Keep Things Interesting.
    //worldMeshName = "DemoSceneInsideABox00.obj";

    //A Simple Hemispherical Dome Interior Created By Starting With A Sphere Then
    //Intersecting A Plane Horizontally Through The Middle
    worldMeshName = "SimpleSkyDome_ReExport.obj";


    //A very simple large sphere [may take a bit to load]
    //worldMeshName = "LargeSphere.obj";



    //My First Attempt at a skybox cube 
    //worldMeshName = "AlienWorldSkybox.obj";

    sceneObjects.emplace_back(std::make_unique<QuickObj>(modelsRFP + worldMeshName, 1.0f));



    /////////////////////
    //  Well-Behaved models
    /////////////
    ///sceneObjects.emplace_back(std::make_unique<QuickObj>(modelsRFP + "blockThing_Quads.obj", blockThing_QuadsScale));
    ///sceneObjects.emplace_back(std::make_unique<QuickObj>(modelsRFP + "BeveledCube.obj", beveledCubeScale));
    ///sceneObjects.emplace_back(std::make_unique<QuickObj>(modelsRFP + "BlockshipSampleExports\\BlockShipSample_01_3DCoatExport01.obj", blockShipScale, true, true, 0.3f, 0.4f));
    
     ///sceneObjects.emplace_back(std::make_unique<QuickObj>(modelsRFP + "BlockShip_UvMap.obj", blockShipScale));
    //sceneObjects.emplace_back(std::make_unique<QuickObj>(modelsRFP + "SubdivisionCube.obj", subdivisionCubeScale)); //Has no text coords
    //sceneObjects.emplace_back(std::make_unique<QuickObj>(modelsRFP + "AbstractShape.obj", abstractShapeScale)); //Only position data
    
    //sceneObjects.emplace_back(std::make_unique<QuickObj>(modelsRFP + "AbstractShapeDecimated.obj", abstractShapeScale));

    //sceneObjects.emplace_back(std::make_unique<QuickObj>(modelsRFP + "NewOrderTie_Triangulated.obj", 1.0f));

    //sceneObjects.emplace_back(std::make_unique<QuickObj>(modelsRFP + "CargoSpaceshipIdeaThing02.obj", 1.0f));

    ///for (int i = 0; i < 3; i++) 
        sceneObjects.emplace_back(std::make_unique<QuickObj>(modelsRFP + "Spaceship.obj", 1.0f));
    ///sceneObjects.emplace_back(std::make_unique<QuickObj>(modelsRFP + "Interceptor00.obj", 1.0f));
    ///sceneObjects.emplace_back(std::make_unique<QuickObj>(modelsRFP + "thing.obj", 1.0f));  
    ///sceneObjects.emplace_back(std::make_unique<QuickObj>(modelsRFP + "ExperimentalEngine.obj", 1.0f));

    //sceneObjects.emplace_back(std::make_unique<QuickObj>(modelsRFP + "RockThing.obj", 1.0f));

    ///sceneObjects.emplace_back(std::make_unique<QuickObj>(modelsRFP + "ViperMKIV_Fighter.obj", 1.0f));

    sceneObjects.emplace_back(std::make_unique<QuickObj>(modelsRFP + "DrillThing00.obj", 1.0));

    ///sceneObjects.emplace_back(std::make_unique<QuickObj>(modelsRFP + "Spaceship.obj", 1.0f));
    /// sceneObjects.emplace_back(std::make_unique<QuickObj>(modelsRFP + "Spaceship.obj", 1.0f));

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
    ///sceneObjects.emplace_back(std::make_unique<QuickObj>(modelsRFP + "spiral.obj", 1.0f)); // denominator ));
    //    */
    //}

    //sceneObjects.emplace_back(std::make_unique<QuickObj>(modelsRFP + "2DTexturedQuadPlane.obj", 1.0f));

    //Several different objects were given a parent-child relationship in Blender and then saved into the same file
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
    OPTICK_CATEGORY("All Data Loaded Into Memory", Optick::Category::GPU_VFX);
    OPTICK_EVENT();
    fprintf(MSGLOG, "\nCreating the primary scene from loaded assets...\n");
    buildSceneBufferFromLoadedSceneObjects();
    generateTriangleOutlineElementOrdering();
    fprintf(MSGLOG, "Primary scene creation complete!\n\n");

    createSceneVBO();
    createTriangleOutlineEBO();

    fprintf(MSGLOG, "Uploading scene buffer to GPU...\n");
    uploadSceneBufferToGPU(sceneBufferVBO, sceneBuffer);
    configureVertexArrayAttributes(); 
    uploadTriangleOutlineElementOrderingBufferToGPU(triangleOutlineEBO,
                                                    triangleOutlineElementOrdering);
}


//  /////////////////////////////////////////////////////////////////////////////////  //
//                /////////////////////////////////////////////////////                //
//                ///////////      The Render Loop      ///////////////                //
//                /////////////////////////////////////////////////////                //
//  /////////////////////////////////////////////////////////////////////////////////  //
void AssetLoadingDemo::renderLoop() {
  
    //Call this function to initialize the frame performance profiler object
    framePerformance.prepareToEnterRenderLoop();

    assert(mainRenderWindow);

    //Make sure we still have a context
    if ((glfwGetCurrentContext() != mainRenderWindow)) {
        fprintf(ERRLOG,
            "\n\n\t\t\t!!!!\t[ERROR OCCURED]\t!!!!\a\n"
            "\tCause: Window Lost Graphics Context / Context Is ");
        return;
    }

    while (glfwWindowShouldClose(mainRenderWindow) == GLFW_FALSE) {

       
        //------------------------------------------------------
        //             OPTICK's PER-FRAME FUNCTION              
        //   !!!!!!!!!!!!!!!!                !!!!!!!!!!!!!!!!   
        //           !!!!!!!!  [ WARNING! ]  !!!!!!!!           
        //   !!!!!!!!!!!!!!!!                !!!!!!!!!!!!!!!!   
        //                                                      
        //  This Macro Introduces A Variable Of Type 'uint32_t' 
        //  Named 'frameNumber' Into The Current Scope.         
        //                                                      
        //   This Can Cause A Lot Of Problems If Called From    
        //   Within A Member Function Of A Class Which Also     
        //   Just So Happens To Define A Field Variable With    
        //   The Exact Same Name (Don't Ask Me How I Know Or How
        //   Long It Took Me To Figure This One Out) (Alright   
        //    fine it actually only took around 3 hours)        
        OPTICK_FRAME("MainThread");                             
        //------------------------------------------------------

        recordFrameStartTimepoint();

        //        ---------------------------------------------------------------------        //
        //        * * * *                  +---------------+                    * * * *        //
        //        * * * *                  |  INPUT PHASE  |                    * * * *        //
        //        * * * *                  +---------------+                    * * * *        //
        //        ---------------------------------------------------------------------        //
        ////////////////////////
        //Check Input
        ////////////////////////
        detectInput();

        //        ---------------------------------------------------------------------        //
        //        * * * *                  +---------------+                    * * * *        //
        //        * * * *                  |  LOGIC PHASE  |                    * * * *        //
        //        * * * *                  +---------------+                    * * * *        //
        //        ---------------------------------------------------------------------        //
        ////////////////////////
        //Perform logic 
        ////////////////////////
        computeLogic();


        recordBeginRenderCommandsTimepoint();

        //        ---------------------------------------------------------------------        //
        //        * * * *                  +--------------+                     * * * *        //
        //        * * * *                  |  DRAW PHASE  |                     * * * *        //
        //        * * * *                  +--------------+                     * * * *        //
        //        ---------------------------------------------------------------------        //
        ///////////////////////////
        //Draw frame
        //////////////////////////
        renderScene();
        


        recordSwapFramebuffersTimepoint();

        //        ---------------------------------------------------------------------        //
        //        * * * *                  +---------------+                    * * * *        //
        //        * * * *                  | PRESENT PHASE |                    * * * *        //
        //        * * * *                  +---------------+                    * * * *        //
        //        ---------------------------------------------------------------------        //
        presentFrame();
        
        
    }

}


void AssetLoadingDemo::recordFrameStartTimepoint() {
    //Record timepoint for frame start 
    framePerformance.recordLoopStartTimepoint(frameCounter);
}

void AssetLoadingDemo::recordBeginRenderCommandsTimepoint() {
    framePerformance.recordBeginDrawCommandsTimepoint();
}

void AssetLoadingDemo::recordSwapFramebuffersTimepoint() {
    framePerformance.recordReadyToFlipBuffersTimepoint();
}

void AssetLoadingDemo::detectInput() {
    OPTICK_EVENT();
    
    //   ***\______+============+______/***
    //       ______|  KEYBOARD  |______
    //   ***/      +============+      \***
    //Cap the number of times keyboard input can be reprocessed
    static constexpr const int MAX_ITERATIONS = 150;
    int iterations = 0;
    //Some keyboard inputs may require for all keyboard inputs
    //detected thus far to be invalid and thus requiring all 
    //keyboard input for the current frame to be processed again.
    do {
        iterations++;

        //TODO This Will Crash Application When Escape Is Held Down, Need To Rewrite
        if (iterations >= MAX_ITERATIONS) {
            break;
        }
    } while (!checkKeyboardInput());

    //   ***\______+============+______/***
    //       ______|  JOYSTICK  |______
    //   ***/      +============+      \***
    checkControllerInput();
}



//   ***\______+============+______/***
//       ______|  KEYBOARD  |______
//   ***/      +============+      \***
bool AssetLoadingDemo::checkKeyboardInput() {
    OPTICK_EVENT();
    if (checkToSeeIfShouldCloseWindow()) {
        markMainRenderWindowAsReadyToClose();
        return false; 
    }
    reportPerformance = checkIfShouldTogglePerformanceReporting();
    if (checkIfShouldPause()) {
        pause();
        return false;
    }
    if (checkIfShouldReset()) {
        if (glfwGetKey(mainRenderWindow, GLFW_KEY_SPACE) == GLFW_PRESS) {
            pause();
            return false;
        }
        reset();
    }
    if (checkIfShouldFreezeTime()) { //'checkIfShouldFreezeTime()' relies on 'toggleFreezeTime()' to 
        toggleTimeFreeze(); //         update member field 'frameTimeFreezeLastToggled' to value of 'frameNumber' 
    }

    if (checkIfShouldReverseDirectionOfTime())
        reverseTime();

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

    //CUSTOM SHADER PARAMETERS
    if (checkIfShouldIncreaseCustomShaderParameter1())
        increaseCustomShaderParameter1();
    //if (checkIfShouldResetCustomShaderParameter1())
    //    resetCustomShaderParameter1();
    if (checkIfShouldIncreaseCustomShaderParameter2())
        increaseCustomShaderParameter2();
    //if (checkIfShouldResetCustomShaderParameter2())
    //    resetCustomShaderParameter2();
    if (checkIfShouldIncreaseCustomShaderParameter3())
        increaseCustomShaderParameter3();
    //if (checkIfShouldResetCustomShaderParameter3())
    //    resetCustomShaderParameter3();
    if (checkIfShouldResetCustomShaderParameters()) {
        resetCustomShaderParameter1();
        resetCustomShaderParameter2();
        resetCustomShaderParameter3();
    }

    //More Input Checking
    changePrimitiveType();
    changeInstancedDrawingBehavior(); //Toggle on/off drawing instances
    rotate();
    changeZoom();
    translate();
}

//   ***\______+============+______/***
//       ______|  JOYSTICK  |______
//   ***/      +============+      \***
void AssetLoadingDemo::checkControllerInput() {
    OPTICK_EVENT();
    readJoystick0State_AssumingXInput_AndThenProcessAllInput();
}

inline bool AssetLoadingDemo::checkToSeeIfShouldCloseWindow() const  noexcept {
    if (glfwGetKey(mainRenderWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        return true;
    }
    return false;
}

inline bool AssetLoadingDemo::checkIfShouldTogglePerformanceReporting() const noexcept {
    if ((framePerformanceReportingLastToggled + FRAMES_TO_WAIT_BETWEEN_INPUT_READS) > frameCounter)
        return reportPerformance;
    bool performanceReportingState = reportPerformance;
    if ((glfwGetKey(mainRenderWindow, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) ||
        (glfwGetKey(mainRenderWindow, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS)) {
        if (glfwGetKey(mainRenderWindow, GLFW_KEY_S) == GLFW_PRESS) {
            performanceReportingState = !performanceReportingState;
            fprintf(MSGLOG, "Performance Reporting %s\n", performanceReportingState ?
                "Enabled" : "Disabled");
            framePerformanceReportingLastToggled = frameCounter;
            return performanceReportingState;
        }
    }
    return performanceReportingState;
}

inline bool AssetLoadingDemo::checkIfShouldPause() const noexcept {
    if ((frameCounter >= (frameUnpaused + DELAY_LENGTH_OF_PAUSE_CHECKING_AFTER_UNPAUSE))
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
    if ((frameCounter - frameTimeFreezeLastToggled) > FRAMES_TO_WAIT_BETWEEN_INPUT_READS) {
        if (glfwGetKey(mainRenderWindow, GLFW_KEY_T) == GLFW_PRESS)
            return true;
    }
    return false;
}

inline bool AssetLoadingDemo::checkIfShouldReverseDirectionOfTime() const noexcept {
   
    if ((frameCounter - frameThatTimePropogationWasLastReversed) > FRAMES_TO_WAIT_BETWEEN_INPUT_READS) {
        if (glfwGetKey(mainRenderWindow, GLFW_KEY_G) == GLFW_PRESS) 
            return true;
    }
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
    if ((frameCounter - frameBlendOperationLastToggled) > FRAMES_TO_WAIT_BETWEEN_INPUT_READS) {
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
    if ((frameCounter - frameDepthClampLastToggled) > FRAMES_TO_WAIT_BETWEEN_INPUT_READS) {
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



inline bool AssetLoadingDemo::checkIfShouldIncreaseCustomShaderParameter1() const noexcept {
    if ((frameCounter - frameThatCustomShaderParameter1LastModified) > FRAMES_TO_WAIT_BETWEEN_INPUT_READS) {
        if (glfwGetKey(mainRenderWindow, GLFW_KEY_5) == GLFW_PRESS) {
            return true;
        }
    }
    return false;
}

inline bool AssetLoadingDemo::checkIfShouldIncreaseCustomShaderParameter2() const noexcept {
    if ((frameCounter - frameThatCustomShaderParameter2LastModified) > FRAMES_TO_WAIT_BETWEEN_INPUT_READS) {
        if (glfwGetKey(mainRenderWindow, GLFW_KEY_6) == GLFW_PRESS) {
            return true;
        }
    }
    return false;
}

inline bool AssetLoadingDemo::checkIfShouldIncreaseCustomShaderParameter3() const noexcept {
    if ((frameCounter - frameThatCustomShaderParameter3LastModified) > FRAMES_TO_WAIT_BETWEEN_INPUT_READS) {
        if (glfwGetKey(mainRenderWindow, GLFW_KEY_7) == GLFW_PRESS) {
            return true;
        }
    }
    return false;
}


//-----------------
//  It turns out GLFW will only acknowledge 'press' input from each key only once, so what I was 
//  trying to do here with these following functions each reacting to the same key press input by 
//  each detecting it separately does not work. Instead they are all reset by the function 
//    'checkIfShouldResetCustomShaderParameters()'
//-----------------
//inline bool AssetLoadingDemo::checkIfShouldResetCustomShaderParameter1() const noexcept {
//    const unsigned long long frameThatMostRecentCustomShaderParameterUpdateOccurred = std::max({ frameThatCustomShaderParameter1LastModified, frameThatCustomShaderParameter2LastModified, frameThatCustomShaderParameter3LastModified });
//    if ((frameNumber - frameThatMostRecentCustomShaderParameterUpdateOccurred) > FRAMES_TO_WAIT_BETWEEN_INPUT_READS) {
//        if (glfwGetKey(mainRenderWindow, GLFW_KEY_8) == GLFW_PRESS) {
//            return true;
//        }
//    }
//    return false;
//}
//
//inline bool AssetLoadingDemo::checkIfShouldResetCustomShaderParameter2() const noexcept {
//    const unsigned long long frameThatMostRecentCustomShaderParameterUpdateOccurred = std::max({ frameThatCustomShaderParameter1LastModified, frameThatCustomShaderParameter2LastModified, frameThatCustomShaderParameter3LastModified });
//    if ((frameNumber - frameThatMostRecentCustomShaderParameterUpdateOccurred) > FRAMES_TO_WAIT_BETWEEN_INPUT_READS) {
//        if (glfwGetKey(mainRenderWindow, GLFW_KEY_8) == GLFW_PRESS) {
//            return true;
//            printf("Returning True for # 2!\n");
//        }
//    }
//    return false;
//}
//
//inline bool AssetLoadingDemo::checkIfShouldResetCustomShaderParameter3() const noexcept {
//    const unsigned long long frameThatMostRecentCustomShaderParameterUpdateOccurred = std::max({ frameThatCustomShaderParameter1LastModified, frameThatCustomShaderParameter2LastModified, frameThatCustomShaderParameter3LastModified });
//    if ((frameNumber - frameThatMostRecentCustomShaderParameterUpdateOccurred) > FRAMES_TO_WAIT_BETWEEN_INPUT_READS) {
//        if (glfwGetKey(mainRenderWindow, GLFW_KEY_8) == GLFW_PRESS) {
//            return true;
//        }
//    }
//    return false;
//}

inline bool AssetLoadingDemo::checkIfShouldResetCustomShaderParameters() const noexcept {
    const uint64_t frameThatMostRecentCustomShaderParameterUpdateOccurred = std::max({ frameThatCustomShaderParameter1LastModified, frameThatCustomShaderParameter2LastModified, frameThatCustomShaderParameter3LastModified });
    if ((frameCounter - frameThatMostRecentCustomShaderParameterUpdateOccurred) > FRAMES_TO_WAIT_BETWEEN_INPUT_READS) {
        if (glfwGetKey(mainRenderWindow, GLFW_KEY_8) == GLFW_PRESS) {
            return true;
        }
    }
    return false;
}



void AssetLoadingDemo::pause() {
    OPTICK_EVENT();
    const auto begin = std::chrono::high_resolution_clock::now(); //Time measurement
    auto end = std::chrono::high_resolution_clock::now();
    fprintf(MSGLOG, "PAUSED!\n");
    //Upon first pausing, enter into the following loop for a short period of time before moving on to
    //the full pause loop. This will prevent unpausing from occurring directly after a pause is initiated. 
    while (std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count() < 300000000LL) {
        std::this_thread::sleep_for(std::chrono::nanoseconds(2000000LL));
        end = std::chrono::high_resolution_clock::now();
    }

    //Enter an infinite loop checking for the unpause key (or exit key) to be pressed
    while (true) {
        glfwPollEvents();
        if (glfwGetKey(mainRenderWindow, GLFW_KEY_SPACE) == GLFW_PRESS) {
            frameUnpaused = frameCounter;
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
    OPTICK_EVENT();
    fprintf(MSGLOG, "\nReseting Demo...\n");
    counter = 0.0f; //Reset time to 0
    timeTickRateModifier = 0.0f;
    fov = CAMERA_DEFAULT_FOV;
    head = 0.0f; //Reset rotation
    pitch = 0.0f;
    roll = 0.0f;
    cameraPos = CAMERA_POSITION;
    xTranslation = 0.0f;
    yTranslation = 0.0f;
    zTranslation = 0.0f;
    backgroundColor = glm::vec3(0.15f, 0.5f, 0.75f);
    frameCounter = 0ull;
    frameUnpaused = 0ull;
    frameLineTypeLastSwitched = 0ull;
    frameInstancedDrawingBehaviorLastToggled = 0ull;
    frameInstancedDrawingCountLastModified = 0ull;
    frameTimeFreezeLastToggled = 0ull;
    frameBlendOperationLastToggled = 0ull;
    frameDepthClampLastToggled = 0ull;
    frameThatTimePropogationWasLastReversed = 0ULL;
    frameThatCustomShaderParameter1LastModified = 0ULL;
    frameThatCustomShaderParameter2LastModified = 0ULL;
    frameThatCustomShaderParameter3LastModified = 0ULL;
    framePerformanceReportingLastToggled = 0ULL;
    zoom = 1.0f;
    if (drawMultipleInstances) 
        instanceCount = STARTING_INSTANCE_COUNT;
    
    
    recomputeProjectionMatrix();
}

void AssetLoadingDemo::toggleTimeFreeze() noexcept {
    OPTICK_EVENT();
    //Note that it is vitally important that 'frameTimeFreezeLastToggled' is updated to match the current 'frameNumber'
    frameTimeFreezeLastToggled = frameCounter;
    freezeTimeToggle = !freezeTimeToggle;
    if (freezeTimeToggle) 
        fprintf(MSGLOG, "Time Frozen!\n");
    
    else 
        fprintf(MSGLOG, "Time Unfrozen!\n");
    
}

void AssetLoadingDemo::reverseTime() noexcept {
    OPTICK_EVENT();
    frameThatTimePropogationWasLastReversed = frameCounter;
    reverseTimePropogation = !reverseTimePropogation;
    fprintf(MSGLOG, "Time is now propagating %s\n",
        reverseTimePropogation ? "Backwards" : "Forwards");
}

void AssetLoadingDemo::increasePassageOfTime() noexcept {
    OPTICK_EVENT();
    timeTickRateModifier += (0.005f * getShiftBoost());
    static auto frameUpdateMessageWasLastPrinted = frameCounter;
    if (frameCounter < frameUpdateMessageWasLastPrinted) //
        frameUpdateMessageWasLastPrinted = frameCounter;
    else if (frameCounter >= (15ull + frameUpdateMessageWasLastPrinted))
        frameUpdateMessageWasLastPrinted = frameCounter;
    else
        return;
    
    float delta = (1.0f + timeTickRateModifier) * 100.0f;
    if (reverseTimePropogation)
        delta *= -1.0f;
    fprintf(MSGLOG, "Time now operating at %%%f speed\n", delta);
}

void AssetLoadingDemo::decreasePassageToTime() noexcept {
    OPTICK_EVENT();
    timeTickRateModifier -= (0.005f * getShiftBoost());
    static auto frameUpdateMessageWasLastPrinted = frameCounter;
    if (frameCounter < frameUpdateMessageWasLastPrinted) 
        frameUpdateMessageWasLastPrinted = frameCounter;
    else if (frameCounter >= (15ull + frameUpdateMessageWasLastPrinted))
        frameUpdateMessageWasLastPrinted = frameCounter;
    else
        return;
    float delta = (1.0f + timeTickRateModifier) * 100.0f;
    if (reverseTimePropogation)
        delta *= -1.0f;
    fprintf(MSGLOG, "Time now operating at %%%f speed\n", delta);
}


void AssetLoadingDemo::toggleBlending() noexcept {
    OPTICK_EVENT();
    //Note that it is vitally important that 'frameBlendOperationLastToggled' is updated to match the current 'frameNumber'
    frameBlendOperationLastToggled = frameCounter;

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
    OPTICK_EVENT();
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
    frameDepthClampLastToggled = frameCounter;
}

void AssetLoadingDemo::updateFieldOfView() noexcept {
    OPTICK_EVENT();
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
    OPTICK_EVENT();
    view = glm::lookAt(cameraPos, lookAtOrgin, upDirection);

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

    //Safety first [Prevent a divide by zero]
    if (fabsf(screenWidth) < 0.000001f)
        return;
    if (fabsf(screenHeight) < 0.000001f)
        return;

    //Compute the screen -projection matrix
    //perspective = glm::mat4(1.0f); //Set projection matrix to 4x4 identity
    //see: https://gamedev.stackexchange.com/questions/98226/how-can-i-set-up-an-intuitive-perspective-projection-view-matrix-combination-in
    perspective = glm::perspectiveFov(fov, screenWidth, screenHeight, zNear, zFar );

}


void AssetLoadingDemo::changePrimitiveType() {
    OPTICK_EVENT();
    static PIPELINE_PRIMITIVE_INPUT_TYPE previousPrimitiveInputType = currentPrimitiveInputType;

    if (glfwGetKey(mainRenderWindow, GLFW_KEY_1) == GLFW_PRESS)
        currentPrimitiveInputType = PIPELINE_PRIMITIVE_INPUT_TYPE::DISCRETE_TRIANGLES;

    else if (glfwGetKey(mainRenderWindow, GLFW_KEY_2) == GLFW_PRESS)
        currentPrimitiveInputType = PIPELINE_PRIMITIVE_INPUT_TYPE::TRIANGLE_STRIP;

    else if (glfwGetKey(mainRenderWindow, GLFW_KEY_3) == GLFW_PRESS)
        currentPrimitiveInputType = PIPELINE_PRIMITIVE_INPUT_TYPE::TRIANGLE_FAN;

    else if (glfwGetKey(mainRenderWindow, GLFW_KEY_4) == GLFW_PRESS)
        currentPrimitiveInputType = PIPELINE_PRIMITIVE_INPUT_TYPE::POINTS;
    
    //Pressing the '`' key will toggle between the 3 options for drawing line primitives
    if (glfwGetKey(mainRenderWindow, GLFW_KEY_GRAVE_ACCENT) == GLFW_PRESS) {
        if ((frameCounter - frameLineTypeLastSwitched) < FRAMES_TO_WAIT_BETWEEN_INPUT_READS) {
            frameLineTypeLastSwitched = frameCounter;
        }
        else {
            frameLineTypeLastSwitched = frameCounter;
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
    OPTICK_EVENT();
    if ((frameCounter - frameInstancedDrawingBehaviorLastToggled) > FRAMES_TO_WAIT_BETWEEN_INPUT_READS) {
        if (glfwGetKey(mainRenderWindow, GLFW_KEY_I) == GLFW_PRESS) {
            frameInstancedDrawingBehaviorLastToggled = frameCounter; //Mark the current frame as being the one when instanced drawing behavior was last toggled
            drawMultipleInstances = !drawMultipleInstances; //Perform Toggle
            fprintf(MSGLOG, "Instanced Rendering set to: %s",
                (drawMultipleInstances ? ("Enabled\n") : ("Disabled\n")));
        }
    }

    //Only allow the instance count to be modified every 10 frames (1/6th second)
    if ((frameCounter - frameInstancedDrawingCountLastModified) > 10ull) {
        if (drawMultipleInstances) {
            if (glfwGetKey(mainRenderWindow, GLFW_KEY_EQUAL) == GLFW_PRESS) {
                frameInstancedDrawingBehaviorLastToggled = frameCounter; 
                instanceCount += static_cast<GLsizei>(1.0f * getShiftBoost());
                fprintf(MSGLOG, "Rendered Instances increased to: %d\n", instanceCount);
            }
            else if (glfwGetKey(mainRenderWindow, GLFW_KEY_MINUS) == GLFW_PRESS) {
                if (instanceCount > 0u) { //Don't decrement unsigned value below 0
                    frameInstancedDrawingBehaviorLastToggled = frameCounter;
                    instanceCount -= static_cast<GLsizei>(1.0f * getShiftBoost());
                    fprintf(MSGLOG, "Rendered Instances decreased to: %d\n", instanceCount);
                }
            }
        }
    }

    if (instanceCount < 0)
        instanceCount = 0;
}


void AssetLoadingDemo::increaseCustomShaderParameter1() noexcept {
    customShaderParameter1++;
    frameThatCustomShaderParameter1LastModified = frameCounter;
    fprintf(MSGLOG, "Custom Shader Parameter 1 is now %u\n", customShaderParameter1);
}

void AssetLoadingDemo::increaseCustomShaderParameter2() noexcept {
    customShaderParameter2++;
    frameThatCustomShaderParameter2LastModified = frameCounter;
    fprintf(MSGLOG, "Custom Shader Parameter 2 is now %u\n", customShaderParameter2);
}

void AssetLoadingDemo::increaseCustomShaderParameter3() noexcept {
    customShaderParameter3++;
    frameThatCustomShaderParameter3LastModified = frameCounter;
    fprintf(MSGLOG, "Custom Shader Parameter 3 is now %u\n", customShaderParameter3);
}

void AssetLoadingDemo::resetCustomShaderParameter1() noexcept {
    customShaderParameter1 = 0U;
    frameThatCustomShaderParameter1LastModified = frameCounter;
    fprintf(MSGLOG, "Custom Shader Parameter 1 is now %u\n", customShaderParameter1);
}

void AssetLoadingDemo::resetCustomShaderParameter2() noexcept {
    customShaderParameter2 = 0U;
    frameThatCustomShaderParameter2LastModified = frameCounter;
    fprintf(MSGLOG, "Custom Shader Parameter 2 is now %u\n", customShaderParameter2);
}

void AssetLoadingDemo::resetCustomShaderParameter3() noexcept {
    customShaderParameter3 = 0U;
    frameThatCustomShaderParameter3LastModified = frameCounter;
    fprintf(MSGLOG, "Custom Shader Parameter 3 is now %u\n", customShaderParameter3);
}


void AssetLoadingDemo::rotate() noexcept {
    OPTICK_EVENT();
    //The Following Constants determine input sensitivity/reactivity

    // Pitch 
    static constexpr const float PITCH_DELTA_BASE = 0.045f;
    static constexpr const float PITCH_DELTA_LSHIFT = 0.085f;
    static constexpr const float PITCH_DELTA_RSHIFT = 0.125f;
    static constexpr const float PITCH_DELTA_LRSHIFT = 0.2f;
    // Roll
    static constexpr const float ROLL_DELTA_BASE = 0.045f;
    static constexpr const float ROLL_DELTA_LSHIFT = 0.085f;
    static constexpr const float ROLL_DELTA_RSHIFT = 0.125f;
    static constexpr const float ROLL_DELTA_LRSHIFT = 0.2f;
    // Head/Yaw
    static constexpr const float HEAD_DELTA_BASE = 0.045f;
    static constexpr const float HEAD_DELTA_LSHIFT = 0.085f;
    static constexpr const float HEAD_DELTA_RSHIFT = 0.125f;
    static constexpr const float HEAD_DELTA_LRSHIFT = 0.2f;
    
    //Reduce queries to the Renderwindow by polling state of
    //left and right shift keys only once
    static constexpr const unsigned int NEITHERSHIFTPRESSED = 0u;
    static constexpr const unsigned int LSHIFTVAL = 1u;
    static constexpr const unsigned int RSHIFTVAL = 2u;
    int lrShiftState = NEITHERSHIFTPRESSED;
    if (glfwGetKey(mainRenderWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        lrShiftState += LSHIFTVAL;
    if (glfwGetKey(mainRenderWindow, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)
        lrShiftState += RSHIFTVAL;


    if (NEITHERSHIFTPRESSED == lrShiftState) {
        if (glfwGetKey(mainRenderWindow, GLFW_KEY_UP) == GLFW_PRESS)
            pitch += PITCH_DELTA_BASE;
        if (glfwGetKey(mainRenderWindow, GLFW_KEY_DOWN) == GLFW_PRESS)
            pitch -= PITCH_DELTA_BASE;
        if (glfwGetKey(mainRenderWindow, GLFW_KEY_LEFT) == GLFW_PRESS)
            roll += ROLL_DELTA_BASE;
        if (glfwGetKey(mainRenderWindow, GLFW_KEY_RIGHT) == GLFW_PRESS)
            roll -= ROLL_DELTA_BASE;
        if (glfwGetKey(mainRenderWindow, GLFW_KEY_Q) == GLFW_PRESS)
            head += HEAD_DELTA_BASE;
        if (glfwGetKey(mainRenderWindow, GLFW_KEY_E) == GLFW_PRESS)
            head -= HEAD_DELTA_BASE;
    }
    else if (LSHIFTVAL == lrShiftState) {
        if (glfwGetKey(mainRenderWindow, GLFW_KEY_UP) == GLFW_PRESS)
            pitch += PITCH_DELTA_LSHIFT;
        if (glfwGetKey(mainRenderWindow, GLFW_KEY_DOWN) == GLFW_PRESS)
            pitch -= PITCH_DELTA_LSHIFT;
        if (glfwGetKey(mainRenderWindow, GLFW_KEY_LEFT) == GLFW_PRESS)
            roll += ROLL_DELTA_LSHIFT;
        if (glfwGetKey(mainRenderWindow, GLFW_KEY_RIGHT) == GLFW_PRESS)
            roll -= ROLL_DELTA_LSHIFT;
        if (glfwGetKey(mainRenderWindow, GLFW_KEY_Q) == GLFW_PRESS)
            head += HEAD_DELTA_LSHIFT;
        if (glfwGetKey(mainRenderWindow, GLFW_KEY_E) == GLFW_PRESS)
            head -= HEAD_DELTA_LSHIFT;
    }
    else if (RSHIFTVAL == lrShiftState) {
        if (glfwGetKey(mainRenderWindow, GLFW_KEY_UP) == GLFW_PRESS)
            pitch += PITCH_DELTA_RSHIFT;
        if (glfwGetKey(mainRenderWindow, GLFW_KEY_DOWN) == GLFW_PRESS)
            pitch -= PITCH_DELTA_RSHIFT;
        if (glfwGetKey(mainRenderWindow, GLFW_KEY_LEFT) == GLFW_PRESS)
            roll += ROLL_DELTA_RSHIFT;
        if (glfwGetKey(mainRenderWindow, GLFW_KEY_RIGHT) == GLFW_PRESS)
            roll -= ROLL_DELTA_RSHIFT;
        if (glfwGetKey(mainRenderWindow, GLFW_KEY_Q) == GLFW_PRESS)
            head += HEAD_DELTA_RSHIFT;
        if (glfwGetKey(mainRenderWindow, GLFW_KEY_E) == GLFW_PRESS)
            head -= HEAD_DELTA_RSHIFT;
    }
    else {
        if (glfwGetKey(mainRenderWindow, GLFW_KEY_UP) == GLFW_PRESS)
            pitch += PITCH_DELTA_LRSHIFT;
        if (glfwGetKey(mainRenderWindow, GLFW_KEY_DOWN) == GLFW_PRESS)
            pitch -= PITCH_DELTA_LRSHIFT;
        if (glfwGetKey(mainRenderWindow, GLFW_KEY_LEFT) == GLFW_PRESS)
            roll += ROLL_DELTA_LRSHIFT;
        if (glfwGetKey(mainRenderWindow, GLFW_KEY_RIGHT) == GLFW_PRESS)
            roll -= ROLL_DELTA_LRSHIFT;
        if (glfwGetKey(mainRenderWindow, GLFW_KEY_Q) == GLFW_PRESS)
            head += HEAD_DELTA_LRSHIFT;
        if (glfwGetKey(mainRenderWindow, GLFW_KEY_E) == GLFW_PRESS)
            head -= HEAD_DELTA_LRSHIFT;	
    }
}

void AssetLoadingDemo::changeZoom() noexcept {
    OPTICK_EVENT();
    // Zoom Input Sensitivities
    static constexpr const float ZOOM_DELTA_BASE = 0.025f;
    static constexpr const float ZOOM_DELTA_LSHIFT = 0.095f;
    static constexpr const float ZOOM_DELTA_RSHIFT = 0.125f;
    static constexpr const float ZOOM_DELTA_LRSHIFT = 0.25f;

    static constexpr const unsigned int NEITHERSHIFTPRESSED = 0u;
    static constexpr const unsigned int LSHIFTVAL = 1u;
    static constexpr const unsigned int RSHIFTVAL = 2u;

    int lrShiftState = NEITHERSHIFTPRESSED;
    
    if (NEITHERSHIFTPRESSED == lrShiftState) {
        if (glfwGetKey(mainRenderWindow, GLFW_KEY_Z) == GLFW_PRESS)
            zoom += ZOOM_DELTA_BASE;
        if (glfwGetKey(mainRenderWindow, GLFW_KEY_X) == GLFW_PRESS)
            zoom -= ZOOM_DELTA_BASE;
    }
    else if (LSHIFTVAL == lrShiftState) {
        if (glfwGetKey(mainRenderWindow, GLFW_KEY_Z) == GLFW_PRESS)
            zoom += ZOOM_DELTA_LSHIFT;
        if (glfwGetKey(mainRenderWindow, GLFW_KEY_X) == GLFW_PRESS)
            zoom -= ZOOM_DELTA_LSHIFT;
    }
    else if (RSHIFTVAL == lrShiftState) {
        if (glfwGetKey(mainRenderWindow, GLFW_KEY_Z) == GLFW_PRESS)
            zoom += ZOOM_DELTA_RSHIFT;
        if (glfwGetKey(mainRenderWindow, GLFW_KEY_X) == GLFW_PRESS)
            zoom -= ZOOM_DELTA_RSHIFT;
    }
    else {
        if (glfwGetKey(mainRenderWindow, GLFW_KEY_Z) == GLFW_PRESS)
            zoom += ZOOM_DELTA_LRSHIFT;
        if (glfwGetKey(mainRenderWindow, GLFW_KEY_X) == GLFW_PRESS)
            zoom -= ZOOM_DELTA_LRSHIFT;
    }

    //Clamp zoom to prevent from growing too close to 0.0f
    if (zoom < 0.2f)
        zoom = 0.2f;
}


void AssetLoadingDemo::translate() noexcept {
    OPTICK_EVENT();
    float turbo = 1.0f;
    const float xSpeed = 0.1f;
    const float ySpeed = 0.1f;
    const float zSpeed = 0.08f;
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

    //Forward and backwards
    if (glfwGetKey(mainRenderWindow, GLFW_KEY_KP_ADD) == GLFW_PRESS) {
        zTranslation += turbo * zSpeed;
    }
    if (glfwGetKey(mainRenderWindow, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS) {
        zTranslation -= turbo * zSpeed;
    }

}



void AssetLoadingDemo::computeLogic() {
    OPTICK_EVENT();

    performRenderDemoSharedInputLogic(); //This is the loop function of the base class

    //It is required to call 'reportStatistics()' periodically regardless of whether we are 
    //printing performance statics or not so that the recorded frame Timepoint samples 
    //are periodically flushed. 
    if ((frameCounter % FRAMES_BETWEEN_PERFORMANCE_REPORT) == 0ULL) {
        reportStatistics();
    }

    if ((frameCounter % FRAMES_TO_WAIT_BEFORE_CHECKING_TO_UPDATE_SHADERS) ==
        (FRAMES_TO_WAIT_BEFORE_CHECKING_TO_UPDATE_SHADERS - 1ULL)) { //check every 59th frame (of a 60-frame cycle) for updated shaders
        if (checkForUpdatedShaders()) {
            buildNewShader();
        }
    }

    propagateTime();

    updateTaggedVariablesWithOptick();
}



bool AssetLoadingDemo::checkForUpdatedShaders() {
    OPTICK_EVENT();
    for (auto iter = shaderSources.begin(); iter != shaderSources.end(); iter++) {
        if (iter->file.hasUpdatedFileAvailable()) {
            fprintf(MSGLOG, "\nDetected that shader %s has been updated. Rebuilding Shaders...\n", iter->file.filepath().c_str());
            return true;
        }
    }
    return false;
}

void AssetLoadingDemo::buildNewShader() {
    OPTICK_EVENT();
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

        default: // (Default should never happen so the message to be printed is a bit ridiculous)
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

//Eventually this ugly implementation logic should get moved to be a member of the
//FramePerformanceTimepointsList object.
void AssetLoadingDemo::reportStatistics() {
    OPTICK_EVENT();
    //Make sure we have a large enough sample size to generate an accurate report.
    if (framePerformance.framePerformanceListSize < 10ULL) { return; }

    double tBeginSum, tBeginAvg;
    double timeFromLoopBeginToDrawCommandsTotal, timeFromLoopBeginToDrawCommandsAvg;
    double timeFromDrawCommandsToFlipBuffersTotal, timeFromDrawCommandsToFlipBuffersAvg;
    double timeFromFlipBuffersToNextFrameBeginTotal, timeFromFlipBuffersToNextFrameBeginAvg;
    double capturedFramesCounter = 0.0;

    tBeginSum = 0.0;
    timeFromLoopBeginToDrawCommandsTotal = 0.0;
    timeFromDrawCommandsToFlipBuffersTotal = 0.0;
    timeFromFlipBuffersToNextFrameBeginTotal = 0.0;

    while (framePerformance.framePerformanceListHead != framePerformance.framePerformanceListCurrent) {

        //We need to make sure that this object has all of its Timepoints
        if ((nullptr == (framePerformance.framePerformanceListHead->timepointBeginRender)) ||
            (nullptr == (framePerformance.framePerformanceListHead->timepointFlipBuffers))) {
            //If a required Timepoint is missing from this object, delete it and move on 
            auto next = framePerformance.framePerformanceListHead->next;
            delete framePerformance.framePerformanceListHead;
            framePerformance.framePerformanceListSize--;
            framePerformance.framePerformanceListHead = next;
            continue;
        }

        else {
            const double t0 = framePerformance.framePerformanceListHead->tStart.timepoint;
            capturedFramesCounter += 1.0;
            tBeginSum += (framePerformance.framePerformanceListHead->next->tStart.timepoint - t0);
            timeFromLoopBeginToDrawCommandsTotal +=
                (framePerformance.framePerformanceListHead->timepointBeginRender->timepoint - t0);
            timeFromDrawCommandsToFlipBuffersTotal +=
                (framePerformance.framePerformanceListHead->timepointFlipBuffers->timepoint -
                    framePerformance.framePerformanceListHead->timepointBeginRender->timepoint);
            timeFromFlipBuffersToNextFrameBeginTotal +=
                (framePerformance.framePerformanceListHead->next->tStart.timepoint -
                    framePerformance.framePerformanceListHead->timepointFlipBuffers->timepoint);

            auto next = framePerformance.framePerformanceListHead->next;
            delete framePerformance.framePerformanceListHead;
            framePerformance.framePerformanceListSize--;
            framePerformance.framePerformanceListHead = next;
        }
    }

    //Make sure we have at least one frame to prevent division by 0
    if (capturedFramesCounter == 0ULL) 
        return; 

    if (reportPerformance) {
        tBeginAvg = tBeginSum / capturedFramesCounter;
        timeFromLoopBeginToDrawCommandsAvg = timeFromLoopBeginToDrawCommandsTotal / capturedFramesCounter;
        timeFromDrawCommandsToFlipBuffersAvg = timeFromDrawCommandsToFlipBuffersTotal / capturedFramesCounter;
        timeFromFlipBuffersToNextFrameBeginAvg = timeFromFlipBuffersToNextFrameBeginTotal / capturedFramesCounter;
        fprintf(MSGLOG, "\nPerformance Report:   [These are all averages] \n");
        fprintf(MSGLOG, "  Frame Average Time:                     %f sec\n"
            "  Time From Frame Begin to Draw:          %f sec\n"
            "  Time From Draw to Flip Buffers:         %f sec\n"
            "  Time From Flip Buffers to Next Frame:   %f sec\n",
            tBeginAvg, timeFromLoopBeginToDrawCommandsAvg,
            timeFromDrawCommandsToFlipBuffersAvg,
            timeFromFlipBuffersToNextFrameBeginAvg);
    }
}


void AssetLoadingDemo::propagateTime()  {
    OPTICK_EVENT();
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //  Propagate Time
    if (!freezeTimeToggle) { //if time is not frozen
        const float delta = (0.0125f * (1.0f + timeTickRateModifier));

        reverseTimePropogation ?
            (counter += delta) : (counter -= delta); //compute time propagation 
    
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
}


void AssetLoadingDemo::updateTaggedVariablesWithOptick() const {

    std::stringstream ss;
    ss << "0x" << std::hex << mainRenderWindow /*<< std::dec*/;

    OPTICK_TAG("WINDOW HANDLE", ss.str().c_str());
    OPTICK_TAG("FRAME #", frameCounter);
    OPTICK_TAG("Time Warp", timeTickRateModifier);
    OPTICK_TAG("Camera", cameraPos.x, cameraPos.y, cameraPos.z);
    OPTICK_TAG("Instance Count", (drawMultipleInstances ? instanceCount : (GLsizei)1));

}


//    [Implementation Note] 
//  The name of this next function is misleading in that it implies input 
//  reading and input processing are done as distinct phases (i.e. in sequence 
//  such that no updates to any program state are made until after the final 
//  polling of the gamepad's state). The actual implementation is much more 
//  direct about things, performing modifications to the program state as soon
//  as an input is read.
//   (UPDATE) Well actually I thought about it and really it is true that all of
//            the gamepads state will have been read by GLFW 
void AssetLoadingDemo::readJoystick0State_AssumingXInput_AndThenProcessAllInput() {
    OPTICK_EVENT();
    //Which joystick port to read input from 
    static constexpr const int JOYSTICK_TO_READ = 0; //Only read from hardcoded joystick port for now
    //Frequency (in frames elapsed) between repeated echos of warning messages
    static constexpr const uint32_t REPEAT_WARNING_FREQUENCY = 140U; //Must ALWAYS be 2U or higher


    if (glfwJoystickPresent(JOYSTICK_TO_READ)) {
        
        //If the Joystick is supported as a Gamepad we can make more broad
        //assumptions regarding its input mapping layout, thus allowing a
        //larger range of user actions to be available
        if (glfwJoystickIsGamepad(JOYSTICK_TO_READ)) {
            
            //-----------------------------------
            // Create and Initialize The Gamepad State Data Structure
            //-----------------------------------
            GLFWgamepadstate gamepadInput{};
            //Initialize the gamepad state object
            for (auto i = 0; i < sizeof(gamepadInput.buttons); i++)
                gamepadInput.buttons[i] = GLFW_RELEASE;// = static_cast<unsigned char>('\0');
            //Set the four analog stick axes to read as centered
            gamepadInput.axes[GLFW_GAMEPAD_AXIS_LEFT_X] = 0.0f;
            gamepadInput.axes[GLFW_GAMEPAD_AXIS_LEFT_Y] = 0.0f;
            gamepadInput.axes[GLFW_GAMEPAD_AXIS_RIGHT_X] = 0.0f;
            gamepadInput.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y] = 0.0f;
            //Set the triggers to -1.0f
            gamepadInput.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER] = -1.0f;
            gamepadInput.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER] = -1.0f;
            

            //-----------------------------------
            // Try to read the gamepad's state
            //-----------------------------------
            const int success = glfwGetGamepadState(JOYSTICK_TO_READ, &gamepadInput);
            if (GLFW_TRUE != success) {
                static bool firstFailureToReadGamepadFlag = true;
                bool printWarningMsgFlag = false;
                if (firstFailureToReadGamepadFlag) {
                    firstFailureToReadGamepadFlag = false;
                    printWarningMsgFlag = true;
                }
                else {
                    assert(REPEAT_WARNING_FREQUENCY > 2U);
                    if ((frameCounter % REPEAT_WARNING_FREQUENCY) == (REPEAT_WARNING_FREQUENCY - 1U))
                        printWarningMsgFlag = true;
                }
                if (printWarningMsgFlag) {
                    fprintf(WRNLOG, "\n\n"
                        "\t\t\t\t   WARNING!\a\n"
                        "        "
                        "FAILED TO READ STATE OF GAMEPAD CONNECTED TO JOYSTICK PORT %-2d!"
                        "        \n\n",
                            JOYSTICK_TO_READ);
                }
                return;
            }

            
            //Process the Axes [L/R Analog Sticks and Triggers]
            constexpr float HEAD_SPEED = 0.035f;
            constexpr float ROLL_SPEED = 0.035f;
            constexpr float PITCH_SPEED = 0.035f;

            //~~~~~~~~~~~~
            //    LEFT THUMB STICK
            //~~~~~~~~~~~~
            //If stick's value is greater than the neutral dead zone
            if (fabsf(gamepadInput.axes[GLFW_GAMEPAD_AXIS_LEFT_Y]) > 0.08f) 
                cameraPos += (0.185f * gamepadInput.axes[GLFW_GAMEPAD_AXIS_LEFT_Y] * glm::normalize(lookAtOrgin));
            if (fabsf(gamepadInput.axes[GLFW_GAMEPAD_AXIS_LEFT_X]) > 0.08f)
                cameraPos += (0.185f * gamepadInput.axes[GLFW_GAMEPAD_AXIS_LEFT_X] * glm::cross(glm::normalize(lookAtOrgin), glm::normalize(upDirection)));

            //~~~~~~~~~~~~
            //    RIGHT THUMB STICK
            //~~~~~~~~~~~~
            //Read head and pitch from axes 2 and 3 (which are x-y directions of right thumbstick)
            if (!(fabsf(gamepadInput.axes[2]) <= 0.08f)) //If stick's value is greater than the neutral dead zone
                head += gamepadInput.axes[2] * HEAD_SPEED;
            if (!(fabsf(gamepadInput.axes[3]) <= 0.08f)) 
                pitch += gamepadInput.axes[3] * PITCH_SPEED;
            roll += (((gamepadInput.axes[4] + 1.0f) / 2.0f) * ROLL_SPEED);
            roll -= (((gamepadInput.axes[5] + 1.0f) / 2.0f) * ROLL_SPEED);


           //~~~~~~~~~~~~
           //    D-PAD
           //~~~~~~~~~~~~
            if (gamepadInput.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP]) {
                zoom -= 0.075f; //Zoom 
            }
            if (gamepadInput.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN]) {
                zoom += 0.075f;
            }

            //Only do d-pad left/right input if drawing instanced
            if (drawMultipleInstances) {
                if (gamepadInput.buttons[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT]) {
                    instanceCount++;
                }
                if (gamepadInput.buttons[GLFW_GAMEPAD_BUTTON_DPAD_LEFT]) {
                    instanceCount--;
                    if (instanceCount < 1)
                        instanceCount = 1;
                }
            }

            if (gamepadInput.buttons[GLFW_GAMEPAD_BUTTON_LEFT_BUMPER]) {
                fov = fov * 0.985f;
            }
            if (gamepadInput.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER]) {
                fov = fov * 1.015f;
            }

            if (gamepadInput.buttons[GLFW_GAMEPAD_BUTTON_BACK]) {
                reset();
            }

            recomputeProjectionMatrix();


            /////////////////////////////////////
            //Process the A, B, X, Y buttons
            /////////////////////////////////////
            if (gamepadInput.buttons[GLFW_GAMEPAD_BUTTON_A]) {
                if ((frameCounter - frameBlendOperationLastToggled) > FRAMES_TO_WAIT_BETWEEN_INPUT_READS) {
                    toggleBlending();
                }
            }
            if (gamepadInput.buttons[GLFW_GAMEPAD_BUTTON_B]) {
                if ((frameCounter - frameThatTimePropogationWasLastReversed) > FRAMES_TO_WAIT_BETWEEN_INPUT_READS)
                    reverseTime();
            }
            if (gamepadInput.buttons[GLFW_GAMEPAD_BUTTON_X]) {
                if ((frameCounter - frameTimeFreezeLastToggled) > FRAMES_TO_WAIT_BETWEEN_INPUT_READS)
                    toggleTimeFreeze();
            }
            if (gamepadInput.buttons[GLFW_GAMEPAD_BUTTON_Y]) {
                if ((frameCounter - frameInstancedDrawingBehaviorLastToggled) > FRAMES_TO_WAIT_BETWEEN_INPUT_READS) {
                    frameInstancedDrawingBehaviorLastToggled = frameCounter; //Mark the current frame as being the one when instanced drawing behavior was last toggled
                    drawMultipleInstances = !drawMultipleInstances; //Perform Toggle
                    fprintf(MSGLOG, "Instanced Rendering set to: %s",
                        (drawMultipleInstances ? ("Enabled\n") : ("Disabled\n")));
                }
            }




            //If Input Button Combo
                //Then Recompile Shaders? [This would save time versus reimplementing entire shader class collection]



        }

        
        else { //If Joystick 0 is not a gamepad 
            int buttons = 0;
            const unsigned char* buttonStates = glfwGetJoystickButtons(0, &buttons);
            if (buttons > 0) {
                for (int i = 0; i < buttons; i++) {

                    //   if (buttonStates[i] == GLFW_PRESS)
                          // stateMsg << "PRESSED";
                     //  else
                     //     // stateMsg << "RELEASED";
                     //  if ((i % 2) == 0)
                          // stateMsg << "\t\t";
                     //  else
                         //  stateMsg << "\n";
                }
                //if ((buttons % 2) == 0)
                   // stateMsg << "\n";

            }
        }
    }


}


void AssetLoadingDemo::renderScene() {
    OPTICK_EVENT();
    ////////////////////////////
    //  Set up to draw frame
    ////////////////////////////
    updateFrameClearColor(); //background color
    updateBaseUniforms();

    drawVerts();
}


void AssetLoadingDemo::updateFrameClearColor() {
    OPTICK_EVENT();
    if constexpr (true) {
        glClearColor(0.000932f, 0.000924f, 0.0009135f, 1.0f);
        //glClearColor(0.132f, 0.24f, 0.135f, 1.0f);
    }
    else /*if constexpr (false)*/ {
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


void AssetLoadingDemo::updateBaseUniforms() noexcept {
    OPTICK_EVENT();
    //quadTextureTestShader = nullptr;
    if (quadTextureTestShader) {
        quadTextureTestShader->use();
        //Update the quadTextureTestShader uniforms
        quadTextureTestShader->uniforms.updateUniform1f("time", counter);
        quadTextureTestShader->uniforms.updateUniform1f("zoom", zoom);
        rotation = MathFunc::computeRotationMatrix4x4(head, pitch, roll);
        quadTextureTestShader->uniforms.updateUniformMat4x4("rotation", &rotation);
        
        glm::mat4 MVP; //Model-View-Projection matrix 
        MVP = perspective * (view * (rotation));
        const glm::mat4 userTranslation = glm::mat4(1.0f, 0.0f, 0.0f, 0.0f,             //Translation from user input
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            xTranslation, yTranslation, zTranslation, 1.0f);
        MVP *= userTranslation;//* MVP;
        
        
        quadTextureTestShader->uniforms.updateUniformMat4x4("MVP", &MVP);
        
        quadTextureTestShader->uniforms.updateUniform1u(CUSTOM_SHADER_PARAMETER_1_UNIFORM_NAME, customShaderParameter1);
        quadTextureTestShader->uniforms.updateUniform1u(CUSTOM_SHADER_PARAMETER_2_UNIFORM_NAME, customShaderParameter2);
        quadTextureTestShader->uniforms.updateUniform1u(CUSTOM_SHADER_PARAMETER_3_UNIFORM_NAME, customShaderParameter3);

        return;
    }
    
    //ELSE 
    
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
                                                xTranslation, yTranslation, zTranslation, 1.0f);
    MVP *= userTranslation;//* MVP;
    
    
    sceneShader->uniforms.updateUniformMat4x4("MVP", &MVP);
    
    sceneShader->uniforms.updateUniform1u(CUSTOM_SHADER_PARAMETER_1_UNIFORM_NAME, customShaderParameter1);
    sceneShader->uniforms.updateUniform1u(CUSTOM_SHADER_PARAMETER_2_UNIFORM_NAME, customShaderParameter2);
    sceneShader->uniforms.updateUniform1u(CUSTOM_SHADER_PARAMETER_3_UNIFORM_NAME, customShaderParameter3);
    
}


void AssetLoadingDemo::drawVerts() {
    OPTICK_EVENT();
    const GLsizei BUFFER_SIZE = computeNumberOfVerticesInSceneBuffer(sceneBuffer);

    //if (sceneShader)
    //	sceneShader->use();

    if (quadTextureTestShader)
        quadTextureTestShader->use();

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
    GLsizei instanceCount);
    */
    if (currentPrimitiveInputType == PIPELINE_PRIMITIVE_INPUT_TYPE::TRIANGLE_OUTLINE) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triangleOutlineEBO);
        if (drawMultipleInstances)
            glDrawElementsInstanced(GL_LINES, 2*BUFFER_SIZE, GL_UNSIGNED_INT, (const void*)0, instanceCount);
        else
            glDrawElements(GL_LINES, 2*BUFFER_SIZE, GL_UNSIGNED_INT, (const void*)0); //Last param is offset into ebo to start with
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

void AssetLoadingDemo::presentFrame() {
    OPTICK_EVENT();


    glfwSwapBuffers(mainRenderWindow); //Swap the buffer to present image to monitor
    glfwPollEvents();

    //See RenderDemoBase for description, basically this function should be called once a frame to detect context-reset situations
    if (checkForContextReset()) {
        fprintf(MSGLOG, "\nContext Reset Required!\n");
        fprintf(MSGLOG, "\n\t[Press enter to crash]\n");
        std::cin.get();
        glfwSetWindowShouldClose(mainRenderWindow, GLFW_TRUE); //For now just close the mainRenderWindow
    }

    frameCounter++; //Increment the frame counter
    prepareGLContextForNextFrame();
}

void AssetLoadingDemo::prepareGLContextForNextFrame() noexcept {
    OPTICK_EVENT();
    //Make sure we still have our context
    if ((glfwGetCurrentContext() != mainRenderWindow)) {
        fprintf(ERRLOG,
            "\n\n\t\t\t!!!!\t[ERROR OCCURED]\t!!!!\a\n"
            "\tCause: Window Lost Graphics Context / Context Is Not Current\n");
        glfwSetWindowShouldClose(mainRenderWindow, GLFW_TRUE);
        return;
    }
    glBindVertexArray(0);
    glUseProgram(0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


bool AssetLoadingDemo::buildQuadTextureTestShader() {
    OPTICK_EVENT();
    quadTextureTestShader = std::make_unique<ShaderProgram>();
    quadTextureTestShader->attachVert("Shaders\\DrawTextureOnQuad.vert");
    quadTextureTestShader->attachFrag("Shaders\\DrawTextureOnQuad.frag");
    //Attach Secondary Vertex Shaders

    //Attach Secondary Fragment Shaders
    std::unique_ptr<ShaderInterface::FragmentShader> fragmentNoiseShader =
        std::make_unique<ShaderInterface::FragmentShader>(R"(Shaders\ShaderNoiseFunctions.glsl)");
    fragmentNoiseShader->makeSecondary();
    quadTextureTestShader->attachSecondaryFrag(fragmentNoiseShader.get());
    std::unique_ptr<ShaderInterface::FragmentShader> practiceNoiseShader =
        std::make_unique<ShaderInterface::FragmentShader>(R"(Shaders\PracticeNoise.glsl)");
    practiceNoiseShader->makeSecondary();
    quadTextureTestShader->attachSecondaryFrag(practiceNoiseShader.get());
    quadTextureTestShader->link();
    return quadTextureTestShader->checkIfLinked();
}


GLsizei AssetLoadingDemo::computeNumberOfVerticesInSceneBuffer(const std::vector<GLfloat>& sceneBuffer) const noexcept {
    static constexpr const GLsizei vertexSize = 4u + 2u + 3u;  //Since each vertex is {x,y,z,w, s,t, nx,ny,nz}  (i.e. 9 components total)
    return (sceneBuffer.size() / vertexSize);
}


void AssetLoadingDemo::generateTriangleOutlineElementOrdering() noexcept {
    OPTICK_EVENT();
    //For most of the available primitive types in member enum 
    //'PIPELINE_PRIMITIVE_INPUT_TYPE', the vertex data gets loaded into its vertex
    //buffer already arranged in the proper ordering to be drawn correctly. The
    //only exception to this is the primitive type 'TRIANGLE_OUTLINE', which
    //requires a slightly more involved vertex ordering pattern for drawing [Yes
    //I at this stage intend to have primitives 'TRIANGLE_STRIP' and 'TRIANGLE_FAN'
    //both look like that. Believe it or not, although rare, they have each at times
    //produced some cool results. Plus rearranging the data to accomodate either
    //of them would be a considerably more involved process]. 

    //The idea behind this primitive type is to trace along the edges of each triangle
    //with a line segment, producing an outline of each triangle. To pull this off is 
    //actually very straight-forward and easily seen from considering the following
    //diagrans.
    //   
    //Consider how each triangle vertex is arranged in the vertex buffer:
    //
    //                  ORDERING OF VERTEX DATA IN VERTEX BUFFER                 
    //                                                                           
    //                      v0                                 v3                
    //                     /  \                               /  \               
    //                    /    \                             /    \              
    //                   /      \                           /      \             
    //                  /        \                         /        \            
    //                 /          \                       /          \           
    //                /            \                     /            \          
    //               /  Triangle 0  \                   /  Triangle 1  \         
    //              /                \                 /                \        
    //            v1 ---------------- v2             v4 ----------------  v5     
    //                                                                           
    //                                                                           
    //                                                                           
    //  OpenGL draws line segments from vertex data with the following pattern:  
    //                                                                           
    //      v0--------v1     v2--------v3     v4--------v5     v6--------v7      
    //                                                                           
    //                                                                           
    //                                                                           
    //  Thus rather than drawing straight through the vertex data, it is         
    //  necessary to create a different vertex ordering so that each triangle's  
    //  edges are drawn as line segments. Since each vertex will be both a       
    //  starting point and an endpoint for a line segment, it must be that this  
    //  new ordering must require using twice the number of total vertices as    
    //  to completely draw the entire data set.
    //   
    //  This new ordering appears as:
    //
    //          v0v1 v1v2 v2v0          v3v4 v4v5 v5v3
    //
    //  The reason this works should be readily apparent by examining the above 
    //  triangle and line segment diagrams.
    //

    //  The task of this function is to generate an ascending sequence of numbers 
    //  following the pattern mentioned above until we have created values        
    //  twice the size of the vertex data in the vertex buffer.                   
    //


    std::vector<GLuint> vertexOrderingForTriangleOutline;


    const GLsizei numberOfVerticesInSceneBuffer = computeNumberOfVerticesInSceneBuffer(sceneBuffer);

    //The number of elements required is always twice the number of vertices in the sceneBuffer
    const GLsizei elementsToGenerate = 2u * numberOfVerticesInSceneBuffer;
    try {
        vertexOrderingForTriangleOutline.reserve(elementsToGenerate);
        for (GLsizei i = 0; i < elementsToGenerate; i += 3) {//numberOfVerticesInSceneBuffer; i += TRIANGLE_SIDES_AMOUNTAGE) {

            //Triangle side 1
            vertexOrderingForTriangleOutline.push_back(i);
            vertexOrderingForTriangleOutline.push_back(i + 1);

            //Triangle side 2
            vertexOrderingForTriangleOutline.push_back(i + 1);
            vertexOrderingForTriangleOutline.push_back(i + 2);

            //Triangle side 3
            vertexOrderingForTriangleOutline.push_back(i + 2);
            vertexOrderingForTriangleOutline.push_back(i);
        }
    }
    catch (const std::exception & e) {
        try { //Compiler was griping that 'e.what()' might throw an exception
            fprintf(ERRLOG, "\nCaught Exception: %s!\n", e.what());
        }
        catch (...) { fprintf(ERRLOG, "\nError printing error message!\n"); std::exit(EXIT_FAILURE); }
    }

    //Once generated, swap order out with AssetLoadingDemo's member
    triangleOutlineElementOrdering.swap(vertexOrderingForTriangleOutline);
}



void AssetLoadingDemo::printNameOfTheCurrentlyActivePrimitive() const {
    OPTICK_EVENT();
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


//Checks to see if either of the shift keys are pressed and returns a 
//value for a 'boost' to be used for increasing input rate of changes. 
float AssetLoadingDemo::getShiftBoost() const noexcept {
    float boost = 1.0f;
    if (glfwGetKey(mainRenderWindow, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)
        boost *= 4.0f;
    if (glfwGetKey(mainRenderWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        boost *= 3.0f;
    return boost;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
////    The remaining functions are utility functions that are called by the setup functions   ////
///////////////////////////////////////////////////////////////////////////////////////////////////



void AssetLoadingDemo::buildSceneBufferFromLoadedSceneObjects() {
    OPTICK_EVENT();
    //  Background and Description
    //In the loadModels() function, a vector of unique pointer to QuickObjects 
    //is created. Each of these QuickObjects contains mesh data for a model,
    //with each model in its own model space.
    //This function's purpose is to take this vector of model meshes (each in its own
    //space) and position them into a single mesh all in world space. 
    
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

    if (sceneSize == 0)
        return;

    //Get an iterator to the first member of the sceneObject vector
    auto sceneObjIter = sceneObjects.cbegin();
    //Place the first object in the scene at the origin
    addObject(sceneObjIter, glm::vec3(0.0f, 0.0f, 0.0f));
    sceneObjIter++;
    objectCounter++;
    fprintf(MSGLOG, "\tAdding Object 0 to scene...\n");
    fprintf(MSGLOG, "\t\tPosition of Object 0:   <0.0f, 0.0f, 0.0f>\n");

    //Iterate through each object in the sceneObjects vector
    for ( ; sceneObjIter != sceneObjects.cend(); sceneObjIter++) {
        objectCounter++;

        //Objects missing Normal and/or Texture Coordinates will need to have data generated for them
        //so that all vertices in the scene share the same format

        

        //Increment offset to prepare for the next object
        objectPositionOffset.x += CHANGE_BETWEEN_OBJECTS.x + MathFunc::getRandomInRangef(-12.0f, 12.0f);
        objectPositionOffset.y += CHANGE_BETWEEN_OBJECTS.y + MathFunc::getRandomInRangef(-6.0f, 6.0f);
        //objectPositionOffset.z += MathFunc::getRandomInRangef(5.0f, 5.0f);

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
    OPTICK_EVENT();
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
        "vertex ordering required to properly draw lines. (BufferID = %u)\n\n",
        triangleOutlineEBO);
}


void AssetLoadingDemo::uploadSceneBufferToGPU(GLuint& targetVBO, const std::vector<float>& sceneBuf) noexcept {
    OPTICK_EVENT();
    //if (sceneBuf.size() == 0u)
    //    return;
    if (0u == targetVBO) {
        fprintf(WRNLOG, "\nWARNING! An issue has occurred while uploading a sceneBuffer\n"
            "to its Vertex Buffer Object because it appears as though the target VBO was never\n"
            "allocated from the context!\n");
        fprintf(WRNLOG, "\nThis function here will try to allocate a VBO to use instead...\n");
        glCreateBuffers(1, &targetVBO);
        if (0u == targetVBO) {
            fprintf(ERRLOG, "\n\n\nThe attempt to allocate a VBO to use has failed...\n");
            fprintf(ERRLOG, "         Press [ENTER] to crash...\n");
            try { //Compiler was warning that 'cin.get()' may throw exceptions 
                std::cin.get();
                std::exit(EXIT_FAILURE);
            }  //Catch it all
            catch (...) { std::exit(EXIT_FAILURE); }
        }
        else
            fprintf(WRNLOG, "A Vertex Buffer Object with ID=%u has been created!\n\n", targetVBO);
    }

    const GLsizei vertexCount = computeNumberOfVerticesInSceneBuffer(sceneBuf);


    glBindBuffer(GL_ARRAY_BUFFER, targetVBO);

    fprintf(MSGLOG, "\nInitiating transfer of the sceneBuffer data from the Application to the GPU.\n"
        "Target destination on GPU is set to use Array Buffer ID %u.\n", targetVBO);

    fprintf(MSGLOG, "  [TRANSFER STATISTICS]\n");
    fprintf(MSGLOG, "There are %d vertices total in the scene, or %d 32-bit floating point values\n\n", vertexCount, vertexCount * NUM_VERTEX_COMPONENTS);

    glBufferData(GL_ARRAY_BUFFER, sceneBuf.size() * sizeof(sceneBuf.data()), sceneBuf.data(), GL_STATIC_DRAW);

}

void AssetLoadingDemo::uploadTriangleOutlineElementOrderingBufferToGPU(GLuint& ebo, const std::vector<GLuint>& eboData) noexcept {
    OPTICK_EVENT();
    if (0u == ebo) {
        fprintf(WRNLOG, "\n\tWARNING!\nUnable to Upload Element Ordering Buffer to GPU because EBO was\n"
            "never created with GL context!\n");
        return;
    }

    const GLsizei elementBufferSize = eboData.size();


    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    fprintf(MSGLOG, "\nTransferring Additional Data from the Application to the GPU.\n"
        "Target destination on GPU is set to use Element Array Buffer ID %u.\n", ebo);

    fprintf(MSGLOG, "  [TRANSFER STATISTICS]\n");
    fprintf(MSGLOG, "There are %u indices total in the scene, or %u GL_UNSIGNED_INT values\n\n", elementBufferSize, elementBufferSize);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * elementBufferSize, eboData.data(), GL_STATIC_DRAW);

}



void AssetLoadingDemo::configureVertexArrayAttributes() noexcept {
    OPTICK_EVENT();
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


