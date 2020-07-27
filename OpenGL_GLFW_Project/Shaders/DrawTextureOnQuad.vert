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


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//Modify the following to affect overall behavior
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//Determines how many unique VertexID values will be possible 
//for effects that have a repeating per-vertex pattern
#define VERTEX_ID_MOD_VALUE 13   

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
#define inst float((gl_InstanceID/3)+1)


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//External Noise Function Declarations [Func Defs found in "ShaderNoiseFunctions.glsl"]
float noise(float p);
float noise(vec2 n);
float noise(vec3 P);
//Perlin
float pNoise(vec2 p, int res);
//Periodic Perlin
float cnoise(vec4 P);
//Classical Periodic Perlin
float cnoise(vec4 P, vec4 rep);
//Simple
float snoise(vec2 v); 
float snoise(vec3 v);
float snoise(vec4 v);
//Fractal Brownian Motion
float fbm(float x);
float fbm(vec2 x);
float fbm(vec3 x);
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~



////////////////////////////////////////////////////////////////////////////
////  Local Functions Implementing Shared Functionality Between Shaders
////////////////////////////////////////////////////////////////////////////

// Computes the VertexIDMod value
int getVertIDMod(in int vertexID) {
    return (vertexID % int(VERTEX_ID_MOD_VALUE)) + 1;
}

vec2 rotate2(const in vec2 v, const in float theta) {
    const mat2 rotation = mat2(cos(theta), sin(theta),
                              -sin(theta), cos(theta));
    return rotation * v;
}



#if defined BASIC_VERT

void main() {
    processed_vertex.vertID = vert;
    processed_vertex.vertIDMod = vertMod;
    processed_vertex.instanceID = inst;

    processed_vertex.normal = mat3(rotation) * ModelNormal;
    processed_vertex.texCoord.xy = rotate2(ModelTexCoord, 0.0);
    processed_vertex.position = MVP * ModelPosition;

    if (false) { //Old Way
        vec4 intermediate_position = processed_vertex.position + vec4(2.025*inst*sin(2.04*inst), 2.112975*inst*cos(1.14*inst), 0.0, zoom);
        intermediate_position.xyz *= 1.0 + (float(gl_InstanceID % 2)*.005 * float(gl_VertexID));
        gl_Position = intermediate_position;
    }

    else { //New Way using noise

        //Compute the position of instance 0
        vec4 basePosition = processed_vertex.position; 
        basePosition.w += zoom; //Apply zoom

        const float instTranslationAmount = 0.0162*float(gl_InstanceID);
        float instPhase = .4 + .31*float(gl_InstanceID);
        vec2 instTranslationDirection = vec2(cos(instPhase*time), sin(instPhase*time));
        basePosition.xy += instTranslationAmount*instTranslationDirection;
        basePosition.yz += .1*instTranslationAmount*instTranslationDirection;
        if (gl_InstanceID > 0) {
        //Now add an ever-so-slight amount of noise per-vertex
        vec3 noiseFactor = .1*vec3(cos(3.14*sin(.073*time+vertMod)),
                                   sin(3.14*cos(.073*time)),
                                   .1*cos(3.14 * (cos(/*float(gl_VertexID)*/vertMod*.1*time)+.1*sin(.041*time))));
        float moreNoise = cnoise(basePosition + noiseFactor.xyxy);
        float perVertexNoise = 0.48*snoise(moreNoise*(basePosition.xyz + noiseFactor));
        basePosition.xyz *= 1.0+.02*float(gl_InstanceID)*perVertexNoise;
        }
        gl_Position = basePosition;
        gl_Position.x *= 1.35;
        gl_Position.w *= ((1.0+.008*float(gl_InstanceID)) / 1.0);

    }

    gl_PointSize = vertMod / (2.0 - .025*float(customParameter1%7));
}

#endif 