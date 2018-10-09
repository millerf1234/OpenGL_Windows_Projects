//Fragment shader for the GeomtryShaderExplosion demo project. 
// Forrest Miller on 9/28/2018

#version 450 core

in vec3 g2f_pos;  //Position data from geomtry shader
in vec3 g2f_vel;  //Velocity data from geometry shader

out vec4 Color;


uniform float time;
uniform float colorShiftThreshhold;

uniform float redRotationTheta;
uniform float greenRotationTheta;
uniform float blueRotationTheta;

//Rotation Functions
//Implementation for the rotation functions
	void rotateColorX(inout vec3 color, in float theta) {
		mat3 xRot = mat3( 1.0,     0.0,         0.0,
					      0.0,  cos(theta), -sin(theta),
		                  0.0,  sin(theta),  cos(theta));
		color = xRot * color;
	}

	void rotateColorY(inout vec3 color, in float theta) {
		mat3 yRot = mat3(cos(theta), 0.0, -sin(theta),
							0.0    , 1.0,     0.0f   ,
						 sin(theta), 0.0, cos(theta));
		color = (yRot * color);
	}

	void rotateColorZ(inout vec3 color, in float theta) {
		mat3 zRot = mat3(cos(theta), -sin(theta),  0.0,
						 sin(theta),  cos(theta),  0.0,
						     0.0   ,      0.0   ,  1.0);
		color = (zRot * color);
	}
//void rotateColorX(inout vec3 color, in float theta);
//void rotateColorY(inout vec3 color, in float theta);
//void rotateColorZ(inout vec3 color, in float theta);

//Only define 1 of the following to determine color scheme
//#define USE_COLOR_SCHEME_0 
//#define USE_COLOR_SCHEME_1  
#define USE_COLOR_SCHEME_2

void main() {

#if defined USE_COLOR_SCHEME_0
	vec3 position = vec3(abs(g2f_pos));
	Color = normalize(vec4(dot(position, g2f_vel), cross(position, g2f_vel)));
#endif //USE_COLOR_SCHEME_0



#if defined USE_COLOR_SCHEME_1
	float minColor = 0.0f;
	float maxColor = 0.0f;

	Color = vec4(abs(g2f_pos.x), abs(g2f_pos.y), abs(g2f_pos.z), 1.0);

	clamp(FragColor, minColor, maxColor);
#endif //USE_COLOR_SCHEME_1


#if defined USE_COLOR_SCHEME_2
	float blue = (g2f_pos.y + 1.0) / 2.0;
	float green = 0.05 * (20.3 * sin(time*5.0) - length(g2f_vel.xyz));
	float red = length(g2f_vel.xyz) / (9.0 * length(g2f_pos.xyz)) + (length(dFdxCoarse(g2f_vel.xyz)) + length(dFdy(g2f_vel.xyz)) / 2.5);

	if (blue > 1.0) {
		blue -= 1.0;
		red = blue / (blue + 3.0);
	}

	float cutoff = colorShiftThreshhold; //+ 0.25*abs(sin(time)); //length((0.15 + noise3(vec3(red, green, blue))));  //0.2 

	if ( (length(vec3(red, green, blue)) < cutoff)) {
		red += abs(gl_FragCoord.x);
		green += abs(gl_FragCoord.x * gl_FragCoord.x - gl_FragCoord.y / 2.0);
		blue += 0.75 * (sin(time * gl_FragCoord.y) + cos(time * gl_FragCoord.x));
	}

	/* red += min(max(0.125, g2f_pos.z), 0.65);

	if (gl_FragCoord.y < -0.75) {
		FragColor = vec4(blue, green, red, 1.0);
	}
	else if (gl_FragCoord.z < -0.35) {
		FragColor = vec4(0.25 * (green / (green - red + blue)), clamp(green + red, 0.1, 0.85), (red + green + blue) / 3.0, 1.0);
	}
	else {
		FragColor = vec4(0.0, green, blue, 1.0);
	}
	*/

	//Add the ability to rotate the color using uniforms
	vec3 rotatedColor = vec3(red, green, blue);
	
	rotateColorX(rotatedColor, redRotationTheta);
	rotatedColor = clamp(rotatedColor, 0.01, 1.0); //Clamp the rotation to prevent out-of-range colors

	rotateColorY(rotatedColor, greenRotationTheta);
	rotatedColor = clamp(rotatedColor, 0.01, 1.0);

	rotateColorZ(rotatedColor, blueRotationTheta);
	rotatedColor = clamp(rotatedColor, 0.01, 1.0);

	Color = vec4(rotatedColor, 1.0);


	//Color = vec4(red, green, blue, 1.0);

	//FragColor = vec4(max(red, green), abs(sin((green + red)*(3.14159*2.0))), max(max(red, green),blue), 1.0);
	//FragColor = vec4( (red + green) / 2.0, (green + blue) / (2.0 + red), max(blue, 0.25), 1.0);
	

#endif //USE_COLOR_SCHEME_2




	

		
}