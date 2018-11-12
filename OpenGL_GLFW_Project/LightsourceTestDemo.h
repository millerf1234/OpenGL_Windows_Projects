//This file contains the driver for some experimenting I did
//with rendering a light source. This also is the first time I 
//use noise() functions in my shaders. I learned quite a bit from
//this demo, but I plan to do a better version in the near future
//where I use alpha testing and blending and possibly other features
//more properly. This demo has some basic blending/alpha testing tacked
//on after most everything was in place. 
//
//  INPUT:  -Press 'Q', 'W', "E", "R", "T", "Y", (and possibly, if I add more frag shader options, "U", "I", "O", "P")
//             to cycle between different noise shaders and color schemes. 
//          -Press SPACE to pause and ESC to exit
//          -Press TAB to reset 
//          -'D' to toggle Dither
//          -'B' to toggle blending
//          -'S' to toggle polygon smoothing
//
// Programmer:   Forrest Miller
// Date:         11/2/2018 - 11/12/2018  (or so)

#pragma once

#ifndef	LIGHTSOURCE_TEST_DEMO_H_
#define LIGHTSOURCE_TEST_DEMO_H_

#include <thread>
#include <math.h>

#include "ProjectConstants.h"
#include "ProjectParameters.h"
#include "MathFunctions.h"
#include "GLFW_Init.h"
#include "ProjectSetup.h"
#include "ProjectResourceFilepaths.h"
#include "ShaderProgram.h"
#include "RenderDemoBase.h"
#include "LightsourceTest.h"



class LightsourceTestDemo : public RenderDemoBase { 
public:
	LightsourceTestDemo() = delete;
	LightsourceTestDemo(std::shared_ptr<MonitorData> screenInfo);
	virtual ~LightsourceTestDemo() override;

	virtual void loadAssets() override;
	virtual void run() override;
	
private:
	bool error;
	float counter;
	unsigned long long frameNumber, frameUnpaused;
	unsigned long long frameDitherLastToggled, frameBlendLastToggled, framePolygonSmoothLastToggled;


	glm::vec3 backgroundColor;

	GLuint vao, vbo, ebo;
	size_t eboSize;

	
	//Shader Program 
	std::unique_ptr<ShaderProgram> lightSourceShader;
	
	//Model Generator Object
	std::unique_ptr<LightEmitterSource> testLightEmitter;
	

	int noiseFunctionToUse, noiseResolution;

	//Asset Transformation information
	float zoom;
	float zRotation;
	
	bool useDither;
	bool useBlend;
	bool usePolygonSmooth;
	

	//   Helper Functions
	void initialize(); 
	void cleanup();
	
	void loadModels();
	void loadShaders();
	void configureContext();

	
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
	
	bool checkToSeeIfShouldCloseWindow() const; // 'esc'
	bool checkIfShouldPause() const; // 'space'
	bool checkIfShouldReset() const; // 'tab'


	/*						   +~~~~~~~~~~~~~~~~~~~~~~~~~~~+
							   |  (2)  Input Processing    |
							   +~~~~~~~~~~~~~~~~~~~~~~~~~~~+	        								 */

	void pause();
	void reset();


	void changeNoiseType();
	void togglePipelineEffects();
	void toggleDither();
	void toggleBlend();
	void togglePolygonSmooth();

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
	///  (4d)  Make Draw Calls    ///                       
	/////////////////////////////////
	void drawVerts();


	
	
	/*						+~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+
							|	(5) Clean-up and Prepare for Next Frame     |
							+~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+			   			     */
	
	void prepareGLContextForNextFrame();
};





#endif //LIGHTSOURCE_TEST_DEMO_H_