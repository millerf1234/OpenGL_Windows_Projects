//Fragment shader for the asset loading demo
//
// Programmer:  Forrest Miller
// Date:        November 28, 2018

#version 450 core

in vec4 position;
in vec2 texCoord;
in vec3 normal;

out vec4 color;

uniform float zoom;
uniform float time;


void main() {
	vec3 lightPos = vec3(-3.0 + 5.0 * sin(time), 0.0, 3.0);
	vec3 lightCol = vec3(0.75, 0.25, 0.85);

	vec3 amb = vec3(0.15);
	
	vec3 diffuse = lightCol * (abs(dot(lightPos, normal)));

	color = vec4(vec3(amb + 0.45*diffuse), 1.0);
}

//void main() {
	//Very simple for now...
	//color = smoothstep(vec4(abs(normal), length(position)) - vec4(dot(normal, position.xyz)), vec4(texCoord, texCoord), position);
//}