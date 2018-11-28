//Draws a lightsource as is from it's screen coords

#version 450 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;

out vec3 lightPosition;
out vec3 lightColor;


//uniform float distanceToCamera;
//uniform mat4 projection;
uniform float time;
uniform float zoom;

void main() {
	//Pass the light's color on to the geometry shader
	lightColor = color;
	

	//Translate the lights position from world space to camera space
	//vec4 projectedLightPosition = projection *  vec4((0.035*position), 0.0); // distanceToCamera);
	
	//lightPosition = projectedLightPosition.xyz;
	//gl_Position = projectedLightPosition;
	
	gl_Position = vec4(position, zoom);
}