//An OpenGL Render Project

//Created by Forrest Miller on July 18, 2018

#pragma once

#ifndef RENDER_PROJECT_1_H_
#define RENDER_PROJECT_1_H_

#include <limits>
#include <thread>
#include <chrono>

#include "ProjectConstants.h"
#include "ProjectParameters.h"
#include "GLFW_Init.h"
#include "ProjectSetup.h"

#include "ProjectResourceFilepaths.h"

#include "TextEngine.h"

#include "ShaderProgram.h"
//#include "ShaderProgramBuilder.h"

#include "SimpleVertexBuffer.h"


class RenderProject1 {
public:
	RenderProject1() = delete;
	RenderProject1(std::shared_ptr<MonitorData> screenInfo);
	~RenderProject1();

	void run();
	void loadAssets();
	

private:
	bool error;
	GLFWwindow * window; //Pointer to target renderable window (Application should have this)
	unsigned long long frameNumber, frameUnpaused, frameOfMostRecentColorRecording;
	float counter;
	glm::vec3 backgroundColor;

	std::unique_ptr<ShaderInterface::SimpleVertexBuffer> vertices;
	std::unique_ptr<ShaderProgram> sceneShader; 


	std::unique_ptr<TextEngine> txtEngine;

	static int numTriangles;
	std::vector<GLfloat> triangleBase2D = { 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f };
	std::vector<GLfloat> triangles2D;
	//glm::mat4 rotationMat;

	
	//   Helper Functions
	void initialize(); 

	void createTriangles2D();

	void loadShaders();
	void createDataBuffer(); //Creates an OpenGL Data Buffer and puts Data into it
	void updateDataBuffer(); 

	


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
	bool checkIfShouldRecordColor() const; //records the current frames background clear color, probably 'p' or 'P'
	




	/*						   +~~~~~~~~~~~~~~~~~~~~~~~~~~~+
							   |  (2)  Input Processing    |
							   +~~~~~~~~~~~~~~~~~~~~~~~~~~~+	        								 */

	void pause();
	void recordColorToLog();






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

#endif //RENDER_PROJECT_1_H_











/////////////////////////////////////////////////////////////////////////////
//Extra comment style stash  (Basically just different formats of the above comment titles)
//     (ignore this for now, this should be DELETED eventually,
//		once a more standardized commenting style gets developed...)	  
////////////////////////////////////////////////////////////////////////////

//////////////////////////////
/// (1)  Input Detection   ///
//////////////////////////////



///////////////////////////////
/// (2)  Input Processing   ///
///////////////////////////////



///////////////////////////////////
/// (3)  Perform World Logic    ///
///////////////////////////////////



///////////////////
/// (4) Render  ///
///////////////////



/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+
|	(4a)  Background Color Update  |
+~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/



/*~~~~~~~~~~~~~~~~~~~~~~~~~~+
|	(4b)  Update Uniforms   |
+~~~~~~~~~~~~~~~~~~~~~~~~~~*/



/////////////////////////////////////////////////
/// (5) Clean-up and Prepare for Next Frame   ///
/////////////////////////////////////////////////

