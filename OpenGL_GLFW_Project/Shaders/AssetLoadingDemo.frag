
//Fragment shader for the asset loading demo
//
// Programmer:  Forrest Miller
// Date:        November 28, 2018

#version 450 core

//Test:
in vec4 position;
in vec2 texCoord;
in vec3 normal;

out vec4 color;

uniform float zoom;
uniform float time;

//uniform vec2 mouseCoord; //To be added...



///////////////////////////////////////////////////////////////////////////
// EXTERNAL FUNCTION PROTOTYPES  [for noise]
///////////////////////////////////////////////////////////////////////////
float noise(vec2 p);           //   Generic 2d noise       
float pNoise(vec2 p, int res); //2d Perlin
float snoise(vec2 v);          //2d simplex noise
float cnoise(vec4 P);          //4d Periodic Classic Perlin Noise
float fbm(float x);            //1d Fractal Brownian Motion
float fbm(vec2 x);             //2d Fractal Brownian Motion
float fbm(vec3 x);             //3d Fractal Brownian Motion


void main() {
	vec3 lightDir = vec3(1.5 * cos(0.2*time), 0.1, 2.0 * sin(time*1.80));
	//vec3 lightDir = vec3(0.15, 0.0045*sin(1.1*time), 1.175);
	//float radius = 5.0;
	//vec3 lightDir = vec3(radius*cos(time), radius*sin(1.1*time), 1.175);
	float intensity; 
	intensity = dot(normalize(lightDir), normal);

	//intensity += smoothstep(abs(dFdx(noise(normal.xy + normal.xz))), abs(dFdy(position.x)), intensity);

	if (intensity > 0.905) {
		color = vec4(0.99, 0.93, 0.78, 1.0);
	}
	//else if (intensity > 0.85) {
	//	color = vec4(0.89, 0.85, 0.77, 0.9);
	//}
	//else if (intensity > 0.8) {
	//	color = vec4(0.82, 0.78, 0.7, 0.8);
	//}
	//else if (intensity > 0.75) {
	//	color = vec4(0.72, 0.6, 0.5, 0.7);
	//}
	else if (intensity > 0.6) {
		color = vec4(0.8, 0.7, 0.65, 1.0);// +smoothstep(vec4(0.11, 0.13, 0.21, 1.0), vec4(0.9, 0.9, 0.9, 1.0), vec4(intensity * 0.20 - 0.005*sin(3.*time))).gbra; //vec4(0.6, 0.3, 0.3, 1.0);
	}
	//else if (intensity > (0.2)) {
	//	color = vec4(0.42, 0.42, 0.34, 1.0); //smoothstep(vec4(0.1, 0.1, 0.1, 1.0), vec4(0.8, 0.5, 0.8, 1.0), vec4(intensity * 2.0 ));
	//}
	else if (intensity > (0.3)) {
		color = vec4(0.32, 0.301, 0.2501, 1.0);
	}
	else if (intensity > 0.0) {
		color = vec4(0.32, 0.301, 0.2501, 1.0);
	}
	else {
		color = vec4(0.12, 0.15, 0.21, 1.0);// +vec4(0.9*sin(cnoise(position))*position);
	} 
	//if (intensity < 0.6)
		//color.r += smoothstep(-color.b, 3.0*color.r, gl_FragCoord.x/(9.00 / (intensity + (0.2 * sin(5.0*time))))*0.075*snoise(0.05*(dot(gl_FragCoord.yy,gl_FragCoord.xx)*gl_FragCoord.xy)*fbm(intensity * gl_FragCoord.xy)));
}
