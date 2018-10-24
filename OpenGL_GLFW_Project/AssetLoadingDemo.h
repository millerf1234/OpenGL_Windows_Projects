//This file contains sample code that I wrote for testing 
//the asset loading implementation code. It isn't finished 
//yet as I write this comment, but expect the code to have
//something to do with loading and rendering assets from
//'.obj' and '.mtl' files.
//
// Programmer:   Forrest Miller
// Date:         10/23/2018

#pragma once

#ifndef	ASSET_LOADING_DEMO_H_
#define ASSET_LOADING_DEMO_H_

#include <thread>

#include "ProjectConstants.h"
#include "ProjectParameters.h"
#include "MathFunctions.h"
#include "GLFW_Init.h"
#include "ProjectSetup.h"

#include "ProjectResourceFilepaths.h"

#include "ShaderProgram.h"
#include "GenericVertexAttributeSet.h"

#include "RenderDemoBase.h"

#include "QuickObj.h" //For loading '.obj' files


using ShaderInterface::GenericVertexAttributeSet; 

//enum class PIPELINE_TRIANGLE_INPUT_TYPE { NORMAL, STRIP, FAN };

class AssetLoadingDemo : public RenderDemoBase { 
public:
	AssetLoadingDemo() = delete;
	AssetLoadingDemo(std::shared_ptr<MonitorData> screenInfo);
	virtual ~AssetLoadingDemo() override;

	virtual void loadAssets() override;
	virtual void run() override;
	
private:
	bool error;
	unsigned long long frameNumber, frameUnpaused, frameOfMostRecentColorRecording;
	float counter;
	float xRotation, yRotation, zRotation;
	float redRotationTheta, greenRotationTheta, blueRotationTheta;
	glm::vec3 backgroundColor;

	PIPELINE_TRIANGLE_INPUT_TYPE currentTriangleInputType;

	//The GLFWwindow pointer is a protected member of the RenderDemoBase class 
	//GLFWwindow * window; //Pointer to target renderable window (Application should provide this)

	std::unique_ptr<ShaderProgram> sceneShader; 
	std::unique_ptr<ShaderProgram> sceneShaderLine;
	std::vector<std::unique_ptr<QuickObj>> sceneObjectPtrs;
	std::unique_ptr<GenericVertexAttributeSet> vertexAttributes;
	

	//   Helper Functions
	void initialize(); 

	//This function will only be correct if this objects sceneObjectPtrs all have the exact same type of components.
	GLsizei computeSceneObjectPtrsTotalIndices() const;

	void loadModels();

	void loadShaders();



	///////////////////////////////////////////////////////
	/////////////      The Render Loop      ///////////////
	///////////////////////////////////////////////////////
	void renderLoop();

	//-----------------------------------------------------------------//
	//    The Render Loop Consists of the following series of steps    //
	//-----------------------------------------------------------------//

	/*							+~~~~~~~~~~~~~~~~~~~~~~~~~~+
								|   (1)  Input Detection   |
								+~~~~~~~~~~~~~~~~~~~~~~~~~~+	        								 */
	
	bool checkToSeeIfShouldCloseWindow() const; //Probably 'esc'
	bool checkIfShouldPause() const; //Probably 'space'
	bool checkIfShouldRecordColor() const; //records the current frames background clear color, probably tied to input 'p' or 'P'
	
	bool checkIfShouldReset() const;


	/*						   +~~~~~~~~~~~~~~~~~~~~~~~~~~~+
							   |  (2)  Input Processing    |
							   +~~~~~~~~~~~~~~~~~~~~~~~~~~~+	        								 */

	void pause();
	void recordColorToLog();
	void reset();


	
	void changePrimitiveType(); 



	/*						    +~~~~~~~~~~~~~~~~~~~~~~~~~~~~+
							    |	 (3)   Handle Events	 |
							    +~~~~~~~~~~~~~~~~~~~~~~~~~~~~+			    			                 */
	







	/*								+~~~~~~~~~~~~~~~~~~+
									|	(4) Render     |
									+~~~~~~~~~~~~~~~~~~+			    							     */

	////////////////////////////////////////
	///  (4a)  Background Color Update   ///
	////////////////////////////////////////

	void updateFrameClearColor();


	/////////////////////////////////
	///  (4b)  Update Uniforms    ///
	/////////////////////////////////
	void updateUniforms();



	////////////////////////////////
	///  (4c)  Update Buffers    ///
	////////////////////////////////




	/////////////////////////////////
	///  (4d)  Make Draw Calls    ///                          (This step could also do a multi-draw indirect)
	/////////////////////////////////
	void drawVerts();


	
	
	/*						+~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+
							|	(5) Clean-up and Prepare for Next Frame     |
							+~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+			   			     */
	
	void prepareGLContextForNextFrame();
};





#endif //ASSET_LOADING_DEMO_H_