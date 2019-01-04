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



uniform float time;



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
	
	vec3 lightDir = vec3(10.0 * cos(time), 0.0 * sin(time), -10.0);
	float intensity;
	intensity = dot(normalize(lightDir), normal);


	if (intensity > 1.04) {
		color = vec4(0.99, 0.93, 0.82, 1.0);
	}
	else if (intensity > 0.6) {
		color = vec4(0.8, 0.75, 0.65, 1.0);
	}
	else if (intensity > (0.3)) {
		color = vec4(0.32, 0.301, 0.2501, 1.0);
	}
	else if (intensity > 0.05) {
		color = vec4(0.32, 0.301, 0.2501, 1.0);
	}
	else {
		color = vec4(0.0612, 0.17, 0.21, 1.0);// +vec4(0.95*sin(cnoise(position))*position);
	}

	//float extraNoise = smoothstep(-color.b, 3.0*color.r, gl_FragCoord.x / (9.00 / (intensity + (0.2 * sin(5.0*time))))*0.075*snoise(0.05*(dot(gl_FragCoord.yy, gl_FragCoord.xx)*gl_FragCoord.xy)*fbm(intensity * gl_FragCoord.xy)));
	//if (intensity < (0.6 - abs(0.5*cos(0.1*time)) ))
	//	color.r += extraNoise + sin(extraNoise / time);
	//else
	//	color.g = clamp( (color.r+color.b) / extraNoise, 0.3, 0.7);
	color.a = smoothstep(-10.22, 10.5, 0.35 - 0.33*abs(sin(1.0*time + 4.0 * snoise(vec2(intensity, time)))));
}
