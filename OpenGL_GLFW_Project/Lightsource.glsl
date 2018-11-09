//
// UPDATE: This shader does not work at all as it originally was
//         intended. From the GLSL documentation I read part of it
//         that said the invocations of geometry shaders are guarenteed
//         to execute in-order.
// 
//         What I was trying to do here would be more efficently done on
//         the CPU anyways...
//
//


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
#define NUM_INVOCATIONS 1     //Must be between 1 and 32  
#define POLYGON_CORNERS 6 
//#define DO_SIX_CORNERS_AS_SINGLE_STRIP  //Use the single-strip hexagon with degenerate triangles instead of regular one
#define CENTER_RADIUS 0.025   
#define EXTERIOR_RADII_BASE 0.055
#define EXTERIOR_RADII_GROWTH_FACTOR 1.015
#define COLOR_HUE_FALLOFF_RATE 0.2    //Formula for hue drop per outward invocation group is:  hue = hue * exp(-falloffRate * Invocation). So it's logarithmic   
#define BASE_INSTABILITY 0.0     //Can be made positive or negative, controls the base amount of noise 

// (Parameters For FBM Noise)
#ifndef NUM_OCTAVES
#define NUM_OCTAVES 3     //Change this value to modify the FBM noise
#endif 


///////////////////////////////////////////////////////////////////////////
//CONSTANT VARIABLES
//     These variables are directly tied to the mathematical behavior of
//     the shader and should not be modified to avoid catasrophic 
//     consequences.
///////////////////////////////////////////////////////////////////////////
//const int mPOLYGON_CORNERS_ = max(int(floor(POLYGON_CORNERS)), 4);
const int mPOLYGON_CORNERS_ = clamp(int(floor(POLYGON_CORNERS)), 4, 9);  //Only values between 4-9 will be accepted
const float PI = 3.14159;
const float mCOLOR_HUE_FALLOFF_RATE_LOWER_BOUND_ = 0.04;
//Noise function codes (see uniforms)
const int USE_GENERIC_2D = 0;
const int USE_PERLIN_2D = 1;
const int USE_SIMPLEX_2D = 2;
const int USE_FBM_1D = 3;
//constexpr int USE_FBM_2D = 4; //As for now, any value besides 0-3 will be FBM_2D


///////////////////////////////////////////////////////////////////////////
// GEOMETRY SHADER INPUT/OUTPUT PRIMATIVES     ( Note that input primative must be GL_POINTS! )
///////////////////////////////////////////////////////////////////////////
layout ( points, invocations = NUM_INVOCATIONS) in;
layout ( triangle_strip, max_vertices = (mPOLYGON_CORNERS_ * 2 + 1) ) out; //All invocations beyond the first will emit this limit of vertices


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
void emitCenterVertex(vec3 center, vec3 color); 
void emitInteriorVertex(vec3 center, vec2 offset, vec3 diminishedHue); //It turns out creating the enterior polygon with triangle stips is impossible without making a degenerate triangle
void emitCornerVertex(vec3 center, vec2 offset, vec3 diminishedHue);


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
const float offsetMultiplier = (2.0 * PI) / ( float(mPOLYGON_CORNERS_) );

//-Ensure hueFalloffRate is above the specified lower bound
const float hueFalloffRate = max(float(COLOR_HUE_FALLOFF_RATE), mCOLOR_HUE_FALLOFF_RATE_LOWER_BOUND_); 


void main() {
	vec3 center = gl_Position.xyz;//lightPosition[0];
	vec3 hue = lightColor[0]; //= clamp(lightColor[0], 0.0, 1.0);

	vec2 evenIterationOffsets[POLYGON_CORNERS * NUM_INVOCATIONS]; //The following arrays are used whilest building outwards
	vec2 oddIterationOffsets[POLYGON_CORNERS * NUM_INVOCATIONS];
	if (gl_InvocationID == 0) { //If this is the first invocation of the geometry shader

		//Construct the array of inner polygon corners
		for (int i = 0; i < POLYGON_CORNERS; i++) {
			float x = CENTER_RADIUS * cos(offsetMultiplier * float(i));
			float y = CENTER_RADIUS * sin(offsetMultiplier * float(i));
			float noise = 1.0 - abs(computeNoise(vec2(x, y), 60));
			evenIterationOffsets[i] = center.xy + abs(((1.0 + noise))) * vec2(x, y);
		}

		vec3 diminishedHue = hue / exp(hueFalloffRate);   // = 1/e^(x*rate), where x is shader invocation #

		//Build the interior polygon [it turns out this is impossible using triangle strips, so I am going to try to fudge it]
		//Build polygons on a case-by-case basis for the interior
#if (POLYGON_CORNERS < 4)
		emitCenterVertex(center);
		emitCornerVertex(center, computeNoise(vec2(time, 1.0 / time), noiseResolution, hue - (0.5 * computeNoise(vec2(hue, diminishedHue))));
		emitCornerVertex(center, center.xy * 1.1, diminishedHue);
		EndPrimitive();
#elif (POLYGON_CORNERS == 4) 
		emitCornerVertex(center, evenIterationOffsets[0], diminishedHue);
		emitCornerVertex(center, evenIterationOffsets[1], diminishedHue);
		emitCenterVertex(center, hue);
		emitCornerVertex(center, evenIterationOffsets[2], diminishedHue);
		emitCornerVertex(center, evenIterationOffsets[3], diminishedHue);
		EndPrimitive();
		emitCornerVertex(center, evenIterationOffsets[3], diminishedHue);
		emitCenterVertex(center, hue);
		emitCornerVertex(center, evenIterationOffsets[0], diminishedHue);
		EndPrimitive();
#elif (POLYGON_CORNERS == 5)

#elif ((POLYGON_CORNERS == 6) && (defined DO_SIX_CORNERS_AS_SINGLE_STRIP))

		//This is for a 6 sided ngon. See: https://howevangotburned.wordpress.com/2011/02/13/how-to-draw-a-regular-hexagon-as-a-triangle-strip/
		//Also note that I do not do it this way. Instead I have the 5th and 6th vertex [4th and 5th if counting from 0] being 
		//located at almost the exact same coordinates. This leads to two degenerate triangles in the model. 
		emitCornerVertex(center, evenIterationOffsets[0], diminishedHue);
		emitCornerVertex(center, evenIterationOffsets[1], diminishedHue);
		emitCenterVertex(center, hue);
		emitCornerVertex(center, evenIterationOffsets[2], diminishedHue);
		emitCornerVertex(center, evenIterationOffsets[3], diminishedHue);
		emitCornerVertex(center, 1.001*evenIterationOffsets[3], diminishedHue); //This is the fudge step
		emitCornerVertex(center, evenIterationOffsets[4], diminishedHue);
		emitCenterVertex(center, hue);
		emitCornerVertex(center, evenIterationOffsets[5], diminishedHue);
		emitCornerVertex(center, evenIterationOffsets[0], diminishedHue);
		EndPrimitive();

#elif (POLYGON_CORNERS == 6)
		emitCornerVertex(center, evenIterationOffsets[0], diminishedHue);
		emitCornerVertex(center, evenIterationOffsets[1], diminishedHue);
		emitCenterVertex(center, hue);
		emitCornerVertex(center, evenIterationOffsets[2], diminishedHue);
		emitCornerVertex(center, evenIterationOffsets[3], diminishedHue);
		EndPrimitive();
		emitCornerVertex(center, evenIterationOffsets[0], diminishedHue);
		emitCornerVertex(center, evenIterationOffsets[5], diminishedHue);
		emitCenterVertex(center, hue);
		emitCornerVertex(center, evenIterationOffsets[4], diminishedHue);
		emitCornerVertex(center, evenIterationOffsets[3], diminishedHue);
		EndPrimitive();
#elif (POLYGON_CORNERS == 7) 

#elif (POLYGON_CORNERS == 8) 

#else //9 is as high as I am going to go for now

#endif
	}
	else { //It is an invocation beyond the first
		vec3 diminishedHue = hue - hue * exp(-hueFalloffRate);

		if ((gl_InvocationID % 2) != 0) {
			float invocationRadius = (float(CENTER_RADIUS) + float(EXTERIOR_RADII_BASE)*float(EXTERIOR_RADII_GROWTH_FACTOR)*float(gl_InvocationID));
			for (int i = 0; i < POLYGON_CORNERS; i++) {
				float x = invocationRadius * cos(offsetMultiplier * float(i));
				float y = invocationRadius * sin(offsetMultiplier * float(i));
				float noise = computeNoise(vec2(x, y), 1);
				oddIterationOffsets[i*gl_InvocationID] = center.xy + ((1.0 + abs(noise)) * vec2(x, y));
			}

			for (int i = 0; i < 10; i++) {
				emitCornerVertex(center, evenIterationOffsets[0], diminishedHue);
				emitCornerVertex(center, oddIterationOffsets[0], diminishedHue);
				emitCornerVertex(center, evenIterationOffsets[1], diminishedHue);
				emitCornerVertex(center, oddIterationOffsets[1], diminishedHue);
				emitCornerVertex(center, evenIterationOffsets[2], diminishedHue);
				emitCornerVertex(center, oddIterationOffsets[2], diminishedHue);
				emitCornerVertex(center, evenIterationOffsets[1], diminishedHue);
				emitCornerVertex(center, oddIterationOffsets[2], diminishedHue);
				EndPrimitive();
			}
			
		}
		else {
			float invocationRadius = (float(CENTER_RADIUS) + float(EXTERIOR_RADII_BASE)*float(EXTERIOR_RADII_GROWTH_FACTOR)*float(gl_InvocationID));
			for (int i = 0; i < POLYGON_CORNERS; i++) {
				float x = invocationRadius * cos(offsetMultiplier * float(i));
				float y = invocationRadius * sin(offsetMultiplier * float(i));
				float noise = computeNoise(vec2(x, y), 1);
				evenIterationOffsets[i*gl_InvocationID] = center.xy + ((1.0 + abs(noise)) * vec2(x, y));
			}

			for (int i = 0; i < 10; i++) {
				emitCornerVertex(center, oddIterationOffsets[0], diminishedHue);
				emitCornerVertex(center, evenIterationOffsets[0], diminishedHue);
				emitCornerVertex(center, oddIterationOffsets[1], diminishedHue);
				emitCornerVertex(center, evenIterationOffsets[1], diminishedHue);
				emitCornerVertex(center, oddIterationOffsets[2], diminishedHue);
				emitCornerVertex(center, evenIterationOffsets[2], diminishedHue);
				emitCornerVertex(center, oddIterationOffsets[1], diminishedHue);
				emitCornerVertex(center, evenIterationOffsets[2], diminishedHue);
				EndPrimitive();
			}
		}
	}
}

//Computes a noise value from the position with modifiers applied based off the
//level of the vertex outwords from the center and from the uniform values which 
//affect the noise
float computeNoise(vec2 pos, int level) {
	//meh
	return (snoise(pos) - pNoise(pos, level));

	
	////return  fbm(pos);
	//
	//float computedInstability = 0.0;
	//for (int i = 0; i < level; i++) {
	//	computedInstability += 
	//}

	////Select which noise function to use
	//if (USE_GENERIC_2D == noiseFunctionToUse) {
	//	noise = 
	//}
	////constexpr int USE_GENERIC_2D = 0;
	////constexpr int USE_PERLIN_2D = 1;
	////constexpr int USE_SIMPLEX_2D = 2;
	////constexpr int USE_FBM_1D = 3;

	//float(BASE_INSTABILITY) * pNoise(vec2(x, y)); 

	
}



//Create a vertex at the center of the lightsource
void emitCenterVertex(vec3 center, vec3 color) {
	vertexPosition = center; //Start at the center of the light source
	vertexColor = color; 
	gl_Position = vec4(center, zoom);
	EmitVertex();
}

//Special function to facilitate the drawing of the polygon interior for the first invocation
void emitInteriorVertex(vec3 center, vec2 offset, vec3 diminishedHue) {
	vertexPosition = center + vec3(offset, 0.0);
	vertexColor = diminishedHue;
	gl_Position = vec4((center + vec3(offset, computeNoise(offset, 5))), zoom);
	EmitVertex();
}



void emitCornerVertex(vec3 center, vec2 offset, vec3 diminishedHue) {
	vertexPosition = center + vec3(offset, computeNoise(offset, 3));
	vertexColor = diminishedHue;
	gl_Position = vec4(vertexPosition, zoom); //vec4((center + vec3(offset, 0.0)), 1.0); //zoom);
	EmitVertex();
}
