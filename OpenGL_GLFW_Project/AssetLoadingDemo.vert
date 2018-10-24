//This shader should have the secondary "VertMath.vert" vertex math shader linked with it

#version 450 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 tex;
layout(location = 2) in vec3 normal;

out vec3 vertPos;
out vec3 vertNormal;

/*
uniform float zoom;
uniform float time;
uniform float xRotation;
uniform float yRotation;
uniform float zRotation;


//Declarations of functions that are defined in a different file but are used within this file
void setAspectRatio(inout vec3 vert, in float aspectRatio);
void rotateX(inout vec3 vert, in float theta);
void rotateY(inout vec3 vert, in float theta);
void rotateZ(inout vec3 vert, in float theta);
*/
void main() {


	vertPos = position.xyz;
	vertNormal = normal;

	gl_Position = position;

	//Process the Position input for the vertex
	//vec3 vertPos = position;
	//rotateX(vertPos, xRotation);
	//rotateY(vertPos, yRotation - sin(time));// +(0.75*cosh(tan(time*45.0))));
	//rotateZ(vertPos, zRotation);

	//setAspectRatio(vertPos, -3600.0 / 1800.0);
	//v2g_position = vertPos;

	//gl_Position = vec4(vertPos, zoom);
}