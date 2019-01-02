//Vertex Shader 
//Inputs the Model's vertices and modifes them to be drawn in the scene 
//Requirement: A vertex shader must write x,y,z,w values to 'gl_Position'


//Required Tag stating this shader uses 'OpenGL 4.5 Core' 
#version 450 core

//Describe what an input vertex consists of
layout(location = 0) in vec4 ModelPosition;
layout(location = 1) in vec2 ModelTexCoord;
layout(location = 2) in vec3 ModelNormal;

//Specify optional output for use in Fragment shader
out vec4 position;
out vec2 texCoord;
out vec3 normal;

//"uniform" => parameter to be set by the application which remains constant over the course of a draw call
uniform float zoom;
uniform float time;
uniform mat4 rotation;  //Sets a model's orientation in the scene. This matrix is part of the Model matrix.
uniform mat4 MVP;       //MVP stands for Model-View-Projection.
//                      // 'Model' takes each vertex from its stored Model data and puts it into a position in world
//                      // 'View' moves scene to be what is visible through the camera
//                      // 'Projection' projects the view frustrum to the square screen coordinates


//The following 2 macros are for convience to make typing easier
#define vert float(gl_VertexID)
#define inst (float(gl_InstanceID)*2.0)

///Ignore these for now, they aren't essential
/*
///////////////////////////////////////////////////////////////////////////
// EXTERNAL FUNCTION PROTOTYPES  [for noise]        (these are implemented in a seperate file)
///////////////////////////////////////////////////////////////////////////
float noise(vec2 p);           //   Generic 2d noise
float pNoise(vec2 p, int res); //2d Perlin
float snoise(vec2 v);          //2d simplex noise
float cnoise(vec4 P);          //4d Periodic Classic Perlin Noise
float fbm(float x);            //1d Fractal Brownian Motion
float fbm(vec2 x);             //2d Fractal Brownian Motion
float fbm(vec3 x);             //3d Fractal Brownian Motion
*/



//Actual Shader Function to be run once per input vertex 
void main() {


	gl_Position = ModelPosition;
}
