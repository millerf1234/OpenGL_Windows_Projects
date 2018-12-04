//Fragment shader for the asset loading demo
//
// Programmer:  Forrest Miller
// Date:        November 28, 2018

#version 450 core

//Test:
in flat vec4 position;
in vec2 texCoord;
in flat vec3 normal;

out vec4 color;

uniform float zoom;
uniform float time;

void main() {
	vec3 lightDir = vec3(1.0 * cos(time), 1.0 * sin(time), 0.25);
	float intensity;
	intensity = dot(lightDir, normal);

	if (intensity > 0.95)
		color = vec4(1.0, 0.5, 0.5, 1.0);
	else if (intensity > 0.5)
		color = vec4(0.6, 0.3, 0.3, 1.0);
	else if (intensity > 0.25)
		color = vec4(0.4, 0.2, 0.2, 1.0);
	else
		color = vec4(0.2, 0.1, 0.1, 1.0);
}






//This version has almost a cartoon-shaded look to it
//
//in vec4 position;
//in vec2 texCoord;
//in vec3 normal;
//
//out vec4 color;
//
//uniform float zoom;
//uniform float time;
//
//float noise(in vec2 p);
//float voronoi(in vec2);
//
//
////vec3 combinedVoronoi(in vec3 c) {
////	return vec3(noise(c.xy), noise(c.yz), noise(c.xz));
////}
//
//
//void main() {
//	//vec3 lightPos = normalize(vec3(-3.0 + 5.0 * sin(time), 0.0, 1.0));
//	vec3 redLightPos = vec3(-4.0 + 15.0 * sin(0.5*time), abs(0.0005*noise(vec2(time + time*sin(time), time*sin(time)))), 3.0);
//	vec3 redLightCol = vec3(0.65, 0.02, 0.6045 - abs(0.39*sin(time / 4.712)));
//
//	vec3 blueLightPos = -2.0*redLightPos;
//	vec3 blueLightCol = vec3(0.1, 0.1, 1.0);
//
//	vec3 amb = vec3(0.015);
//
//	vec3 redDiffuse = redLightCol * clamp((dot(normalize((position.xyz - redLightPos)), normal)), 0.0, 1.0);
//	vec3 blueDiffuse = blueLightCol * clamp((dot(normalize((position.xyz - blueLightPos)), normal)), 0.0, 1.0);
//
//	vec3 diffuse = mix(redDiffuse, blueDiffuse, 0.75 + 0.35*sin(0.1*time + 30.*noise(vec2(13.6*gl_FragCoord.y-redDiffuse.x, blueDiffuse.b))));
//
//
//
//	vec3 computedColor1 = vec3(amb + diffuse);
//
//	vec4 finalColor1 = vec4(smoothstep(-4.90*redDiffuse, 2.0*blueDiffuse, (0.75 + 0.15*sin(0.0075*time))*computedColor1), 1.0);
//
//
//	//Basically I was getting an output color that had a lot of whites, so I made that finalColor1 and then added the following lines
//	//to make it so any color beyond white (1.0, 1.0, 1.0) will grow dark again.
//	//Step returns 0 if finalColor1 is less than vec4(1.0, 1.0, 1.0, 1.0) and retruns 1 otherwise
//	vec4 finalColor2 = 1.0*vec4(0.59 + 0.45*sin(time * 0.77 * 3.14 + 0.25), 0.5 + 0.31 * cos(sin(time)), 0.65 + 0.15 * cos(3.14159*2.19+gl_FragCoord.x*3.*gl_FragCoord.y) , 2.0) - (finalColor1 * abs(noise(finalColor1.zx)));
//
//	color = mix(finalColor2, finalColor1, step(vec4(1.28 + 0.56*sin(3.145*time/float(15.0*gl_FragCoord.x))), finalColor1));
//
//}




////Fragment shader for the asset loading demo
////
//// Programmer:  Forrest Miller
//// Date:        November 28, 2018
//
//#version 450 core
//
//in vec4 position;
//in vec2 texCoord;
//in vec3 normal;
//
//out vec4 color;
//
//uniform float zoom;
//uniform float time;
//
//float noise(in vec2 p);
//float voronoi(in vec2);
//
//
//vec3 combinedVoronoi(in vec3 c) {
//	return vec3(noise(c.xy), noise(c.yz), noise(c.xz));
//}
//
//
//void main() {
//	//vec3 lightPos = normalize(vec3(-3.0 + 5.0 * sin(time), 0.0, 1.0));
//	vec3 redLightPos = vec3(-4.0 + 15.0 * sin(0.5*time), 2.0, 3.0);
//	vec3 redLightCol = vec3(0.95, 0.15 + length(texCoord), 0.045);
//
//	vec3 blueLightPos = -redLightPos;
//	vec3 blueLightCol = vec3(0.1, 0.1, 1.0);
//
//	vec3 amb = vec3(0.015);
//	
//	vec3 redDiffuse = redLightCol * clamp((dot(normalize((position.xyz - redLightPos)), normal)), 0.0, 1.0);
//	vec3 blueDiffuse = blueLightCol * clamp((dot(normalize((position.xyz - blueLightPos)), normal)), 0.0, 1.0);
//
//	vec3 diffuse = mix(redDiffuse, blueDiffuse, 0.75 + 0.35*sin(time));
//
//	vec3 computedColor = vec3(amb + diffuse);
//
//	color = vec4(smoothstep(2.0*redDiffuse, 2.0*blueDiffuse, computedColor), 1.0);
//
//	//color = vec4(vec3(1.0)-combinedVoronoi(computedColor), 1.0); 
//	//color = vec4(noise(computedColor.xy), noise(computedColor.yz + gl_FragCoord.xy), noise(computedColor.zy), 1.0);
//	//color = vec4((computedColor), 1.0);
//}


/*
void main() {
	//Very simple for now...
	color = smoothstep(vec4(abs(normal), length(position)) - vec4(dot(normal, position.xyz)), vec4(texCoord, texCoord), position);
}
*/