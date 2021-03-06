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
    vec3 pretransform_normal;
    vec3 normal; 
    vec2 textureCoord; 
    float instanceID;
    float vertexID;
} shaded_vertex;     


out vec4 fragColor;


uniform float time;

//Function to call for creating waveforms other than sine. The third parameter 'intervalSpacing'
//should not be 0 and 
float sinePartialSummation(in float x, in int intervalsToCompute, in float intervalSpacing) {
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
        summation += (1.0 / pow(intervalSpacing, i)) * cos( intervalSpacing * x);
    }
    return summation;
}


#define USING_NOISE_02

#ifdef USING_NOISE_01
//                                                 Reference 
//  Callable Noise Functions included in this file:
//	 +-------------------------------------+------------------------------------------+---------------------------------+
//	 |   	       Noise Type              |            Function Signature(s)         |    Attibution (if available)    |
//   +-------------------------------------+------------------------------------------+---------------------------------+
//	 |              Generic                |                                          |                                 |
//   |                                     |           float noise(vec2 n)            |                                 |
//   |                                     |           float noise(vec3 P)            |                                 |
//   |                                     |                                          |                                 |
//   |              Perlin                 |        float pNoise(vec2 p, int res)     |                                 |
//   |        [Periodic Perlin]            |             float cnoise(vec4 P)         |        Stefan Gustavson         |
//   |     [Classic Periodic Perlin]       |        float cnoise(vec4 P, vec4 rep)    |        Stefan Gustavson         |
//   |                                     |                                          |                                 |
//   |                                     |                                          |                                 |
//   |              Simplex                |            float snoise(vec2 v)          |                                 |
//   |                                     |            float snoise(vec3 v)          |    Ian McEwan, Ashima Arts      |
//   |                                     |            float snoise(vec4 v)          |    Ian McEwan, Ashima Arts      |
//   |                                     |                                          |                                 |
//   |      Fractal Browning Motion*       |             float fbm(float x)           |                                 |
//   |                                     |             float fbm(vec2 x)            |                                 |
//   |                                     |             float fbm(vec3 x)            |                                 |
//   +-------------------------------------+------------------------------------------+---------------------------------+
//

float noise(vec2 p);
float snoise(vec3 v);
float fbm(vec2 x);
float fbm(vec3 x);
#endif 

#ifdef USING_NOISE_02
// FUNCTION:     vec3 psrdnoise(vec2 pos, vec2 per, float rot);
// ShaderFile:   "psrdnoise.glsl"
// Source:       Part of the MIT Permissive Licensed Shader Noise Package 
//               From AshimaArts. 
//
// 2-D tiling simplex noise with rotating gradients and analytical derivative.
// The first component of the 3-element return vector is the noise value,
// and the second and third components are the x and y partial derivatives.
//
vec3 psrdnoise(vec2 pos, vec2 per, float rot);
#endif 

void main() { 


#define EFFECT_TO_DO 0


//NO EFFECT SELECTED
#ifndef EFFECT_TO_DO
    fragColor = vec4(0.75, 0.35, 0.95, 0.75); }


//EFFECT 0 SELECTED
#elif (EFFECT_TO_DO <= 0)
    vec4 color1 = vec4(smoothstep(vec3(0.0, -2.0, 0.0), vec3(2.0, 0.2, 2.0), shaded_vertex.normal),
                       0.75+0.25*cos(shaded_vertex.vertexID + time)); 
    vec4 color2 = vec4(0.75 + 0.25*sin(sinePartialSummation(time, 23, 2.1)),    //r
                     0.2 + length(shaded_vertex.position) * sinePartialSummation(time + shaded_vertex.normal.x, 3, 4.0),                      //g
                     cos(length(shaded_vertex.position)),                       //b
                     0.35);                                                     //a
    vec4 color3 = vec4(0.275, 0.0245, 0.056, 0.8);//3.0*noise4(time);

    fragColor = mix(
                   mix(color1, color2, 0.5 + 0.35*sin(time + shaded_vertex.instanceID)),
                   color3, 0.5 + 0.3 * sinePartialSummation(time + shaded_vertex.instanceID, 6, 2.5));


    if (fragColor.g > 0.3 + 0.45 * sinePartialSummation(time + shaded_vertex.position.y, 3, 3.0)) 
        fragColor.b += 0.35*(float(shaded_vertex.textureCoord.s + shaded_vertex.textureCoord.t) / 2.3);

    if (abs(fragColor.g - fragColor.b) > (1.0 + 0.1*sin(time)*fragColor.r))
        fragColor.rgb = fragColor.rbg;

    if (length(fragColor.rgb) < 0.25)
        fragColor.rgb = vec3(1.0) - fragColor.rgb;

    if (abs(dot(shaded_vertex.pretransform_normal, vec3(0.0, 0.0, 1.0))) < 0.0005)
       fragColor = vec4(0.68, 0.125, 0.985, 0.75 - 0.005*shaded_vertex.instanceID);
       

    //if (length(fragColor.rgb) > length(vec3(0.9)))
    //    fragColor.rgb *= 0.5;


//EFFECT 1 SELECTED
#elif (EFFECT_TO_DO == 1)

    vec3 col = vec3(0.45, 0.45, abs(sinePartialSummation(time + shaded_vertex.instanceID * 3.14159 / 16., 6, 0.005)));
    float alpha = .5  + .2*sin(float(int(shaded_vertex.vertexID)%3)*3.141 / 17.);
    fragColor = vec4(dot(vec3(1.0, 1.0, 0.0), shaded_vertex.normal)*col, alpha);
    if (fragColor.r < 0.)
       fragColor.r *= -1.;
    if (fragColor.g < 0.)
       fragColor.g *= -1.;
    if (fragColor.b < 0.)
       fragColor.b *= -1.;

    if (abs(dot(shaded_vertex.pretransform_normal, vec3(0.0, 0.0, 1.0))) < 0.0005)
       fragColor = vec4(0.68, 0.125, 0.985, 0.75);// + vec4(psrdnoise(fragColor.rb, 0.05*fragColor.rg, 0.0), 0.0);
   // else 
    //   discard;


//EFFECT 2 SELECTED
#elif (EFFECT_TO_DO == 2)
    float scaleFactor = 1.0 / 1.269;
    float vertexIDFactor = 2.791/3.14 * shaded_vertex.vertexID;
    fragColor = vec4(0.5 + 0.35 * scaleFactor * sinePartialSummation(time + vertexIDFactor + snoise(vec3(time, sinePartialSummation(.25*time, int(vertexIDFactor), 20.+sin(vertexIDFactor*time)), noise(vec2(time, shaded_vertex.vertexID)))), 99, 4.),
                     0.5 + 0.35 * scaleFactor * sinePartialSummation(time + snoise(vec3(vertexIDFactor, shaded_vertex.vertexID, time)) + 0.014*shaded_vertex.instanceID, 99, 4.),
                     0.35 + fbm(vec2(time, time*abs(0.65 * scaleFactor * sinePartialSummation(.37*time + 0.025*pow(shaded_vertex.instanceID, 2.), 45, 2.12)))),
                     0.695);
    
    if (abs(dot(shaded_vertex.pretransform_normal, vec3(0., 0., 1.))) < .0005)
       fragColor = vec4(0.98, 0.625, 0.685, 0.75);

    fragColor.a = fragColor.a / (0.01*vertexIDFactor);


#elif (EFFECT_TO_DO == 3)
    //float noiseVal = snoise(vec3(sinePartialSummation(time + 0.014*shaded_vertex.instanceID, 99, max(2.25, 8.25 + gl_FragCoord.x + gl_FragCoord.y))));
    //vec3 color = vec3(abs(noiseVal + shaded_vertex.vertexID * 3.14 / 87.1), abs(cos(3.14*noiseVal)), 0.5 + 0.5*cos(time - noiseVal));
    //fragColor = vec4(color, 0.85 - 0.01*shaded_vertex.instanceID);

    fragColor = vec4(abs(sin(time*5.0*gl_FragCoord.x)), abs(sin(time*35.0*gl_FragCoord.y)), abs(cos(time*gl_FragCoord.x + gl_FragCoord.y)), 0.35); 

//Effect 4 SELECTED
#elif (EFFECT_TO_DO == 4)
    vec4 noise = noise4(shaded_vertex.position);
    fragColor = vec4(0.75, 0.65 + 0.36*cos(time+gl_FragCoord.x+gl_FragCoord.y), 0.55, 0.261) + noise;
    
//Effect 5 SELECTED
#elif (EFFECT_TO_DO == 5)
    const vec4 color1 = vec4(0.68, 0.09, 0.25, 1.0);
    const vec4 color2 = vec4(0.9, 0.7, 1.0, 0.25);

    vec2 temp = normalize(shaded_vertex.position.xy) - vec2(0.5);
    float f = dot(temp, temp);

    //if (f < 0.4 + 0.05 * cos(time))
        //discard;

    fragColor = mix(color1, color2, f);

    #elif (EFFECT_TO_DO == 6 || EFFECT_TO_DO == 7)
    fragColor = vec4( dot(shaded_vertex.normal, vec3(0.0, -1.0, 12.5))*sinePartialSummation(gl_FragCoord.x, 15, 200.5), 
                     dot(shaded_vertex.normal, vec3(0.0, 6.0, 2.5))*cosinePartialSummation(gl_FragCoord.y, 15, 200.5),
                     sinePartialSummation(gl_FragCoord.x * gl_FragCoord.y + time, 4, 2.76),
                     0.29);

    #elif (EFFECT_TO_DO >= 8) 
    const vec4 color1 = vec4(0.9, 0.4, 0.19, 0.5);
    const vec4 color2 = vec4(0.5, 0.6, 0.9, 0.85);
    const vec3 lightPosition = vec3(10.0 * cos(time), 8.0 * sin(time), sinePartialSummation(0.05*time, 8, 2.0));

    fragColor = mix(color1, color2, abs(dot(normalize(lightPosition), normalize(shaded_vertex.normal))));
    fragColor += vec4(abs(0.25*psrdnoise(vec2(shaded_vertex.instanceID, 0.0), shaded_vertex.position.xy, 1.5 + 0.5*sin(time))), 0.0);
#endif //EFFECT_TO_DO
}