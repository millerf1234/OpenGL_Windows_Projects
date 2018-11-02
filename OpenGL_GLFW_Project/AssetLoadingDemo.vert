//This shader should have the secondary "VertMath.vert" vertex math shader linked with it

#version 450 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 tex;
layout(location = 2) in vec3 normal;

out vec3 vertPos;
out vec2 texCoord;
out vec3 vertNormal;


uniform float zoom;
uniform float time;
uniform float xRotation;
uniform float yRotation;
uniform float zRotation;

//The following uniforms define the 'frustrum' 
uniform float fNear;
uniform float fFar;
uniform float fLeft;
uniform float fRight;
uniform float fBottom;
uniform float fTop;


//Declarations of functions that are defined in a different file but are used within this file
void setAspectRatio(inout vec3 vert, in float aspectRatio);
void rotateX(inout vec3 vert, in float theta);
void rotateY(inout vec3 vert, in float theta);
void rotateZ(inout vec3 vert, in float theta);

//From experimental Frustrum matrix file called "PerspectiveProjection":
void frustrumMult(inout vec4 vert, in float fNear, in float fFar, in float fLeft, in float fRight, in float fBottom, in float fTop);

void main() {

	vec4 projPos = position;

	frustrumMult(projPos, fNear, fFar, fLeft, fRight, fBottom, fTop);

	rotateX(projPos.xyz, xRotation);
	rotateY(projPos.xyz, yRotation);
	rotateZ(projPos.xyz, zRotation);

	vertPos = projPos.xyz;
	texCoord = tex;
	vertNormal = normal;

	gl_Position = projPos + vec4(0.0, 0.0, 0.0, 2.0*zoom);
	/*
	// Process the Position input for the vertex
	vec3 rotatedPosition = position.xyz;

	rotateX(rotatedPosition, xRotation);
	rotateY(rotatedPosition, yRotation);
	rotateZ(rotatedPosition, zRotation);
	setAspectRatio(rotatedPosition, -3600.0 / 1800.0);
	

	vertPos = rotatedPosition;
	texCoord = tex;
	vertNormal = normal;
	
	
	//gl_Position = vec4(position.xyz, 1.0);
	gl_Position = vec4(rotatedPosition, position.w + zoom);//clamp(position.w + zoom, 1.0, 10.0));
	*/
}