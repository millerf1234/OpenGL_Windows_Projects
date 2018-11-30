//Fragment shader for the asset loading demo
//
// Programmer:  Forrest Miller
// Date:        November 28, 2018

#version 450 core

in vec4 position;
in flat vec2 texCoord;
in flat vec3 normal;

out vec4 color;

uniform float zoom;
uniform float time;


void main() {
	//Very simple for now...
	color = smoothstep(vec4(abs(normal), length(position)) - vec4(dot(normal, position.xyz)), vec4(texCoord, texCoord), position);
}