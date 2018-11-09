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
#include <math.h>

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
#include "LightSource.h"


using ShaderInterface::GenericVertexAttributeSet; 


class AssetLoadingDemo : public RenderDemoBase { 
public:
	AssetLoadingDemo() = delete;
	AssetLoadingDemo(std::shared_ptr<MonitorData> screenInfo);
	virtual ~AssetLoadingDemo() override;

	virtual void loadAssets() override;
	virtual void run() override;
	
private:
	bool error;
	float counter;
	unsigned long long frameNumber, frameUnpaused, frameOfMostRecentColorRecording, frameLineTypeLastSwitched;

	glm::vec3 backgroundColor;

	GLuint vao, vbo;

	PIPELINE_PRIMATIVE_INPUT_TYPE currentPrimativeInputType;
	std::unique_ptr<ShaderProgram> sceneShaderWithoutTexture;
	std::unique_ptr<ShaderProgram> sceneShaderWithTexture;

	std::unique_ptr<ShaderProgram> lightSourceShader;

	//std::vector<std::unique_ptr<QuickObj>> sceneObjectPtrs;
	
	glm::vec3 cameraPosition;

	//Asset Transformation information
	float xRotation, yRotation, zRotation;
	float zoom;
	float scalingAppliedToAsset; //Needed for keeping normal vectors the right length after Model-View transforming
	bool mvpMatrixNeedsUpdating;
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
	glm::mat4 mvp;
	glm::mat3 normalModelView;

	
	

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
	void rotate();



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