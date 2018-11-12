//Short header here because i am tired

#version 450 core

in vec3 lightPosition;
in  vec3 lightColor;



//in vec3 vertexPosition;
//in vec3 vertexColor;

out vec4 color;

uniform float time;
uniform float zoom;

uniform int noiseFunctionToUse;
uniform int noiseResolution; //Will only have an effect when using perlin noise

uniform int colorShift; //Will just use sign of colorShift to do the colorShift



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
		break;
	case(5):
		//color = vec4(length(inversesqrt(fbm(lightColor * 1.0 / time))), pNoise(vec2(abs(5.5*gl_FragCoord.y-cos(lightPosition.x-time)), 5.0*gl_FragCoord.x-abs(sin(time))), abs(int(floor(500*cos(0.25*time))))), lightColor.b, 1.0 - abs(0.5*sin(time)*fbm(lightPosition)));
		color = vec4(length(inversesqrt(fbm(lightColor * 1.0 / time))),
			snoise(
				vec2(
					snoise(
						vec2(
							abs(((1.0 / time)*gl_FragCoord.y - cosh(lightPosition.x - 0.25*cos(time)))),
							(1.0 / time) * gl_FragCoord.x - abs(sinh(cos(0.0*time)))
						)),
					inversesqrt(pow((abs(gl_FragCoord.x) + abs(gl_FragCoord.y)), abs((gl_FragCoord.y + gl_FragCoord.x)))))),
			snoise(
				vec2(
					snoise(
						vec2(
							abs(((1.0 / time)*gl_FragCoord.x + cosh(lightPosition.y - 0.25*cos(time+gl_FragCoord.y)))),
							(1.0 / time) * gl_FragCoord.x * abs(sinh(gl_FragCoord.y))
						)),
					inversesqrt(gl_FragCoord.y))),
			1.0);
		color.r *= 0.15;
		color.b = smoothstep(color.g, color.r, 20.0*float(sign(colorShift))) / color.g;
		break;


	case(6):
		color = vec4(length(inversesqrt(fbm(lightColor * 1.0 / time))),
			snoise(
				vec2(
					snoise(
						vec2(
							abs(((1.0 / time)*gl_FragCoord.y - cosh(lightPosition.x - 0.25*cos(time)))),
							(1.0 / time) * gl_FragCoord.x - abs(sinh(cos(0.0*time)))
						)),
					inversesqrt(gl_FragCoord.x/gl_FragCoord.y))), 0.0, 1.0);
		color.r *= 0.15;
		color.b = color.r / color.g;
		break;
	default:
		color = vec4(fbm(vec2(lightPosition.x, lightPosition.y)), fbm(vec2(lightColor.x, lightColor.y)), lightColor.z, 1.0);
		break;
	}
	

}


