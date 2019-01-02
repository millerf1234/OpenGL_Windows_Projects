//Fragment shader 
//Assigns a color to each pixel that is covered after rasterization

#version 450 core

//Input from vertex shader
in vec4 position;     //<x, y, z, w>
in vec2 texCoord;     //<u, v>
in vec3 normal;       //<x, y, z>

//Output to screen
out vec4 color;    //<Red, Green, Blue, Alpha>      Alpha only matters when blending    


//"uniform" => parameter to be set by the application which remains constant over the course of a draw call
uniform float time;  


///Ignore these for now, they aren't essential
/*
///////////////////////////////////////////////////////////////////////////
// EXTERNAL FUNCTION PROTOTYPES  [for noise]        (these are implemented in a seperate file)
///////////////////////////////////////////////////////////////////////////
float noise(vec2 p);           //   Generic 2d noise       
float pNoise(vec2 p, int res); //2d Perlin
float snoise(vec2 v);          //2d simplex noise
float cnoise(vec4 P);          //4d Periodic Classic Perlin Noise
float fbm(float x);            //1d Fractal Brownian Motion
float fbm(vec2 x);             //2d Fractal Brownian Motion
float fbm(vec3 x);             //3d Fractal Brownian Motion
*/

void main() {

	vec3 lightDir = vec3(2.0, 2.0, 2.0);
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
		color = vec4(0.0612, 0.17, 0.21, 1.0);
	}

}	
