//Vertex Shader 

#version 450 core

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////      HOW TO USE    /////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//Define One Of the Following Shaders To Use It
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define BASIC_VERT
#define UNDERWATER_EFFECT_OF_SORTS
#define VOLATILE_SURFACE
#define INSTANCED_CIRCLE_VERT
#define EXTRA_ROTATION_PER_INSTANCE_VERT
#define COOL_VERT

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//Modify the following to affect overall behavior
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//Determines how many unique VertexID values will be possible 
//for effects that have a repeating per-vertex pattern
#define VERTEX_ID_MOD_VALUE 46   

///////////////////////////////////////////////////////////////////////////////////////
//////////////////////       End Of Configuration       ///////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////


//The next 50 lines or so are just boilerplate

//What the input data looks like
layout(location = 0) in vec4 ModelPosition;
layout(location = 1) in vec2 ModelTexCoord;
layout(location = 2) in vec3 ModelNormal;

//What our Vertex Shader output should look like 
//(this struct represents a single vertex)
out VERTEX_SHADER_OUTPUT {  
    float vertID;
    float vertIDMod;
    float instanceID;
    vec4 position;
    vec2 texCoord;
    vec3 normal;
} processed_vertex;


//Some parameters that are passed in by the Application
uniform float zoom;
uniform float time;
uniform mat4 rotation;
uniform mat4 MVP;

uniform uint customParameter1, customParameter2, customParameter3;
const float noiseAmpl_A = 1.0 + (float(customParameter2) * 0.5);
const float noiseAmpl_B = 1.0 + (float(customParameter3) * 0.1);
const float noiseAmpl = noiseAmpl_A*noiseAmpl_B;

#define vert float(1+gl_VertexID)
#define vertMod float(getVertIDMod(gl_VertexID))
#define inst float(gl_InstanceID+1)

///////////////////////////////////////////////////////////////////////////
// EXTERNAL FUNCTION PROTOTYPES  [for noise]
///////////////////////////////////////////////////////////////////////////
float noise(vec2 p);           //Generic 2d noise       
float pNoise(vec2 p, int res); //2d Perlin
float snoise(vec2 v);          //2d simplex noise
float cnoise(vec4 P);          //4d Periodic Classic Perlin Noise
float fbm(float x);            //1d Fractal Brownian Motion
float fbm(vec2 x);             //2d Fractal Brownian Motion
float fbm(vec3 x);             //3d Fractal Brownian Motion


////////////////////////////////////////////////////////////////////////////
////  Local Functions Implementing Shared Functionality Between Shaders
////////////////////////////////////////////////////////////////////////////

// Computes the VertexIDMod value
int getVertIDMod(in int vertexID) {
    return (vertexID % int(VERTEX_ID_MOD_VALUE)) + 1;
}



#if defined BASIC_VERT

void main() {
    processed_vertex.vertID = vert;
    processed_vertex.vertIDMod = vertMod;
    processed_vertex.instanceID = inst;
    processed_vertex.texCoord = ModelTexCoord;
	processed_vertex.normal = mat3(rotation) * ModelNormal;

    const float instanceDistanceAmplitude = 5. + abs(log2(noiseAmpl * snoise(time*0.005*vec2(cross(vec3(ModelTexCoord, vertMod + inst), ModelNormal)))) * float(gl_InstanceID));

    vec3 instanceDisplacementVector = vec3(0.0);
    vec3 instanceDisplacement = instanceDistanceAmplitude * vec3(cos(1.75*(time / (0.1*pow(1.05,inst)))),
                                                                 sin(1.25*(0.25*time / (0.1*pnoise(vec2(inst, exp(inst)), int(customParameter3))))),
                                                                 0.0);

	processed_vertex.position = MVP * (ModelPosition + vec4(instanceDisplacement, zoom));

    gl_Position = processed_vertex.position;
//	gl_Position = MVP * processed_vertex.position;

    gl_PointSize = vertMod * cos(vertMod*time*max(1.0, pow(vertMod, noiseAmpl))) * (cnoise(2.0*vec4(2.5*processed_vertex.position.xy, 5.0 * sin(0.25*(time + vertMod)), cos(0.25*(time + vertMod * 3.14/2.0)))));
}

#elif defined UNDERWATER_EFFECT_OF_SORTS
void main() {
    processed_vertex.vertID = vert;
    processed_vertex.vertIDMod = vertMod;
    processed_vertex.instanceID = inst;
    processed_vertex.texCoord = ModelTexCoord;
    processed_vertex.normal = mat3(rotation) * ModelNormal;
                                                                                    
    processed_vertex.position = ModelPosition + smoothstep(vec4(-20.0 + 15.5*cos(inst*(time / vertMod))),
                                                           vec4(175.0+110.0 *sin(0.3 * inst - 0.5*3.14159 * time*cos(time + 1.9*inst)), 8.0+5.0 *cos(0.5*3.14159 * time + 1.5*inst), 0.0, 0.45*zoom),
                                                           ModelPosition);


    
    gl_Position = MVP * processed_vertex.position;

    gl_Position.xy *= 1.5 + abs(
                                0.45*cnoise(
                                            0.124*gl_Position + vec4(
                                                                     sin(
                                                                         vertMod+time
                                                                         ),
                                                                     cos(
                                                                         sin(
                                                                             -vertMod+time
                                                                             )
                                                                         ),
                                                                     cos(
                                                                         cos(
                                                                             inst+vertMod
                                                                             )
                                                                         ),
                                                                     zoom
                                                                     )
                                           )
                               );
                                
}


#elif defined VOLATILE_SURFACE
void main() {
    processed_vertex.vertID = vert;
    processed_vertex.vertIDMod = vertMod;
    processed_vertex.instanceID = inst;
    processed_vertex.texCoord = ModelTexCoord;

	processed_vertex.normal = mat3(rotation) * ModelNormal;

	processed_vertex.position = (MVP * (ModelPosition + (vec4(0.0, 0.0, 0.0, 2.5*zoom))) + (vec4(floor((inst+4.0) / 5.0) * cos(6.28 / 5.0 * inst + vert*time), floor((inst + 4.0) / 5.0) * sin(6.28 / 5.0 * inst), 0.0, 0.0)));
	gl_Position = processed_vertex.position;
}

#elif defined INSTANCED_CIRCLE_VERT
void main() {
    processed_vertex.vertID = vert;
    processed_vertex.vertIDMod = vertMod;
    processed_vertex.instanceID = inst;
    processed_vertex.texCoord = ModelTexCoord;

	processed_vertex.normal = mat3(rotation) * ModelNormal;


	//Step value (for second step function) determined through experimentation
	processed_vertex.position = MVP * (rotation * ModelPosition + vec4(0., 0., 0., zoom)) + step(-inst, -0.1) * vec4(15.0*cos(inst), 25.0*sin(inst), 0., 0.0) + step(-inst, -711.01) * vec4(45.0*cos(inst), 65.0*sin(inst), 0.0, 0.);
	gl_Position = processed_vertex.position;
}

#elif defined EXTRA_ROTATION_PER_INSTANCE_VERT
void main() {
    processed_vertex.vertID = vert;
    processed_vertex.vertIDMod = vertMod;
    processed_vertex.instanceID = inst;
    processed_vertex.texCoord = ModelTexCoord;

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
    processed_vertex.vertID = vert;
    processed_vertex.vertIDMod = vertMod;
    processed_vertex.instanceID = inst;
    processed_vertex.texCoord = ModelTexCoord;



	float cool = (sin(1.24*(time / (0.2*inst + 1) - (0.009*vert))));
	cool *= step(0.257, abs(cool));
	position = ModelPosition + vec4(3.0*cos(inst + cool), (1.0 - inst) / cool, (0.5*inst) / cool, zoom);



	//Normally a separate matrix is needed for transforming the normal vectors.
	// However this has not yet been implemented...
	normal = mat3(rotation) * ModelNormal;


	gl_Position = (MVP * position);
}

#else 
 void main() {
    processed_vertex.vertID = vert;
    processed_vertex.vertIDMod = vertMod;
    processed_vertex.instanceID = inst;
    processed_vertex.texCoord = ModelTexCoord;


	float cool = (sin(1.24*(time / (0.2*inst + 1) - (0.009*vert))));
	cool *= step(0.257, abs(cool));
	position = ModelPosition + vec4(3.0*cos(inst + cool), (1.0 - inst) / cool, (0.5*inst) / cool, zoom);


	texCoord = ModelTexCoord;

	//Normally a separate matrix is needed for transforming the normal vectors.
	// However this has not yet been implemented...
	normal = mat3(rotation) * ModelNormal;


	gl_Position = (MVP * position);

 }

#endif