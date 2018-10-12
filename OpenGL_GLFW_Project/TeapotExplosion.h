//This file contains a class that is based upon my previous GeometryShaderExplosion class,
//the differrence between the two being that this class uses RenderDemoBase to provide
//the interface for the outside calling Application. 
//
//The original GeometryShaderExplosion class was based off the code example at
//  http://web.engr.oregonstate.edu/~mjb/cs519/Handouts/geometry_shaders.6pp.pdf
//      (Slides 29-32) 

#pragma once

#ifndef TEAPOT_EXPLOSION_H_
#define TEAPOT_EXPLOSION_H_

#include <thread>

#include "ProjectConstants.h"
#include "ProjectParameters.h"
#include "MathFunctions.h"
#include "GLFW_Init.h"
#include "ProjectSetup.h"

#include "ProjectResourceFilepaths.h"

#include "ShaderProgram.h"
#include "GenericVertexAttributeSet.h"

#include "teapot.h"  //From the internet, is a file containing the vertices for a teapot

#include "RenderDemoBase.h"

#include "FilepathWrapper.h"

using ShaderInterface::GenericVertexAttributeSet; 

enum class PIPELINE_TRIANGLE_INPUT_TYPE { NORMAL, STRIP, FAN };

static constexpr int COLOR_MOD_VALUES_COUNT = 7;
static constexpr float COLOR_MOD_VALUE_CHANGE_SPEED = 0.3f;

class TeapotExplosion : public RenderDemoBase {
public:
	TeapotExplosion() = delete;
	TeapotExplosion(std::shared_ptr<MonitorData> screenInfo);
	virtual ~TeapotExplosion() override;

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

	float colorChangeThreshold;
	GLfloat colorModificationValues[COLOR_MOD_VALUES_COUNT];

	//The GLFWwindow pointer is a protected member of the RenderDemoBase class 
	//GLFWwindow * window; //Pointer to target renderable window (Application should provide this)

	std::unique_ptr<ShaderProgram> sceneShader; 
	std::unique_ptr<ShaderProgram> sceneShaderLine;
	std::unique_ptr<GenericVertexAttributeSet> vertexAttributes;


	

	
	//   Helper Functions
	void initialize(); 

	void loadTeapot();

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


	//1 and 2 in same function:
	void changePrimitiveType(); 
	void modifyColorThreshhold();
	void rotateColor();
	void updateColorModificationValues();



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





#endif //TEAPOT_EXPLOSION_H_