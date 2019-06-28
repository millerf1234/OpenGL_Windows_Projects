//Based heavily on the AssetLoadingDemo

//Work in progress

#pragma once

#ifndef FLYING_CAMERA_DEMO_H_
#define FLYING_CAMERA_DEMO_H_

#include "AssetLoadingDemo.h"
/* By including AssetLoadingDemo.h, the following are also imported
#include <thread>  //for sleep
#include <math.h>
#include <functional>   //std::function

#include "GlobalIncludes.h"
#include "GlobalConstants.h"
#include "RelativeFilepathsToResources.h"
#include "MathFunctions.h"
#include "MeshFunctions.h"
#include "GLFW_Init.h"
#include "ShaderProgram.h"

#include "RenderDemoBase.h"
#include "QuickObj.h" //For loading '.obj' files
*/









class FlyingCameraDemo : public AssetLoadingDemo { 
public:
    FlyingCameraDemo() = delete;
    FlyingCameraDemo(InitReport*);
    virtual ~FlyingCameraDemo() noexcept override;

    virtual void loadAssets() override;
    virtual void run() override;

protected:
    virtual void updateRenderDemoSpecificUniforms() noexcept override;

private:

    void createCamera() noexcept;

    void loadFlyingCameraDemoShaders() noexcept;

    void loadFlyingCameraDemoWorldMesh() noexcept;
};




#endif //FLYING_CAMERA_DEMO_H_