
//
//
//-------------------------------------------------------------------------------------------
//   Updated and more-complete details on this RenderDemo can be found in the implementation file
//    "LightsourceTestDemo.cpp". The 
//    implementation file is also where compile-time settings are defined/declared.
//-------------------------------------------------------------------------------------------
//
//

//            Original (incomplete) Description
//This file contains the driver for some experimenting I did
//with rendering a light source. This also is the first time I 
//use noise() functions in my shader code. I learned quite a bit from
//this demo, but I plan to do a better version in the near future
//where I use alpha testing and blending and possibly other features
//more properly. This demo has some basic blending/alpha testing tacked
//on after most everything was in place. 
//
//  INPUT:  -Press 'Q', 'W', "E", "R", "T", "Y", (and possibly, if I add more frag shader options, "U", "I", "O", "P")
//             to cycle between different noise shaders and color schemes. 
//          -Press SPACE to pause and ESC to exit
//          -Press TAB to reset 
//          -'D' to toggle Dither   //Won't be noticable at high res... Dither was mostly used back in the 90's 
//          -'B' to toggle blending
//          -'S' to toggle polygon smoothing
//
//    (on some effects)
//          - 'G' to toggle a color shift
//
// Programmer:   Forrest Miller
// Date:         11/2/2018 - 11/12/2018  (or so)  (it works out to about 3-4 days of work implementing then experimenting)

#pragma once

#ifndef	LIGHTSOURCE_TEST_DEMO_H_
#define LIGHTSOURCE_TEST_DEMO_H_

#include <thread>
#include <math.h>

#include "GlobalConstants.h"
#include "GlobalIncludes.h"

#include "MathFunctions.h"
#include "RelativeFilepathsToResources.h" //Contains asset filepath locations
#include "ShaderProgram.h"
#include "RenderDemoBase.h"
#include "LightsourceTest.h"



class LightsourceTestDemo : public RenderDemoBase { 
public:
	LightsourceTestDemo() = delete;
	LightsourceTestDemo(InitReport*);
	virtual ~LightsourceTestDemo() noexcept override;

	virtual void loadAssets() override;
	virtual void run() override;
	
private:
	bool error;
    //float counter;  //Counter was moved to be a member of the base class 'RenderDemoBase'
	unsigned long long frameNumber, frameUnpaused;
	unsigned long long frameDitherLastToggled, frameBlendLastToggled, framePolygonSmoothLastToggled;
	unsigned long long frameColorshiftLastToggled;


	glm::vec3 backgroundColor;

	GLuint vao, vbo, ebo;
	size_t eboSize;
	int colorShift;

	
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

    void updateZoom();

	void changeNoiseType();
	void togglePipelineEffects();
	void toggleDither();
	void toggleBlend();
	void togglePolygonSmooth();
	void toggleColorshift();

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