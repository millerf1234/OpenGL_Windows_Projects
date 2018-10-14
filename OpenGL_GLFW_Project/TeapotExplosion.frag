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
#define USE_COLOR_SCHEME_2
//#define USE_COLOR_SCHEME_3


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

#ifdef USE_COLOR_SCHEME_3
	float blue = (g2f_pos.y+g2f_pos.x + colorModificationValue0) / 2.0;
	float green = 0.05 * (20.3 * sin(time*5.0) - length(g2f_vel.xyz));
	float red = length(g2f_vel.xyz) / (9.0 * length(g2f_pos.xyz)) + (length(dFdxCoarse(g2f_vel.xyz)) + length(dFdy(g2f_vel.xyz)) / 2.5);

	if (blue > 1.0 + colorModificationValue3 * sin(colorModificationValue4 * time * g2f_pos.y / (1.0f + colorModificationValue2))) {
		blue -= (1.0 + colorModificationValue5) + (colorModificationValue6 * g2f_vel.y * 3.0*acosh(time));
		red = blue / (blue + 3.0 + 0.5 * cos(length(g2f_vel)*(time + colorModificationValue4) + colorModificationValue5 * sqrt(gl_FragCoord.x*length(gl_FragCoord.xyz + dFdy(g2f_pos)))));
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
		red = max(red, length(dFdx(g2f_vel.xxx) + dFdy(g2f_vel.yyy)));
		if (red > 0.9) {
			red = red / (red + abs(green) + abs(blue) - colorModificationValue3);
		}
		green = (red + green + blue) / 3.0;
		rotatedColor += dFdy(cross(vec3(red, -green, -blue), dFdx(g2f_pos + g2f_vel + gl_FragCoord.zyx) * abs(sin(gl_FragCoord.y - gl_FragCoord.x))));
	}
	rotatedColor = abs(rotatedColor + vec3(red, green, (red + green) / 2.0));
	Color = vec4(abs(cross(rotatedColor.zyx, dFdx(rotatedColor))), 1.0);

	if (length(Color.xyz) < (0.25*colorModificationValue5 + 0.05*sin(g2f_pos.x + gl_FragCoord.y*time))) {
		Color = abs(abs(vec4(max(max(g2f_pos.x, g2f_vel.x), 2.0*sin(time+abs(gl_FragCoord.x))), 0.75, 0.75 + 0.2*sinh(time+length(g2f_vel)), 0.0) - Color) - Color) ;
	}


	
#endif //USE_COLOR_SCHEME_3

}