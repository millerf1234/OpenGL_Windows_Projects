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
float cnoise(vec4 P);          //4d Periodic Classic Perlin Noise
float fbm(float x);            //1d Fractal Brownian Motion
float fbm(vec2 x);             //2d Fractal Brownian Motion
float fbm(vec3 x);             //3d Fractal Brownian Motion




void main() {

	float timeShift = 9.5; //Used to start some of the noise functions at a better-looking resolution  (time == 0 is wayyy too zoomed out)

	switch (noiseFunctionToUse) {
	case(0):
		//color = vec4(noise(vec2(lightPosition.x, lightPosition.y)), noise(vec2(lightColor.x, lightColor.y)), lightColor.z, 1.0);
		vec3 tempColor = vec3(noise(vec2(lightColor.z, lightPosition.y)), noise(vec2(lightColor.x, lightColor.y)), lightColor.z);
		//if (dot(tempColor, vec3(0.0, 0.0, 0.0)) >= abs(0.25 * sin(time))) {
		//	discard;
		//}
		color = vec4(tempColor, -abs(sin(time)) + (length(tempColor) / (3.0*fbm(tempColor))));
		if (int(sign(colorShift)) < 0) {
			color = vec4(cnoise(floor(500.0*sin(0.1*time + sqrt(pow(gl_FragCoord.x, 2.0) + pow(gl_FragCoord.y, 2.0))))*color), 355.0*cnoise(floor(5.0*cos(0.0001*time))*color), 0.75 + 0.25 * sin(cnoise(color) + 0.001*time), color.a);
		}
		break;
	case(1):
		color = vec4(fbm(vec2(250.5101*0.0555*time*lightColor.gb)), fbm(lightPosition + gl_FragCoord.xyx), snoise(vec2(fbm(lightPosition.xy), fbm(lightPosition.yx))), 1.0);
		/*color = vec4(fbm(vec2(250.5101*sin(0.05*time)*lightColor.gb)), fbm(lightPosition), snoise(vec2(fbm(lightPosition.xy), fbm(lightPosition.yx))), 1.0);*/
		if (int(sign(colorShift)) < 0) {
			color.rgb = color.rrr + color.bbg + color.bbr;
		}
		break;
	case(2): 
		color = vec4(vec3(mix(fbm(lightPosition), fbm(lightColor), fbm(gl_FragCoord.xyz))), 1.0);
		if (int(sign(colorShift)) < 0) {
			color.rgb = vec3(1.0) - color.rgb;
		}
		break;
	case(3):
		color = vec4(lightColor.r * fbm(lightColor), lightColor.g * fbm(lightColor.gbr), lightColor.b - 0.5*fbm(lightColor), 0.15 + 0.1*sin(2.0*time));
		if (int(sign(colorShift)) < 0) {
			color.rgb = color.brg;
		}
		break;
	case(4):
		color = vec4(vec3(0.8, 1.3 + (0.25*sin(gl_FragCoord.x)), 1.15) - (time * fbm(lightColor) * lightColor), 1.0 - 0.75 * sin(time));
		if (int(sign(colorShift)) < 0) {
			color.rgb = color.grb;
		}
		break;
	case(5):
		//color = vec4(length(inversesqrt(fbm(lightColor * 1.0 / time))), pNoise(vec2(abs(5.5*gl_FragCoord.y-cos(lightPosition.x-time)), 5.0*gl_FragCoord.x-abs(sin(time))), abs(int(floor(500*cos(0.25*time))))), lightColor.b, 1.0 - abs(0.5*sin(time)*fbm(lightPosition)));
		color = vec4(length(inversesqrt(fbm(lightColor * 1.5 / (time + timeShift)))),
			snoise(
				vec2(
					snoise(
						vec2(
							abs(((1.0 / (time + timeShift))*gl_FragCoord.y - cosh(lightPosition.x - 0.25*cos((time + timeShift))))),
							(1.0 / (time + timeShift)) * gl_FragCoord.x
						)
					),
					inversesqrt(pow((abs(gl_FragCoord.x) + abs(gl_FragCoord.y)), abs((gl_FragCoord.y + gl_FragCoord.x))))
				)
			),
			snoise(
				vec2(
					snoise(
						vec2(
							abs(((1.0 / (time + timeShift))*gl_FragCoord.x + cosh(lightPosition.y - 0.25*cos((time + timeShift) + gl_FragCoord.y)))),
							(1.0 / (time + timeShift)) * gl_FragCoord.x * abs(sinh(gl_FragCoord.y))
						)),
					inversesqrt(gl_FragCoord.y))),
			1.0);
		color.r *= 0.15;
		color.b = smoothstep(color.g, color.r, 20.0*float(sign(colorShift))) / color.g;
		break;


	case(6):
		color = vec4(length(inversesqrt(3.0*fbm(lightColor * 1.0 / (time + timeShift)))),
			snoise(
				vec2(
					snoise(
						vec2(
							abs(((1.0 / (time + timeShift))*gl_FragCoord.y - cosh(lightPosition.x - 0.25*cos(time + timeShift)))),
							(1.0 / (time + timeShift)) * gl_FragCoord.x
						)
					),
					inversesqrt(gl_FragCoord.x/gl_FragCoord.y)
				)
			),
			0.0,
			1.0 );
		color.r *= 0.15 + (0.035 + (0.035 * -1.0 * float(sign(colorShift))));
		color.g = color.g - (0.25 * sin(floor(150.0*(sin((gl_FragCoord.x - sin(exp(log2(time))))) * gl_FragCoord.y)))  + (0.25 * -1.0 * float(sign(colorShift))));
		color.b = color.r / color.g;
		break;
	case(7):
		color = vec4(length(inversesqrt(3.0*fbm(lightColor * 1.0 / (time + timeShift)))),
			snoise(
				vec2(
					snoise(
						vec2(
							abs(((1.0 / (time + timeShift))*gl_FragCoord.y - cosh(lightPosition.x - 0.25*cos(time + timeShift)))),
							(1.0 / (time + timeShift)) * gl_FragCoord.x
						)
					),
					inversesqrt(gl_FragCoord.x / gl_FragCoord.y)
				)
			),
			0.0,
			1.0);
		color.r += 0.15 + (0.035 + (0.035 * -1.0 * float(sign(colorShift))));
		color.r *= 0.15;
		color.b = color.r / color.g;
		break;

	default:
		color = vec4(fbm(vec2(lightPosition.x, lightPosition.y)), fbm(vec2(lightColor.x, lightColor.y)), lightColor.z, 1.0);
		break;
	}
	

}


