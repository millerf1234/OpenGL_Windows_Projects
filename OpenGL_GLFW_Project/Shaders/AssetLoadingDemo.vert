//Vertex Shader for the Asset Loading Demo
//
// Programmer:  Forrest Miller
// Date:        November 28, 2018

#version 450 core

layout(location = 0) in vec4 ModelPosition;
layout(location = 1) in vec2 ModelTexCoord;
layout(location = 2) in vec3 ModelNormal;

out vec4 position;
out vec2 texCoord;
out vec3 normal;

uniform float zoom;
uniform float time;
uniform mat4 rotation;

uniform mat4 MVP;

uniform float instanceSpiralPatternPeriod_x;  //Modifier for the x value of the pattern used when drawing instances 
uniform float instanceSpiralPatternPeriod_y;  //Modifier for the x value of the pattern used when drawing instances



#define TRANSLATE_INSTANCES_BEFORE_MVP_TRANSFORM  //Comment/Uncomment to toggle when the translation between each instance is applied 


float noise(in vec2 p);
float voronoi(in vec2 x);

vec3 combinedVoronoi(in vec3 c) {
	return vec3(noise(c.xy), noise(c.yz), noise(c.xz));
}


void main() {

	//Keep things very simple for now...
	position = ModelPosition + vec4(0.0, 0.0, 0.0, zoom);
	//position.y *= 1.5;

	texCoord = ModelTexCoord;

	//Normally a seperate matrix is needed for transforming the normal vectors.
	// However this has not yet been implemented...
	normal = mat3(rotation) * ModelNormal;


#ifdef TRANSLATE_INSTANCES_BEFORE_MVP_TRANSFORM
	float radius = float(gl_InstanceID) / 0.10;
	//position.x += radius * sin(atan((noise(115.*position.yx)-float(gl_InstanceID) * noise(vec2(instanceSpiralPatternPeriod_x, sqrt(instanceSpiralPatternPeriod_x))))));
	//position.y += radius * acos(sin(float(gl_InstanceID) + instanceSpiralPatternPeriod_y));
	position.z += smoothstep(-abs(sin(0.3*time+2.*noise(position.xy))), abs(sin(time + 2.*noise(position.xy))), position.z);
	position.x += 1.2*noise(3.*(0.75 + 0.35*sin(time))*(position.yz + position.xy));
	position.y += 1.3 * noise(position.zx);
	gl_Position = MVP * position;

#else 
	gl_Position = MVP * position;
	float radius = float(gl_InstanceID) / 2.0;
	gl_Position.x += radius * cos(float(gl_InstanceID) * instanceSpiralPatternPeriod_x);
	gl_Position.y += radius * sin(float(gl_InstanceID) * instanceSpiralPatternPeriod_y);
#endif 
	
	
}