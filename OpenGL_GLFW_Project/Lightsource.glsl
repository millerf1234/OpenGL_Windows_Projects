//File: Lightsource.geom
//
//Description: Takes in a vert position and creates a more detailed shape
//             at that location. This will look something like a billboarded 
//             polygon centered at the lights location, but with multiple layers
//             extending outwords with a drop in color hue/intensity. The center
//             of the polygon is defined as level 0.
//
// To Modify:  Try changing some of the values declared using the '#define' syntax. 
// 
// 
// References:  This shader uses one (or more) of the collections of noise functions 
//              found at:  https://gist.github.com/patriciogonzalezvivo/670c22f3966e662d2f83
//             
//             
// Dependencies: This shader should be linked with the ShaderNoiseFunctions.glsl shader
//               which provides the definitions for all of the noise functions used in this 
//               shader.
//
// Programmer: Forrest Miller
// Date:       November 6, 2018

#version 450 core

//////////////////////////////////////////////////////////////////////////
//CONSTANT PARAMETERS
//       These values dictate how the geometry shader will operate. It should
//       be safe to modify their values, within reason. 
//////////////////////////////////////////////////////////////////////////
#define NUM_INVOCATIONS 16        
#define POLYGON_CORNERS 6  
#define CENTER_RADIUS 0.03   
#define EXTERIOR_RADII_BASE 0.005
#define EXTERIOR_RADII_GROWTH_FACTOR 1.5
#define COLOR_HUE_FALLOFF_RATE 2.2         
#define BASE_INSTABILITY 0.0     //Can be made positive or negative

// (Parameters For FBM Noise)
#ifndef NUM_OCTAVES
#define NUM_OCTAVES 7     //Change this value to modify the FBM noise
#endif 


///////////////////////////////////////////////////////////////////////////
//CONSTANT VARIABLES
//     These variables are directly tied to the mathematical behavior of
//     the shader and should not be modified to avoid catasrophic 
//     consequences.
///////////////////////////////////////////////////////////////////////////
constexpr int mPOLYGON_CORNERS_ = clamp(int(floor(POLYGON_CORNERS)), 4, 9);  //Only values between 4-9 will be accepted
constexpr float PI = 3.14159;
constexpr float mCOLOR_HUE_FALLOFF_RATE_LOWER_BOUND_ = 0.04;
//Noise function codes (see uniforms)
constexpr int USE_GENERIC_2D = 0;
constexpr int USE_PERLIN_2D = 1;
constexpr int USE_SIMPLEX_2D = 2;
constexpr int USE_FBM_1D = 3;
//constexpr int USE_FBM_2D = 4; //As for now, any value besides 0-3 will be FBM_2D


///////////////////////////////////////////////////////////////////////////
// GEOMETRY SHADER INPUT/OUTPUT PRIMATIVES     ( Note that input primative must be GL_POINTS! )
///////////////////////////////////////////////////////////////////////////
layout ( points, invocations = NUM_INVOCATIONS) in;
layout ( triangle_strip, max_vertices = (mPOLYGON_CORNERS_ * 2) ) out; //All invocations beyond the first will emit this limit of vertices


///////////////////////////////////////////////////////////////////////////
// INPUT FROM VERTEX SHADER
///////////////////////////////////////////////////////////////////////////
in vec3 lightPosition[];
in vec3 lightColor[];


///////////////////////////////////////////////////////////////////////////
// OUTPUT DATA TO SEND ON TO FRAGMENT SHADER
///////////////////////////////////////////////////////////////////////////
out vec3 vertexPosition;
out vec3 vertexColor;

///////////////////////////////////////////////////////////////////////////
// UNIFORMS
///////////////////////////////////////////////////////////////////////////
uniform float time;
uniform float zoom; //uniform float distanceToCamera;
uniform int noiseFunctionToUse; //'0' for generic 2d, '1' for perlin , '2' for simplex, '3' for 1d FBM, any other value for 2d FBM 
uniform int noiseResolution; //Will only have an effect when using perlin noise
uniform float instabilityFactor; //i.e. noise amplitude increase as moving away from center


///////////////////////////////////////////////////////////////////////////
// EXTERNAL FUNCTION PROTOTYPES  [for noise]
///////////////////////////////////////////////////////////////////////////
float noise(vec2 p);           //   Generic 2d noise       
float pNoise(vec2 p, int res); //2d Perlin
float snoise(vec2 v);          //2d simplex noise
float fbm(float x);            //1d Fractal Brownian Motion
float fbm(vec2 x);             //2d Fractal Brownian Motion

///////////////////////////////////////////////////////////////////////////
// INTERNAL HELPER FUNCTION PROTOTYPES
///////////////////////////////////////////////////////////////////////////
float computeNoise(vec2 pos, int level); //Choses which noise function to use and returns the computed noise
void emitCenterVertex(); 
void emitInteriorVertex(vec2 offset, vec3 diminishedHue); //It turns out creating the enterior polygon with triangle stips is impossible without making a degenerate triangle
void emitCornerVertex(in vec2 cornerOffset, int level);


///////////////////////////////////////////////////////////////////////////
//  CONSTEXPR PRE-CALCULATION
///////////////////////////////////////////////////////////////////////////
//-Polygon Corner Pre-Calculation:
//    To compute the xy coords of the corners of any NGon of >3 sides, the following
//      formula is used:
//             x = cos (2*PI*k / NUM_SIDES),   y = sin(2*PI*k / NUM_SIDES),   k = 0,1,2,...,(NUM_SIDES-1)
//
//    Note that in these forumlas there is a constant of (2*PI / NUM_SIDES) throughout.
//
// Let's go ahead and calculate this constant ahead of time:
constexpr float offsetMultiplier = (2.0 * PI) / ( float(mPOLYGON_CORNERS_) );

//-Ensure hueFalloffRate is above the specified lower bound
constexpr float hueFalloffRate = max(float(COLOR_HUE_FALLOFF_RATE), mCOLOR_HUE_FALLOFF_RATE_LOWER_BOUND_); 




//The following global variables are used
vec3 center;
vec3 hue;
vec2 evenIterationOffsets[CENTER_POLYGON_CORNERS]; //The following arrays are used whilest building outwards
vec2 oddIterationOffsets[CENTER_POLYGON_CORNERS]; 


void main() {
	
	if (gl_InvocationID == 0) { //If this is the first invocation of the geometry shader
		
		//initialize global variables at start of first invocation
		vec3 center = lightPosition[0];  //Set the centerpoint for the light polygon
		vec3 hue = clamp(lightColor[0], 0.0, 1.0);
		
		//Construct the array of inner polygon corners
		for (int i = 0; i < CENTER_POLYGON_CORNERS; i++) {
			float x = centerRadius * cos(offsetMultiplier * float(i));
			float y = centerRadius * sin(offsetMultiplier * float(i));
			float noise = computeNoise(vec2(x,y), 1);
			evenIterationOffsets[i] = center + ((1.0 + noise) * vec3(x, y, 0.0));
		}
		
		float diminishedHue = hue / exp(hueFalloffRate);   // = 1/e^(x*rate), where x is shader invocation #

		//Build the interior polygon [it turns out this is impossible using triangle strips, so I am going to try to fudge it]
		//Build polygons on a case-by-case basis for the interior
#if (POLYGON_CORNERS <= 4) 
	
#elif (POLYGON_CORNERS == 5)

#elif (POLYGON_CORNERS == 6)
		//This is for a 6 sided 
		emitCornerVertex(evenIterationOffsets[0], diminishedHue);
		emitCornerVertex(evenIterationOffsets[1], diminishedHue);
		emitCenterVertex(hue);
		emitCornerVertex(evenIterationOffsets[2], diminishedHue);
		emitCornerVertex(evenIterationOffsets[3], diminishedHue);
		emitCornerVertex(1.001*evenIterationOffsets[3], diminishedHue); //This is the fudge step
		emitCornerVertex(evenIterationOffsets[4], diminishedHue);
		emitCenterVertex(hue);
		emitCornerVertex(evenIterationOffsets[5], diminishedHue);
		emitCornerVertex(evenIterationOffsets[0], diminishedHue);


#elif (POLYGON_CORNERS == 7) 

#elif (POLYGON_CORNERS == 8) 

#else //9 is as high as I am going to go for now

#endif

	
	
}

//Computes a noise value from the position with modifiers applied based off the
//level of the vertex outwords from the center and from the uniform values which 
//affect the noise
float computeNoise(vec2 pos, int level) {
	float computedInstability = 0.0;
	for (int i = 0; i < level; i++) {
		computedInstability += 
	}

	//Select which noise function to use
	if (USE_GENERIC_2D == noiseFunctionToUse) {
		noise = 
	}
	//constexpr int USE_GENERIC_2D = 0;
	constexpr int USE_PERLIN_2D = 1;
	constexpr int USE_SIMPLEX_2D = 2;
	constexpr int USE_FBM_1D = 3;

	float(BASE_INSTABILITY) * pNoise(vec2(x, y)); 
}




//Create a vertex at the center of the lightsource
void emitCenterVertex() {
	vertexPosition = center; //Start at the center of the light source
	vertexCol = hue;
	gl_Position = vec4(center, zoom);
	EmitVertex();
}

//Special function to facilitate the drawing of the polygon interior for the first invocation
void emitInteriorVertex(vec2 offset, vec3 diminishedHue) {
	vertexPosition = center + vec3(offset, 0.0);
	vertexCol = diminishedHue;
	gl_Position = vec4((center + vec3(offset, 0.0)), zoom);
	EmitVertex();
}



void emitCornerVertex(inout vec2 cornerOffset, int level) {
	


}
