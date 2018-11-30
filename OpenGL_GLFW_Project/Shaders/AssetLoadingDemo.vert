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
//uniform mat4 transform;


void main() {

	//Keep things very simple for now...
	position = ModelPosition + vec4(0.0, 0.0, 0.0, zoom);

	texCoord = ModelTexCoord;

	normal = ModelNormal;


	gl_Position = rotation * position;
}