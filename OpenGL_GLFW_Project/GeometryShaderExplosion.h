//This is a test of using a geometry shader for an explosion effect of a mesh.
//The shader code for this is based off the code example at
//  http://web.engr.oregonstate.edu/~mjb/cs519/Handouts/geometry_shaders.6pp.pdf
//      (Slides 29-32) 
//
// The application code also is based off the code I wrote for RenderProject1 
//

#pragma once

#ifndef GEOMETRY_SHADER_EXPLOSION_H_
#define GEOMETRY_SHADER_EXPLOSION_H_

#include <limits>
#include <thread>
#include <chrono>

#include "ProjectConstants.h"
#include "ProjectParameters.h"
#include "MathFunctions.h"
#include "GLFW_Init.h"
#include "ProjectSetup.h"

#include "ProjectResourceFilepaths.h"

#include "ShaderProgram.h"
#include "GenericVertexAttributeSet.h"

#include "teapot.h"  //From the internet, is a file containing the vertices for a teapot

static constexpr int EXPLOSION_LEVELS = 10;

class GeometryShaderExplosion {
public:
	GeometryShaderExplosion() = delete;
	GeometryShaderExplosion(std::shared_ptr<MonitorData> screenInfo);
	~GeometryShaderExplosion();

	void run();
	void loadAssets();
	

private:
	bool error;
	GLFWwindow * window; //Pointer to target renderable window (Application should provide this)
	unsigned long long frameNumber, frameUnpaused, frameOfMostRecentColorRecording;
	float counter;
	float zRotation;
	glm::vec3 backgroundColor;


	std::unique_ptr<ShaderProgram> sceneShader; 
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



#endif //GEOMETRY_SHADER_EXPLOSION_H_