//Fragment shader for the GeomtryShaderExplosion demo project. 
//
// This shader code has been me just messing around and as such it is
// quite sloppy. I wasn't ever really trying to go for anything in particular,
// just seeing what patterns I could make. 
//
// To Use:  Uncomment one of the "USE_COLOR_SCHEME" macros at a time
//          to switch between color schemes. 
//
// Forrest Miller on 9/28/2018

#version 450 core

flat in vec3 g2f_pos;  //Position data from geomtry shader
in vec3 g2f_vel;  //Velocity data from geometry shader

out vec4 Color;


uniform float time;
uniform float colorShiftThreshhold;

uniform float redRotationTheta;
uniform float greenRotationTheta;
uniform float blueRotationTheta;

uniform float colorModificationValue0;
uniform float colorModificationValue1;
uniform float colorModificationValue2;
uniform float colorModificationValue3;
uniform float colorModificationValue4;
uniform float colorModificationValue5;
uniform float colorModificationValue6;

//Only define 1 of the following to determine color scheme
//#define USE_COLOR_SCHEME_0 
//#define USE_COLOR_SCHEME_1  
//#define USE_COLOR_SCHEME_2
//#define USE_COLOR_SCHEME_3
//#define USE_COLOR_SCHEME_4
#define USE_COLOR_SCHEME_4_1

//Rotation Functions
//Implementation for the rotation functions (these were here before I made a seperate file of frag shader utility functions)
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


void main() {

#if defined USE_COLOR_SCHEME_0
	vec3 position = vec3(abs(g2f_pos));
	Color = normalize(vec4(dot(position, g2f_vel), cross(position, g2f_vel)));
#endif //USE_COLOR_SCHEME_0



#if defined USE_COLOR_SCHEME_1

	vec3 color = vec3((abs(sin(time - g2f_pos.x + g2f_pos.y * 3.14159)*g2f_vel.x)), abs(g2f_pos.y), abs(g2f_pos.z));

	rotateColorX(color, redRotationTheta);
	color = clamp(color, 0.01, 1.0); //Clamp the rotation to prevent out-of-range colors

	rotateColorY(color, greenRotationTheta);
	color = clamp(color, 0.01, 1.0);

	rotateColorZ(color, blueRotationTheta);
	color = clamp(color, 0.01, 1.0);

	if (length(color) < 0.25) {
		color = vec3(1.0, 1.0, 1.0) - color;
	}
	Color = vec4(color, 1.0);

	
#endif //USE_COLOR_SCHEME_1


#if defined USE_COLOR_SCHEME_2
	float blue = (g2f_pos.y + 1.0) / 2.0;
	float green = 0.05 * (20.3 * sin(time*5.0) - length(g2f_vel.xyz));
	float red = length(g2f_vel.xyz) / (9.0 * length(g2f_pos.xyz));

	if (blue > 1.0) {
		blue -= 1.0;
		red = blue / (blue + 3.0);
	}

	float cutoff = colorShiftThreshhold; 

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
	//rotatedColor = clamp(rotatedColor, 0.01, 1.0); //Clamp the rotation to prevent out-of-range colors

	rotateColorY(rotatedColor, greenRotationTheta);
	//rotatedColor = clamp(rotatedColor, 0.01, 1.0);

	rotateColorZ(rotatedColor, blueRotationTheta);
	rotatedColor = clamp(rotatedColor, 0.01, 1.0);

	Color = vec4(rotatedColor, 1.0);


	//Color = vec4(red, green, blue, 1.0);

	//FragColor = vec4(max(red, green), abs(sin((green + red)*(3.14159*2.0))), max(max(red, green),blue), 1.0);
	//FragColor = vec4( (red + green) / 2.0, (green + blue) / (2.0 + red), max(blue, 0.25), 1.0);
	

#endif //USE_COLOR_SCHEME_2

	//I have since learned that the way I was using the GLSL functions dFdx and dFdy 
	//within this color scheme broke the rules of those functions. I think I removed 
	//every place I was causing undefined behavior, but there may still be a few lingering.
	//Basically it is not allowed to use those functions in code that has branching, in addition
	//to some other restrictions. 
	//This guys website provides an excellent overview:
	//       http://www.aclockworkberry.com/shader-derivative-functions/
#ifdef USE_COLOR_SCHEME_3
	float blue = (g2f_pos.y+g2f_pos.x + colorModificationValue0) / 2.0;
	float green = 0.05 * (20.3 * sin(time*5.0) - length(g2f_vel.xyz));
	float red = length(g2f_vel.xyz) / (9.0 * length(g2f_pos.xyz));

	if (blue > 1.0 + colorModificationValue3 * sin(colorModificationValue4 * time * g2f_pos.y / (1.0f + colorModificationValue2))) {
		blue -= (1.0 + colorModificationValue5) + (colorModificationValue6 * g2f_vel.y * 3.0*acosh(time));
		red = blue / (blue + 3.0 + 0.5 * cos(length(g2f_vel)*(time + colorModificationValue4) + colorModificationValue5 * sqrt(gl_FragCoord.x*length(gl_FragCoord.xyz + g2f_pos))));
	}

	float cutoff = colorShiftThreshhold; //+ 0.25*abs(sin(time)); //length((0.15 + noise3(vec3(red, green, blue))));  //0.2 

	if ((length(vec3(red, green, blue)) < cutoff)) {
		red += abs(gl_FragCoord.z + pow((0.5 + colorModificationValue1), (0.65*sin(colorModificationValue2*time + time * g2f_vel.x))));
		green += abs( ((gl_FragCoord.x * gl_FragCoord.x) + (abs(g2f_pos.x)/abs(g2f_vel.x))) - gl_FragCoord.y / 2.0);
		blue += 0.75 * (sin(time * gl_FragCoord.y) + cos(time * gl_FragCoord.x));
	}
	else {
		red *= g2f_vel.z;
		green = (red * sin(g2f_vel.x + g2f_vel.y) + green * sin(g2f_vel.z * 2.0 + time + colorModificationValue1)) / (red + green - 1.0 / colorModificationValue2);
	}

	//Add the ability to rotate the color using uniforms
	vec3 rotatedColor = vec3(red, green, blue);
	vec3 rotatedColor2 = vec3(max(red + green, red+blue), max(red - green + blue, blue-red+green), blue + 0.25*sinh((gl_FragCoord.y-gl_FragCoord.x) * 13.7*time));

	//rotatedColor = clamp(-abs(cross(rotatedColor, rotatedColor2)), -1.0, 1.0);

	rotateColorX(rotatedColor, redRotationTheta);
	//rotatedColor = clamp(rotatedColor, 0.01, 1.0); //Clamp the rotation to prevent out-of-range colors

	rotateColorY(rotatedColor, greenRotationTheta);
	//rotatedColor = clamp(rotatedColor, 0.01, 1.0);

	rotateColorZ(rotatedColor, blueRotationTheta);
	//rotatedColor = clamp(rotatedColor, 0.01, 1.0);

	if (rotatedColor.z < min(abs(colorModificationValue0 + colorModificationValue1) + length(g2f_vel + gl_FragCoord.yxx), abs(colorModificationValue2 + colorModificationValue3 + colorModificationValue4 - colorModificationValue5 * sin(time + colorModificationValue6)))) {
		red = max(red, length(g2f_vel.xxx + g2f_vel.yyy));
		if (red > 0.9) {
			red = red / (red + abs(green) + abs(blue) - colorModificationValue3);
		}
		green = (red + green + blue) / 3.0;
		//rotatedColor += cross(vec3(red, -green, -blue), g2f_pos + g2f_vel + gl_FragCoord.zyx * abs(sin(gl_FragCoord.y - gl_FragCoord.x))));
	}
	rotatedColor = abs(rotatedColor + vec3(red, green, (red + green) / 2.0));
	Color = vec4(abs(cross(rotatedColor.zyx, rotatedColor)), 1.0);

	if (length(Color.xyz) < (0.25*colorModificationValue5 + 0.05*sin(g2f_pos.x + gl_FragCoord.y*time))) {
		Color = abs(abs(vec4(max(max(g2f_pos.x, g2f_vel.x), 2.0*sin(time+abs(gl_FragCoord.x))), 0.75, 0.75 + 0.2*sinh(time+length(g2f_vel)), 0.0) - Color) - Color) ;
	}
	Color.a = 1.0; //Just in case the alpha channel got messed up somewhere in there along the way

	
#endif //USE_COLOR_SCHEME_3

	//This color scheme tells me that the velocity written in thge geometry shader is not what I think it is
#if defined USE_COLOR_SCHEME_4
	vec3 baseColor = vec3(0.25, 0.25, 0.25);
	vec3 slowColor = vec3(0.0, 1.0 - abs(0.75 * sin(time)), 0.0);
	vec3 medColor = vec3(0.0, 0.0, 1.0);
	vec3 fastColor = vec3(1.0, 0.0, 0.0);

	float speed = 0.25 * length(g2f_vel.xyz);
	vec3 col;
	if (speed < 0.5)
		col = baseColor;
	else if (speed < 0.95)
		col = baseColor + slowColor; //col = baseColor + (vec3(2.0) - (speed * slowColor));
	else if (speed < 1.85)
		col = baseColor + medColor;
	else if (speed < 5.5)
		col = baseColor + slowColor + medColor;
	else if (speed < 19.5)
		col = baseColor + medColor + fastColor;
	else
		col = baseColor + fastColor /*- slowColor*/;
	Color = vec4(col, 1.0);
#endif //USE_COLOR_SCHEME_4


	//This color scheme was me trying to play around with assigning color based on 
	//the velocity written in the geometry shader. It did not work as I had intended.
#if defined USE_COLOR_SCHEME_4
	vec3 baseColor = vec3(0.25, 0.25, 0.25);
	vec3 slowColor = vec3(0.0, 1.0, 0.0);
	vec3 medColor = vec3(0.0, 0.0, 1.0);
	vec3 fastColor = vec3(1.0, 0.0, 0.0);

	float speed = 0.25 * length(g2f_vel.xyz);
	vec3 col;
	if (speed < 0.5)
		col = baseColor;
	else if (speed < 0.95)
		col = baseColor + slowColor; //col = baseColor + (vec3(2.0) - (speed * slowColor));
	else if (speed < 1.85)
		col = baseColor + medColor;
	else if (speed < 5.5)
		col = baseColor + slowColor + medColor;
	else if (speed < 19.5)
		col = baseColor + medColor + fastColor;
	else
		col = baseColor + fastColor /*- slowColor*/;
	Color = vec4(col, 1.0);
#endif //USE_COLOR_SCHEME_4


	//This is based off color scheme 4, but with some more fancy modifications
#if defined USE_COLOR_SCHEME_4_1
	//vec3 posNoise = noise3(g2f_pos);
	//vec3 coordNoise = noise3(gl_FragCoord.xyz);

	vec3 baseColor = vec3(0.25, 0.25, 0.25);
	vec3 slowColor = vec3(0.0, 1.0 - abs(0.75 * sin(time)), 0.0);
	vec3 medColor = vec3(0.0, 0.0, 1.0);
	vec3 fastColor = vec3(1.0, 0.0, 0.0);

	float speed = 0.25 * length(g2f_vel.xyz);
	vec3 col;
	if (speed < 0.5)
		col = baseColor;
	else if (speed < 0.95)
		col = baseColor + slowColor; //col = baseColor + (vec3(2.0) - (speed * slowColor));
	else if (speed < 1.85)
		col = baseColor + medColor;
	else if (speed < 5.5)
		col = baseColor + slowColor + medColor;
	else if (speed < 19.5)
		col = baseColor + medColor + fastColor;
	else
		col = baseColor + fastColor;

	Color = vec4(col, 1.0);
#endif //USE_COLOR_SCHEME_4_1
}