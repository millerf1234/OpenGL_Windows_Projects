//Short header here because i am tired

#version 450 core

in vec3 lightPosition;
in  vec3 lightColor;



//in vec3 vertexPosition;
//in vec3 vertexColor;

out vec4 color;

uniform float time;
uniform float zoom;
uniform float distanceToCamera;

uniform int noiseFunctionToUse; //'0' for generic 2d, '1' for perlin , '2' for simplex, '3' for 1d FBM, any other value for 2d FBM 
uniform int noiseResolution; //Will only have an effect when using perlin noise




///////////////////////////////////////////////////////////////////////////
// EXTERNAL FUNCTION PROTOTYPES  [for noise]
///////////////////////////////////////////////////////////////////////////
float noise(vec2 p);           //   Generic 2d noise       
float pNoise(vec2 p, int res); //2d Perlin
float snoise(vec2 v);          //2d simplex noise
float fbm(float x);            //1d Fractal Brownian Motion
float fbm(vec2 x);             //2d Fractal Brownian Motion
float fbm(vec3 x);             //3d Fractal Brownian Motion

void main() {

	switch (noiseFunctionToUse) {
	default:
		color = vec4(fbm(vec2(lightPosition.x, lightPosition.y)), fbm(vec2(lightColor.x, lightColor.y)), lightColor.z, 1.0);
		break;
	case(0):
		//color = vec4(noise(vec2(lightPosition.x, lightPosition.y)), noise(vec2(lightColor.x, lightColor.y)), lightColor.z, 1.0);
		vec3 tempColor = vec3(noise(vec2(lightColor.z, lightPosition.y)), noise(vec2(lightColor.x, lightColor.y)), lightColor.z);
		//if (dot(tempColor, vec3(0.0, 0.0, 0.0)) >= abs(0.25 * sin(time))) {
		//	discard;
		//}
		color = vec4(tempColor, -abs(sin(time)) + (length(tempColor) / fbm(tempColor)));
		break;
	case(1):
		color = vec4(fbm(vec2(250.5101*0.0555*time*lightColor.gb)), fbm(lightPosition + gl_FragCoord.xyx), snoise(vec2(fbm(lightPosition.xy), fbm(lightPosition.yx))), 1.0);
		/*color = vec4(fbm(vec2(250.5101*sin(0.05*time)*lightColor.gb)), fbm(lightPosition), snoise(vec2(fbm(lightPosition.xy), fbm(lightPosition.yx))), 1.0);*/
		break;
	case(2): 
		color = vec4(vec3(mix(fbm(lightPosition), fbm(lightColor), fbm(gl_FragCoord.xyz))), 1.0);
		break;
	case(3):
		color = vec4(lightColor.r * fbm(lightColor), lightColor.g * fbm(lightColor.gbr), lightColor.b - 0.5*fbm(lightColor), 0.15 + 0.1*sin(2.0*time));
		break;
	case(4):
		color = vec4(lightColor, 1.0);
	}
	




	//color = vec4(lightColor, 1.0);
	

	/*
	float distanceDrop = min(1.0, (25.0 / log(distanceToCamera)));

	vec3 lightColor =  vertexColor;
	color = vec4(lightColor, 1.0);
	*/

}


