//See the vertex shader for a more detailed description. 
//The frag shader is less important than the vertex shader 
//for this effect

#version 450 core

in vec4 position;
in vec2 texCoord;
in vec3 normal;

out vec4 color;

uniform float time;

//#define INCLUDE_NOISE   //Define this if implementations are available for the 2 required noise functions

#ifdef INCLUDE_NOISE
float snoise(vec2 v);          //2d simplex noise
float fbm(float x);            //1d Fractal Brownian Motion
#endif 


void main() {
	vec3 lightDir = vec3(1.5 * cos(0.2*time), 0.1, 2.0 * sin(time*1.80));

	float intensity;
	intensity = dot(normalize(lightDir), normal);

	if (intensity > 0.995) {
		color = vec4(0.99, 0.93, 0.78, 1.0);
	}
	else if (intensity > 0.6) {
		color = vec4(0.8, 0.7, 0.65, 1.0);
	}
	else if (intensity > (0.3)) {
		color = vec4(0.32, 0.301, 0.2501, 1.0);
	}
	else if (intensity > 0.0) {
		color = vec4(0.32, 0.301, 0.2501, 1.0);
	}
	else {
		color = vec4(0.12, 0.15, 0.21, 1.0);
	}

	//Optional noise step at the end
#ifdef INCLUDE_NOISE
	color.r *= smoothstep(-color.b, 3.0*color.r, gl_FragCoord.x / (9.00 / (intensity + (0.2 * sin(5.0*time))))*0.075*snoise(0.05*(dot(gl_FragCoord.yy, gl_FragCoord.xx)*gl_FragCoord.xy)*fbm(intensity * gl_FragCoord.xy)));
#endif
}