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
uniform mat4 rotation;

uniform uint customParameter1, customParameter2, customParameter3;
const uint lightingVersion = customParameter1 % 3U;

///////////////////////////////////////////////////////////////////////////
// EXTERNAL FUNCTION PROTOTYPES  [for noise]
///////////////////////////////////////////////////////////////////////////
float noise(vec2 p);           //   Generic 2d noise
float pNoise(vec2 p, int res); //2d Perlin
float snoise(vec2 v);          //2d simplex noise
float snoise(vec3 v);          //3d simplex noise
float snoise(vec4 v);          //4d simplex noise
float cnoise(vec4 P);          //4d Periodic Classic Perlin Noise
float fbm(float x);            //1d Fractal Brownian Motion
float fbm(vec2 x);             //2d Fractal Brownian Motion
float fbm(vec3 x);             //3d Fractal Brownian Motion


#define WHITE vec4(1.0, 1.0, 1.0, 0.2)
#define RED vec4(1.0, 0.1, 0.3, 0.45)
#define BLACK vec4(0.0)

void main() {
//    if (processed_vertex.vertID < 18 || (length(processed_vertex.position.xyz) > 290.0)) {
//        const float amplitude = -.3 + ((9.*3.14159 + 20.)/(60.))*sin(4.0*(sin(time) * sin(time)));  
//        const vec3 tint = vec3(1.0 - amplitude, 0.6 + 0.35 * cos(amplitude / 4.0*sin(time)), abs(amplitude));
//    
//        const float phongFactor = clamp(abs(dot(processed_vertex.normal, vec3(0.0, 3.0 * cos(0.05*time), 3.0 * sin(0.05*time)))), 0.05, 2.0);
//
//        const vec4 shadedTint = vec4( phongFactor * tint , 0.37 + 0.5 * cos(processed_vertex.vertIDMod)); 
//
//        color = (shadedTint + cnoise(shadedTint)*shadedTint); 
//
//        return;
//    }


    const vec4 position = processed_vertex.position;
    const vec3 normal = processed_vertex.normal;
    const vec2 texCoord = processed_vertex.texCoord;

    float intensity;

    const float noiseAmpl_A = float(customParameter2) * 0.5;
    const float noiseAmpl_B = float(customParameter3) * 0.1;

    const float distFalloff = 1. / max(1., .000344*length(processed_vertex.position.xyz));

    if (0U == lightingVersion) {
        vec3 lightDir = normalize(-vec3(-2.475 * cos(time + noiseAmpl_A*cnoise(distFalloff*position)), 0.0*1.5 * cos(time + noiseAmpl_B*fbm(3.0*cross(2.0*vec3(gl_FragCoord.xy, cos(time)), position.xyz))), 1.0));
        vec3 light2Dir = normalize(vec3(-2.475 * cos(time + noiseAmpl_A*cnoise(distFalloff*position)), 0.0*1.5 * cos(time + noiseAmpl_B*snoise(3.0*cross(vec3(gl_FragCoord.xy, cos(time)), position.xyz))), 1.0));
        intensity = abs(dot(normalize(lightDir), normal)) + abs(dot(normalize(mat3(rotation)*light2Dir), normal));
        intensity *= 0.8; 
    }
    else if (1U == lightingVersion) {
        vec3 lightDir = normalize(vec3(-7.0 * cos(time + noiseAmpl_A*cnoise(position)), 1.5 * cos(time + noiseAmpl_B*fbm(cross(vec3(gl_FragCoord.xy, cos(time)), position.xyz))), 1.0));
        vec3 light2Dir = normalize(vec3(-2.475 * cos(time + noiseAmpl_A*cnoise(position)), 1.5 * cos(time + noiseAmpl_B*snoise(3.0*cross(vec3(gl_FragCoord.xy, cos(time)), position.xyz))), 1.0));
        intensity = abs(dot(normalize(lightDir), normal)) + abs(dot(normalize(mat3(rotation)*light2Dir), normal));
        intensity *= 0.8; 
    }
    else {
        vec3 lightDir = normalize(vec3(-7.0 * cos(time + noiseAmpl_A*cnoise(position)), 1.5 * cos(time + noiseAmpl_B*fbm(cross(vec3(gl_FragCoord.xy, cos(time)), position.xyz))), 1.0));
        intensity = abs(dot(normalize(lightDir), normal));
        intensity = clamp(1.1 * intensity, 0.0, 1.0); 
    }




	if (intensity > 0.94) {
		color = RED; 
	}
    else if (intensity > 0.92) {
       discard; 
    }
    else if (intensity > 0.745) {
        color = vec4(0.25, 0.56, 0.95, 0.86);
    }
    else if (intensity > 0.6575) {
        color = vec4(0.3 + (1.0 + pow(1.05, processed_vertex.instanceID)) * snoise(position), 0.93, 0.39, 0.45);
    }
    else if (intensity > 0.6385) {
       discard; 
    }
	else if (intensity > 0.59) {
		color = vec4(0.4 + 0.39 * cos(0.01*time), 0.19, 1.0, 0.33); 
	}
        else if (intensity > 0.56) {
       discard; 
    }
    else if (intensity > 0.46) {
        color = vec4(0.89, 0.035 + 0.035*cos(time), 0.659, 0.79);
    }
    else if (intensity > 0.43) {
        discard;
    }
    else if (intensity > 0.31) {
        color = (snoise(vec2(4.5*cos(intensity * processed_vertex.instanceID - time), 35.0 * tan(0.01*time)))) * vec4(0.2, 0.47, 0.87, 0.56);//BLACK;//discard;
    }
	else if (intensity > 0.25) {
		color = vec4(0.283, 0.72, 0.949, 1.0);
		//discard;
	}
	else if (intensity > (0.175)) {
		color = vec4(0.32, 0.301, 0.69501, 1.0);
	}
	else if (intensity > 0.115) {
		color = vec4(0.32, 0.391, 0.2501, 1.0);
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


    if (lightingVersion == 1u) {
        color.rgb = color.brg;
    }
    
	color.a = smoothstep(-10.22, 10.5, 0.35 - 0.33*abs(sin(1.0*time + 4.0 * snoise(vec2(abs(intensity), time)))));
}
