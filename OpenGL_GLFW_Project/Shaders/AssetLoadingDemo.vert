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
	float offset = smoothstep(-2.50*abs(2.15*sin(0.123*time + 1003.0 * cos(0.03*time)*noise(position.xy + vec2((instanceSpiralPatternPeriod_y * float(gl_InstanceID)))))),
		instanceSpiralPatternPeriod_y * float(gl_InstanceID) * 2.50 + 1.5,
		abs(sin(time + 2.*noise(3.0*position.xy)) - position.z));
	//float radius = float(gl_InstanceID) / 0.10;
	//position.x += radius * sin(atan((noise(115.*position.yx)-float(gl_InstanceID) * noise(vec2(instanceSpiralPatternPeriod_x, sqrt(instanceSpiralPatternPeriod_x))))));
	//position.y += radius * acos(sin(float(gl_InstanceID) + instanceSpiralPatternPeriod_y));
	position.z += offset;
	position.y += 0.0251*noise(3.0*vec2(offset, position.z) + 1.2*cos(time));
	position.x += 2.0*sin(0.25*offset) + cos(offset);
	//position.z *= step(0.007, length(position.z));
	//position.z += (step(-0.1, -length(position.z)) * 0.007*cos(1.7 - time * 3.1415));
	//position += 5.0050*length(vec3(smoothstep(-13.5, 13.5, position.z) * sign(position.z))) * position;
	//position.x += 3.0*noise(vec2(3.35*sin(time)*(position.z + position.y), 10.0*cos(time / 10.0)));
	//position.y = clamp(position.y, -2.5, 1.5);
	gl_Position = (MVP * position); /* +
		vec4((radius * (cos(sin(tan(0.3 + cos(float(gl_InstanceID) * instanceSpiralPatternPeriod_x))))) * sin(float(gl_InstanceID) * 3.141 / 1.29) + sin(float(gl_InstanceID) * instanceSpiralPatternPeriod_x + instanceSpiralPatternPeriod_y * 3.14)),
			radius * cos(instanceSpiralPatternPeriod_y),
			0.2*cos(time + float(gl_InstanceID) * 3.141 / 27.9),
			0.0) +
		vec4(2.10 / (0.95 + float(gl_InstanceID) * step(float(gl_InstanceID), 4.0) * cos(gl_InstanceID * 3.14159 * instanceSpiralPatternPeriod_x / 77.81)),
			2.0 / (0.95 + float(gl_InstanceID) * step(float(gl_InstanceID), 3.0) * cos(gl_InstanceID * 3.94159 * instanceSpiralPatternPeriod_y / 37.45)),
			2.0 * cos(float(gl_InstanceID) + 3.14/22.0),
			0.0);
			*/

	float radius = float(gl_InstanceID) / ((2.50 + 1.05) * instanceSpiralPatternPeriod_y);
	gl_Position.x += (5.0 * floor(float(gl_InstanceID) / 6.0) * cos(float(gl_InstanceID * 3.14159) / 8.0));
	gl_Position.y += (4.0 * floor(float(gl_InstanceID) / 6.0) * sin(float(gl_InstanceID * 3.14159) / 8.0));
	
	//gl_Position.y += (1.0 / 4000.0) * floor(4001.0 * (abs(position.x) + abs(position.z)) + (abs(position.y) + abs(position.z)));
	//gl_Position.x += 3.0*radius + cos(float((gl_InstanceID)) * instanceSpiralPatternPeriod_x);

	//gl_Position += vec4(-150.8 + noise(gl_Position.xy), 4.0 * noise(gl_Position.xy), 0.0, 0.0);

	//gl_Position.x += 2050.0 * sin(float(gl_InstanceID) * 0.0001 * time);

#else 
	gl_Position = MVP * position;
	float radius = float(gl_InstanceID) / 2.0;
	gl_Position.x += radius + cos(float(gl_InstanceID) * instanceSpiralPatternPeriod_x);
	gl_Position.y += radius * sin(float(gl_InstanceID) * instanceSpiralPatternPeriod_y);
#endif 
	
	
}