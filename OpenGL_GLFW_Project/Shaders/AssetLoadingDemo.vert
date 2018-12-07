//Vertex Shader for the Asset Loading Demo
//
// Programmer:  Forrest Miller
// Date:        November 28, 2018

#version 450 core

layout(location = 0) in vec4 ModelPosition;
layout(location = 1) in vec2 ModelTexCoord;
layout(location = 2) in vec3 ModelNormal;

out vec4 position;
out vec2 texCoord;
out vec3 normal;

uniform float zoom;
uniform float time;
uniform mat4 rotation;

uniform mat4 MVP;

uniform float instanceSpiralPatternPeriod_x;  //Modifier for the x value of the pattern used when drawing instances 
uniform float instanceSpiralPatternPeriod_y;  //Modifier for the x value of the pattern used when drawing instances


#define pos position
#define vert float(gl_VertexID)
#define inst float(gl_InstanceID)

///////////////////////////////////////////////////////////////////////////
// EXTERNAL FUNCTION PROTOTYPES  [for noise]
///////////////////////////////////////////////////////////////////////////
float noise(vec2 p);           //   Generic 2d noise       
float pNoise(vec2 p, int res); //2d Perlin
float snoise(vec2 v);          //2d simplex noise
float cnoise(vec4 P);          //4d Periodic Classic Perlin Noise
float fbm(float x);            //1d Fractal Brownian Motion
float fbm(vec2 x);             //2d Fractal Brownian Motion
float fbm(vec3 x);             //3d Fractal Brownian Motion




void main() {

	//Keep things very simple for now...
	float radius = 10.0;
	float posFac = sqrt((pos.x + pos.y + pos.z) / 3.0);
	vec3 ns = vec3(1.0*pNoise(vec2(time, 0.0 - 0.0), 3), sin(posFac + time), cos(posFac + time));

	float cool = (sin(2.14*(time - (0.001*vert))));
	cool *= step(0.01, abs(cool));
	position = ModelPosition + vec4(cos(inst-cool), 1.0 / cool, inst / sin(cool), zoom);
	

	texCoord = ModelTexCoord;

	//Normally a seperate matrix is needed for transforming the normal vectors.
	// However this has not yet been implemented...
	normal = mat3(rotation) * ModelNormal; 

	//position.x += 

	gl_Position = MVP * position;
	


	//float noiz = 12.95*abs(1.0*pNoise(vec2(length(position) * 50.0 * sin(0.001*time), length(position) + sin(0.001*time) + 50.8*float(gl_InstanceID)), 3));
	//gl_Position.z += fbm(vec3(3.0*noiz, -30.0*noiz, noiz*sin(time*noiz)));
	//gl_Position.x += 3.0*cnoise(vec4(noiz, 1.0 + noiz, noiz * cos(3.0+noiz), 1.0));


	//vec4 startingOffset = zoom * vec4(-20.0, -20.0, -20.0, 0.0);
	//float instanceDisplacement = (1. + pow(1.05, 2.0*float(gl_InstanceID + 1))) / 3.0*float(gl_InstanceID);
	//gl_Position += vec4(0.9*instanceDisplacement, 1.0 * instanceDisplacement, 0.5, 2.5 * (pow( (0.8 + 0.75*abs(cos(time))) , float(gl_InstanceID)))) + startingOffset;
	
}