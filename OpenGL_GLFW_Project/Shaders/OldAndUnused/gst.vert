#version 450 core

layout (location = 0) in vec3 vertPos;
layout (location = 1) in vec3 vertCol;


uniform float zoom;
uniform float time;
uniform float xRotation;
uniform float yRotation;
uniform float zRotation;

out vec3 pos;
out vec3 Color;

void main() {
	//Declarations of functions that are defined in a different file but are used within this file
	void setAspectRatio(inout vec3 vert, in float aspectRatio);
	void rotateX(inout vec3 vert, in float theta);
	void rotateY(inout vec3 vert, in float theta);
	void rotateZ(inout vec3 vert, in float theta);

	void main() {
		//Process the color input for the vertex
		vec3 modColor = color;
		rotateY(modColor, time);
		Color = modColor;

		//Process the Position input for the vertex
		vec3 tempPos = position;
		rotateX(tempPos, xRotation);
		rotateY(tempPos, yRotation);
		rotateZ(tempPos, zRotation);

		setAspectRatio(tempPos, 2940.0 / 2160.0);
		pos = tempPos;

		//rotateX(rotateY(rotateZ(pos, xRotation), yRotation), zRotation);



		gl_Position = vec4(pos, zoom);
	}
}