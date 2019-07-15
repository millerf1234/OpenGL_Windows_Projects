//Fragment shader for the asset loading demo
//
// Programmer:  Forrest Miller
// Date:        November 28, 2018

#version 450 core


in VERTEX_SHADER_OUTPUT {  
    float vertID;
    float vertIDMod;
    float instanceID;
    vec4 position;
    vec2 texCoord;
    vec3 normal;
} processed_vertex;

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
	
    const vec4 position = processed_vertex.position;
    const vec3 normal = processed_vertex.normal;
    const vec2 texCoord = processed_vertex.texCoord;

	vec3 lightDir = normalize(vec3(-7.0 * cos(time + 4.0*cnoise(position)), 1.5 * cos(time + 5.0*fbm(cross(vec3(gl_FragCoord.xy, cos(time)), position.xyz))), 1.0));
	float intensity;
	intensity = dot(normalize(lightDir), normal);


	if (intensity > 1.04) {
		discard;//color = vec4(0.99, 0.93, 0.82, 1.0);
	}
	else if (intensity > 0.7) {
		color = vec4(0.8, 0.35 + 0.35*cos(time), 0.65, 1.0);
	}
    else if (intensity > 0.61) {
        color = vec4(0.0, 0.8, 0.3, 1.0);
    }
    else if (intensity > 0.6) {
        discard;
    }
	else if (intensity > 0.49) {
		color = vec4(0.73, 0.7, 0.63, 1.0);
		//discard;
	}
	else if (intensity > (0.32)) {
		color = vec4(0.32, 0.301, 0.2501, 1.0);
	}
	else if (intensity > 0.135) {
		color = vec4(0.32, 0.301, 0.2501, 1.0);
	}
	else if (intensity > 0.0) {
		color = vec4(0.612, 0.127, 0.41, 0.25);// +vec4(0.95*sin(cnoise(position))*position);
	}
    else if (intensity > -0.1 + 0.2 * cos(time + 3.0*processed_vertex.vertIDMod)) {
        discard;
    }
    else {
        color = vec4(vec3(abs(0.1 / intensity)), clamp((-1.0 / intensity), 0.0, 1.0));
    }


	color.a = smoothstep(-10.22, 10.5, 0.35 - 0.33*abs(sin(1.0*time + 4.0 * snoise(vec2(abs(intensity), time)))));
}
