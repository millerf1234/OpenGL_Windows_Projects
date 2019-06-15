/* RenderDemo:              Flying Camera Demo                    */
/* Stage:                    FRAGMEMT SHADER                      */
/*                                                                */
#version 450 core   
// [+~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+]
// [|---------------------------  Header  ---------------------------|]
// [+~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+]
// [|  File:    FlyingCameraDemo.frag                                |]
// [|  Title:   Primary Fragment Shader                              |]
// [|  Desc:    [todo]                                               |]
// [|                                                                |]
// [|                                                                |]
// [|  Author:  Forrest S Miller                                     |]
// [|  Date:    June 15, 2019                                        |]
// [+----------------------------------------------------------------+]



//   THE COMMENTING IN THIS SHADER WILL BE QUITE A BIT MORE SPARSE THAN THE VERTEX SHADER



in SHADED_VERTEX_DATA { 
    vec4 position;
    vec3 normal; 
    vec2 textureCoord; 
} shaded_vertex;     


out vec4 fragColor;


uniform float time;

//Function to call for fun
float sinePartialSummation(float x, int intervalsToCompute, float intervalSpacing) {
    float summation = sin(x);
    for (int i = 1; i <= max(1, intervalsToCompute); i++) {
        summation += (1.0 / pow(intervalSpacing, i)) * sin( intervalSpacing * x);
    }
    return summation;
}

//Function to call for fun
float cosinePartialSummation(float x, int intervalsToCompute, float intervalSpacing) {
    float summation = cos(x);
    for (int i = 1; i <= max(1, intervalsToCompute); i++) {
        summation += (1.0 / pow(intervalSpacing, i)) * sin( intervalSpacing * x);
    }
    return summation;
}


void main () {
    
    //Write to the framebuffer 
    fragColor = vec4(0.75 + 0.25*sin(sinePartialSummation(time, 23, 0.1)),    //r
                     0.5 + length(shaded_vertex.position),                    //g
                     cos(length(shaded_vertex.position)),                               //b
                     0.35);                                                    //a
    
}


