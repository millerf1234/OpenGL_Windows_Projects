//Vertex Shader 


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

#define mod1 instanceSpiralPatternPeriod_x; //That long name I chose is a pain to type 
#define mod2 instanceSpiralPatternPeriod_y;


#define vert float(gl_VertexID)
#define inst (float(gl_InstanceID)*2.0)

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


#define BASIC_VERT
#define INSTANCED_CIRCLE_VERT
#define EXTRA_ROTATION_PER_INSTANCE_VERT
#define COOL_VERT


#if defined BASIC_VERT

void main() {
	normal = mat3(rotation) * ModelNormal;

	position = MVP * (ModelPosition + vec4(0.0, 0.0, 0.0, zoom + (0.005 + (0.003 * sin(time)) * vert)));
	gl_Position = position;
}

#elif defined INSTANCED_CIRCLE_VERT

void main() {
	normal = mat3(rotation) * ModelNormal;

	//texCoord = ModelTexCoord;

	//Step value (for second step function) determined through experimentation
	position = MVP * (rotation * ModelPosition + vec4(0., 0., 0., zoom)) + step(-inst, -0.1) * vec4(15.0*cos(inst), 25.0*sin(inst), 0., 0.0) + step(-inst, -711.01) * vec4(45.0*cos(inst), 65.0*sin(inst), 0.0, 0.);
	gl_Position = position;
}

#elif defined EXTRA_ROTATION_PER_INSTANCE_VERT
void main() {

	float flip = -1.0;
	float flipper = -1.0;
	for (int i = 0; i < gl_InstanceID; i++) {
		flip *= flipper;
	}

	mat4 transl = mat4(0.0, 0.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 0.0,
		flip*(0.25 + 0.25*sin(time)), flip*0.25, flip * 0.025, 0.0);

	position = ModelPosition + vec4((16.0 + 4.0*cos(time - inst)) * sin(3.14*inst / 60.0), 16.0*sin(3.14*inst / 70.0 + 1.71), floor(0.05*inst), zoom);

	texCoord = ModelTexCoord;

	mat4 repeatedRotation = mat4(1.0);

	for (int i = 0; i < gl_InstanceID; i++) {
		repeatedRotation *= rotation;
		repeatedRotation += transl;
	}

	normal = mat3(repeatedRotation * rotation) * ModelNormal;

	gl_Position = (MVP * repeatedRotation) * position;
}



#elif defined COOL_VERT
void main() {

	float cool = (sin(1.24*(time / (0.2*inst + 1) - (0.009*vert))));
	cool *= step(0.257, abs(cool));
	position = ModelPosition + vec4(3.0*cos(inst + cool), (1.0 - inst) / cool, (0.5*inst) / cool, zoom);


	texCoord = ModelTexCoord;

	//Normally a seperate matrix is needed for transforming the normal vectors.
	// However this has not yet been implemented...
	normal = mat3(rotation) * ModelNormal;


	gl_Position = (MVP * position);
}

#endif