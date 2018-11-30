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

void main() {

	//Keep things very simple for now...
	position = ModelPosition + vec4(0.0, 0.0, 0.0, zoom);
	position.y *= 1.5;

	texCoord = ModelTexCoord;

	//Normally a seperate matrix is needed for transforming the normal vectors. However since here all that is being done is a rotation,
	//which is an orthoganol transform, it is fine to just use it for the normals
	normal = mat3(rotation) * ModelNormal;


	gl_Position = MVP * position;
	//gl_Position = rotation * position;
	
}