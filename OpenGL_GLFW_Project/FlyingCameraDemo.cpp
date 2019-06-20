
// WORK IN PROGRESS!!
//Based Heavily on AssetLoadingDemo

#include "FlyingCameraDemo.h"



FlyingCameraDemo::FlyingCameraDemo(InitReport* initReport) : AssetLoadingDemo(initReport) {
    if (error) {
        fprintf(ERRLOG, "\nError Occured While Constructing the FlyingCameraDemo!\n");
        return;
    }

    if (!mainRenderWindow) {
        fprintf(ERRLOG, "\nError Occured While In Constructor for The FlyingCameraDemo!\n"
            "The error is because the variable 'mainRenderWindow' is set to nullptr!\n");
        return;
    }

    createCamera();
}

FlyingCameraDemo::~FlyingCameraDemo() noexcept {

}


void FlyingCameraDemo::loadAssets() {
    loadFlyingCameraDemoShaders();
    loadFlyingCameraDemoWorldMesh();

    prepareScene();
}

void FlyingCameraDemo::run() {
    AssetLoadingDemo::run();
}

void FlyingCameraDemo::createCamera() noexcept {
    //todo
}

void FlyingCameraDemo::loadFlyingCameraDemoShaders() noexcept {

    const std::string filepathToShaders = FILEPATH_TO_SHADERS;

    //Create our shader program (i.e. pipeline)
    sceneShader = std::make_unique<ShaderProgram>();

    sceneShader->attachVert(filepathToShaders + "FlyingCameraDemo.vert");
    ShaderInterface::VertexShader vertNoise(filepathToShaders + "ShaderNoiseFunctions.glsl");
    vertNoise.makeSecondary();
    if (!vertNoise.readyToBeAttached()) {
        fprintf(ERRLOG, "\nError attaching secondary noise shader as vertex shader!\n");
        std::exit(EXIT_FAILURE);
    }
    else {
        sceneShader->attachSecondaryVert(&vertNoise);
    }

    sceneShader->attachFrag(filepathToShaders + "FlyingCameraDemo.frag");
    ShaderInterface::FragmentShader fragNoise(filepathToShaders + "ShaderNoiseFunctions.glsl");
    fragNoise.makeSecondary();

    if (!fragNoise.readyToBeAttached()) {
        fprintf(ERRLOG, "\nError attaching secondary noise shader as fragment shader!\n");
        std::exit(EXIT_FAILURE);
    } 
    else {
        sceneShader->attachSecondaryFrag(&fragNoise);
    }
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
   // modelName = "SimpleSkyDome_ReExport.obj";
    //modelName = "DebugSimpleSkyDomeTest.obj";
   // modelName2 = "DebugSimpleSkyDomeTest_Triangulated.obj";
    //modelName = "SubdivisionCube.obj";

    //modelName = "box_cutter.obj";

    modelName = "CargoSpaceshipIdeaThing02.obj";

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
        //sceneObjects.emplace_back(std::make_unique<QuickObj>(filepathToModels + modelName2, 1.0));
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