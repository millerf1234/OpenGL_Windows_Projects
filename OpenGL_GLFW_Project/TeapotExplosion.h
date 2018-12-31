//This file contains a class that is based upon my previous GeometryShaderExplosion class,
//the differrence between the two being that this class uses RenderDemoBase to provide
//the interface for the outside calling Application. 
//
//The original GeometryShaderExplosion class was based off the code example at
//  http://web.engr.oregonstate.edu/~mjb/cs519/Handouts/geometry_shaders.6pp.pdf
//      (Slides 29-32) 
//
// References:
//             - This RenderDemo draws 'point' primitives. While each 'point'
//                  is not texture mapped (currently) and thus doesn't qualify
//                  as a 'point sprite', this RenderDemo does make use of several
//                  of the available point features such as variable point size
//                  and point - relative fragment coordinates. For more detail on 
//                  the available options and considerations of rendering with point
//                  primitives, see: 
//                      https://www.khronos.org/opengl/wiki/Primitive#Point_primitives
// 
//
// Behind The Scenes:    
//                This RenderDemo was the first RenderDemo to be created over the course
//                  of this project. This demo underwent a lot of its development in tandem
//                  with the utility code, and was the first runnable demo that served as 
//                  the proving ground for a big chunk of this Application's basic infrastructure 
//                  (consisting mostly of the code found within the 'ShaderInterface' 
//                  namespace). This demo was written before there was any capabilities 
//                  of loading data from files, so it pulls its model data from the 
//                  header file "teapot.h" containing a static array of vertex positions
//                  for the classic Utah Teapot. 
//
//                 
//                  
//
// Programmer:      Forrest Miller
// Date:            Sept-Oct 2018 
// 
//                  Beyond October:
//                    Slight modifications have been made to this demo to keep it compatible as 
//                    the underlying utility code driving it morphs and grows.

#pragma once

#ifndef TEAPOT_EXPLOSION_H_
#define TEAPOT_EXPLOSION_H_

//#include <thread>

#include "ProjectConstants.h"
#include "ProjectParameters.h"
#include "MathFunctions.h"
#include "GLFW_Init.h"
#include "ProjectSetup.h"

#include "RelativeFilepathsToResources.h" //Contains asset filepath locations

#include "ShaderProgram.h"
#include "TeapotExplosionDemo_GenericVertexAttributeSet.h"

#include "teapot.h"  //From the internet, is a file containing the vertices for a teapot

#include "RenderDemoBase.h"


//For testing:
#include "AsciiAsset.h"
#include "Face.h"


using ShaderInterface::TeapotExplosionDemo_GenericVertexAttributeSet; 


static constexpr int COLOR_MOD_VALUES_COUNT = 7;
static constexpr float COLOR_MOD_VALUE_CHANGE_SPEED = 0.15f;

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
	float velocity;
	glm::vec3 backgroundColor;

	PIPELINE_PRIMATIVE_INPUT_TYPE currentTriangleInputType;

	float colorChangeThreshold;
	GLfloat colorModificationValues[COLOR_MOD_VALUES_COUNT];

	//The GLFWwindow pointer is a protected member of the RenderDemoBase class 
	//GLFWwindow * mainRenderWindow; //Pointer to target renderable mainRenderWindow (Application should provide this)

	std::unique_ptr<ShaderProgram> sceneShader; 
	std::unique_ptr<ShaderProgram> sceneShaderLine;
	std::unique_ptr<TeapotExplosionDemo_GenericVertexAttributeSet> vertexAttributes;


	

	
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
	void updateVelocity();



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