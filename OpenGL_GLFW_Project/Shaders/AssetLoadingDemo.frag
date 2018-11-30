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
	//vec3 lightPos = normalize(vec3(-3.0 + 5.0 * sin(time), 0.0, 1.0));
	vec3 redLightPos = vec3(-4.0 + 15.0 * sin(0.5*time), 2.0, 3.0);
	vec3 redLightCol = vec3(0.95, 0.15 + length(texCoord), 0.045);

	vec3 blueLightPos = -redLightPos;
	vec3 blueLightCol = vec3(-0.1, 0.1, 1.0);

	vec3 amb = vec3(0.015);
	
	vec3 redDiffuse = redLightCol * clamp((dot((position.xyz - redLightPos), normal)), 0.0, 1.0);
	vec3 blueDiffuse = blueLightCol * clamp((dot((position.xyz - blueLightPos), normal)), 0.0, 1.0);

	vec3 diffuse = mix(redDiffuse, blueDiffuse, 0.5);

	color = vec4(vec3(amb + diffuse), 1.0);
}

//void main() {
	//Very simple for now...
	//color = smoothstep(vec4(abs(normal), length(position)) - vec4(dot(normal, position.xyz)), vec4(texCoord, texCoord), position);
//}