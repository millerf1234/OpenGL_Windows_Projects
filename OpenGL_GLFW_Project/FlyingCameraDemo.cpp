
// WORK IN PROGRESS!!
//Based Heavily on AssetLoadingDemo

#include "FlyingCameraDemo.h"


#include "Timepoint.h"

FlyingCameraDemo::FlyingCameraDemo(InitReport* initReport) : AssetLoadingDemo(initReport) {
    Timepoint creation("FlyingCameraDemo: Beginning Construction of object!");
    if (error) {
        fprintf(ERRLOG, "\nError Occurred While Constructing the FlyingCameraDemo!\n");
        return;
    }

    if (!mainRenderWindow) {
        fprintf(ERRLOG, "\nError Occurred While In Constructor for The FlyingCameraDemo!\n"
            "The error is because the variable 'mainRenderWindow' is set to nullptr!\n");
        return;
    }

    createCamera();
}

FlyingCameraDemo::~FlyingCameraDemo() noexcept {

}


void FlyingCameraDemo::loadAssets() {
    Timepoint loadingStart("FlyingCameraDemo: function \'loadAssets()\' entered!");
    loadFlyingCameraDemoShaders();
    loadFlyingCameraDemoWorldMesh();

    prepareScene();
    Timepoint loadingFinish("FlyingCameraDemo: functions \'loadAssets()\' has completed!");
}

void FlyingCameraDemo::run() {
    AssetLoadingDemo::run();
}

void FlyingCameraDemo::updateRenderDemoSpecificUniforms() noexcept {
    /*static int count = 0;
    count++;
    if (count % 115 == 114)
        fprintf(MSGLOG, "\nFlyingCameraDemoDerivedFuncBehavior in effect!\n");*/

}

void FlyingCameraDemo::createCamera() noexcept {
    //todo
}

void FlyingCameraDemo::loadFlyingCameraDemoShaders() noexcept {

    const std::string filepathToShaders = FILEPATH_TO_SHADERS;

    //Create our shader program (i.e. pipeline)
    sceneShader = std::make_unique<ShaderProgram>();

    
    ////////////////////////////////////////////////////////////////////
    //////////////////         VERTEX SHADERS         //////////////////
    ////////////////////////////////////////////////////////////////////
    //Attach the primary vertex shader
    sceneShader->attachVert(filepathToShaders + "FlyingCameraDemo.vert");
    
    //Build and attach any required secondary Vertex shaders

    //Noise shader collection 01
#if 0
    ShaderInterface::VertexShader vertNoise(filepathToShaders + "ShaderNoiseFunctions.glsl");
    vertNoise.makeSecondary();
    if (!vertNoise.readyToBeAttached()) 
        fprintf(ERRLOG, "\nError attaching secondary noise shader as vertex shader!\n");
    else 
        sceneShader->attachSecondaryVert(&vertNoise);
#endif 
    //Noise shader collection 02
    ShaderInterface::VertexShader psrdNoise(filepathToShaders +
        "AshimaArts_NoiseCollection\\psrdnoise.glsl");
    psrdNoise.makeSecondary();
    if (!psrdNoise.readyToBeAttached())
        fprintf(ERRLOG, "\nError attaching \'psrdNoise\' shader!\n");
    else
        sceneShader->attachSecondaryVert(&psrdNoise);


    ////////////////////////////////////////////////////////////////////
    //////////////////       FRAGMENT SHADERS         //////////////////
    ////////////////////////////////////////////////////////////////////
    sceneShader->attachFrag(filepathToShaders + "FlyingCameraDemo.frag");
#if 0
    ShaderInterface::FragmentShader fragNoise(filepathToShaders + "ShaderNoiseFunctions.glsl");
    fragNoise.makeSecondary();
    if (!fragNoise.readyToBeAttached()) 
        fprintf(ERRLOG, "\nError attaching secondary noise shader as fragment shader!\n");
    else 
        sceneShader->attachSecondaryFrag(&fragNoise);
#endif 
    //Noise shader collection 02
    ShaderInterface::FragmentShader psrdNoiseFrag(filepathToShaders +
        "AshimaArts_NoiseCollection\\psrdnoise.glsl");
    psrdNoiseFrag.makeSecondary();
    if (!psrdNoiseFrag.readyToBeAttached())
        fprintf(ERRLOG, "\nError attaching \'psrdNoise\' shader!\n");
    else
        sceneShader->attachSecondaryFrag(&psrdNoiseFrag);

    
    ////////////////////////  LINK SHADER PROGRAM  /////////////////////
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
}

void FlyingCameraDemo::loadFlyingCameraDemoWorldMesh() noexcept {

    const std::string filepathToModels = FILEPATH_TO_MODELS;
    std::string modelName, modelName2;
    //modelName = "DemoSceneInsideABox00.obj";
    //modelName = "SimpleSkyDome_ReExport.obj";
    //modelName = "DebugSimpleSkyDomeTest.obj";
     //modelName2 = "DebugSimpleSkyDomeTest_Triangulated.obj";
    //modelName = "SubdivisionCube.obj";

    modelName = "HighlyExperimental\\BlenderTextExportExperiment\\BlenderTextExportTest_000001.obj";

    //modelName = "box_cutter.obj";
    //modelName2 = "Spaceship.obj";

    //modelName = "CargoSpaceshipIdeaThing02.obj";

#if 0 
    for (int i = 0; i < 35; i++) {
        int modelToLoad = MathFunc::getRandomInRangei(0, 8);
        switch (modelToLoad) {
        default:
            modelName = "2DTexturedQuadPlane.obj";
            break;
        case 0:
            modelName = "SubdivisionCube.obj";
            break;
        case 1:
            modelName = "2DTexturedQuadPlane.obj";
            break;
        case 2:
            modelName = "2DTexturedQuadPlane.obj";
            break;
        case 3:
            modelName = "2DTexturedQuadPlane.obj";
            break;
        case 4:
            modelName = "2DTexturedQuadPlane.obj";
            break;
        case 5:
            modelName = "2DTexturedQuadPlane.obj";
            break;
        case 6:
            modelName = "2DTexturedQuadPlane.obj";
            break;
        case 7:
            modelName = "2DTexturedQuadPlane.obj";
            break;
        case 8:
            modelName = "2DTexturedQuadPlane.obj";
            break;
        }
#else
        sceneObjects.emplace_back(std::make_unique<QuickObj>(filepathToModels + modelName, 1.0));
        sceneObjects.emplace_back(std::make_unique<QuickObj>(filepathToModels + modelName2, 1.0));
#endif 
#if 0 
    }
#endif 
    //Report to console how many models were loaded
    const size_t loadedModlCount = sceneObjects.size();
    if (loadedModlCount == 0u)
        fprintf(MSGLOG, "\nNo models were loaded!\n");
    else if (loadedModlCount == 1u)
        fprintf(MSGLOG, "\n%u model has been loaded!\n", loadedModlCount);
    else
        fprintf(MSGLOG, "\n%u models were loaded!\n", loadedModlCount);

}