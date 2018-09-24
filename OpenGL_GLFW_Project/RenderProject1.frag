//#version 450 core
//
//layout( location = 0) in vec3 position;
//layout( location = 1) in vec3 color;
//
//uniform float zoom;
//uniform float time;
//uniform float xRotation;
//uniform float yRotation;
//uniform float zRotation;
//
//out vec3 pos;
//out vec3 Color;
//
//void main() {
//
//	Color = color;
//
//	pos = 
//
//
//}

#version 450 core

in vec3 pos;
in vec3 Color;

out vec4 FragColor;

void main() {
	FragColor = vec4(Color, 1.0);

}