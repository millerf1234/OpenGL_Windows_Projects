//Short header here because i am tired

#version 450 core

in vec3 vertexPosition;
in vec3 vertexColor;

out vec4 color;

uniform float time;
uniform float zoom;
uniform float distanceToCamera;


void main() {
	float distanceDrop = min(1.0, (25.0 / log(distanceToCamera)));

	vec3 lightColor =  vertexColor;
	color = vec4(lightColor, 1.0);

}


