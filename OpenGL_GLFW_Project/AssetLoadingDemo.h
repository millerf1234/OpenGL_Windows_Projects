//File:             AssetLoadingDemo.h
//
//Description:      This header file contains the definition for a class which inherits from
//                  RenderDemoBase, thus making this class represent a RenderDemo. 
//                  The point of this RenderDemo is to demonstrate basic object parsing/loading
//                  from '.obj' 3D model files. In order to ensure model loading is done properly,
//                  this renderdemo uses its own utility class classed QuickObject. Note that 
//                  the 'Quick' in QuickObject is refering to the amount of time I spent implementing
//                  the parser/loader rather than the speed at which it parses. Since the parsing is 
//                  simpler though there is a good chance it runs faster than a more robust parser. 
//
// Programmer:      Forrest Miller
// Dates:           10/23/2018      -- Got a rough version together
//                  11/28/2018      -- Performed heavy revision to get this RenderDemo up to snuff with the others.
//                                     Also rewrote all of this RenderDemo's shaders.


#pragma once


#ifndef	ASSET_LOADING_DEMO_H_
#define ASSET_LOADING_DEMO_H_

#include <thread>
#include <math.h>
#include <functional>   //std::function

#include "ProjectConstants.h"
#include "ProjectParameters.h"
#include "MathFunctions.h"
#include "MeshFunctions.h"
#include "GLFW_Init.h"
#include "ProjectSetup.h"
#include "RelativeFilepathsToResources.h"
#include "ShaderProgram.h"

#include "RenderDemoBase.h"
#include "QuickObj.h" //For loading '.obj' files



//The following variable dictates whether assets missing texture coordinates should have random coordinates assigned.
//Setting this variable to false will cause all generated texture coordinates to be the same point 
static constexpr const bool ASSIGN_TEXTURE_COORDS_RANDOMLY = true;

static constexpr const GLsizei STARTING_INSTANCE_COUNT = 5u;
static constexpr const GLfloat STARTING_INSTANCE_SPIRAL_PATTERN_PERIOD_X = 3.0f;
static constexpr const GLfloat STARTING_INSTANCE_SPIRAL_PATTERN_PERIOD_Y = 3.0f;


static constexpr const unsigned long long FRAMES_TO_WAIT_BEFORE_CHECKING_TO_UPDATE_SHADERS = 60ull;


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
	unsigned long long frameNumber;
	unsigned long long frameUnpaused, frameLineTypeLastSwitched, frameInstancedDrawingBehaviorLastToggled, 
		frameInstancedDrawingCountLastModified, frameTimeFreezeLastToggled;

	glm::vec3 backgroundColor;

	GLuint vao, vbo; 

	//Variables that are modifiable by input from the user
	PIPELINE_PRIMATIVE_INPUT_TYPE currentPrimativeInputType;
	bool drawMultipleInstances; //For trying out glDrawArraysInstanced() vs plain old glDrawArrays();
	GLsizei instanceCount;
	GLfloat instanceSpiralPatternPeriod_x, instanceSpiralPatternPeriod_y;

	bool freezeTimeToggle; 
	

	//Scene Control Variables
	std::unique_ptr<ShaderProgram> sceneShader;
	std::vector<std::unique_ptr<QuickObj>> sceneObjects;
	std::vector<GLfloat> sceneBuffer;

	//For dynamic shader recompilation
	struct DynamicShaderSet {
		FilepathWrapper file;
		bool primary;
		ShaderInterface::ShaderType type;
		DynamicShaderSet() = delete;
		DynamicShaderSet(std::string s, bool isPrimary, ShaderInterface::ShaderType type) : file(s), primary(isPrimary), type(type) { ; }
	};
	std::vector<DynamicShaderSet> shaderSources; //Vector of filepaths to shaders for checking for updates
	std::unique_ptr<ShaderProgram> backupSceneShader; //Used for building a new sceneShader once files are updated
	

	//View parameters
	glm::vec3 cameraPos;
	glm::vec3 lookAtOrgin;
	glm::vec3 upDirection;
	float xTranslation;
	float yTranslation;
	//Projection parameters
	float fov, screenHeight, screenWidth, zNear, zFar;   //see https://glm.g-truc.net/0.9.2/api/a00245.html
	//Shader Uniforms
	glm::mat4 rotation;
	glm::mat4 view;
	glm::mat4 perspective; //Computes from Projection parameters
	float head, pitch, roll;
	float zoom;
	
	
	///////////////////////////////////////////////////////
	/////////////      Setup Functions      ///////////////
	///////////////////////////////////////////////////////
	
	void initialize(); //Called by constructor(s)
	

	//This function expects each vertex in sceneBuffer to be exactly 9 components
	GLsizei computeNumberOfVerticesInSceneBuffer() const;


	void loadShaders(); //Sets up the sceneShader
	void loadModels(); //Loads 3D model data from asset files
	

	//This function is meant to be called after the sceneShader is linked and 
	//all models for the scene have finished loading
	void prepareScene();


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
	bool checkIfShouldReset() const;


	/*						   +~~~~~~~~~~~~~~~~~~~~~~~~~~~+
							   |  (2)  Input Processing    |
							   +~~~~~~~~~~~~~~~~~~~~~~~~~~~+	        								 */

	void pause();
	void reset();
	void toggleTimeFreeze();
	void changePrimitiveType();
	void changeInstancedDrawingBehavior();
	void modifyInstancedDrawingSpiralPattern();
	void rotate();
	void translate();


	/*						    +~~~~~~~~~~~~~~~~~~~~~~~~~~~~+
							    |	 (3)   Handle Events	 |
							    +~~~~~~~~~~~~~~~~~~~~~~~~~~~~+			    			                 */
	
	bool checkForUpdatedShaders();
	void buildNewShader();






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
	///  (4d)  Make Draw Calls    ///                          
	/////////////////////////////////
	void drawVerts();


	
	
	/*						+~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+
							|	(5) Clean-up and Prepare for Next Frame     |
							+~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+			   			     */
	
	void prepareGLContextForNextFrame();







	/////////////////////////////////////////////////////////////////////////////////////////////
	////  The following are utility functions called by the setup and renderloop functions   ////
	/////////////////////////////////////////////////////////////////////////////////////////////

	//Sets up the sceneBuffer. Will fill in missing uvTexCoord and Normal vertex components
	//as needed to give every vertex in the sceneBuffer the same format. 
	void buildSceneBufferFromLoadedSceneObjects();

	//The following 4 functions are intended for use within the function
	//buildSceneBufferFromLoadedSceneObjects() to handle 
	//data into the sceneBuffer
	void addObject(std::vector<std::unique_ptr<QuickObj>>::iterator object,
		const glm::vec3& objPos);
	void addObjectWithMissingNormals(std::vector<std::unique_ptr<QuickObj>>::iterator object,
		const glm::vec3& objPos);
	void addObjectWithMissingTexCoords(std::vector<std::unique_ptr<QuickObj>>::iterator object,
		const glm::vec3& objPos);
	void addObjectWithMissingTexCoordsAndNormals(std::vector<std::unique_ptr<QuickObj>>::iterator object,
		const glm::vec3& objPos);

	//Helper function for generating random texture coordinates
	static inline glm::vec2 generateRandomTexCoords() {
		return glm::vec2(MathFunc::getRandomInRangef(0.0f, 1.0f), MathFunc::getRandomInRangef(0.0f, 1.0f));
	}
	//Helper function for generating constant texture coords
	static inline glm::vec2 generateConstantTexCoords() {
		return glm::vec2(0.5f, 0.5f);
	}
	

	//Does exactly what you think [creates buffers within the GL Context, uploads vertices to buffer]
	void uploadSceneBufferToGPU();
	//Sets up the VAO [which describes how the vertex data is arranged in the VertexArrayBuffer]
	void configureVertexArrayAttributes(); 

};


#endif //ASSET_LOADING_DEMO_H_
