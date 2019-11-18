
#version 450 core

//Fragment shader for the asset loading demo
//
// Programmer:  Forrest Miller
// Date:        November 28, 2018



in VERTEX_SHADER_OUTPUT {  
    float vertID;
    float vertIDMod;
    float instanceID;
    vec4 position;
    vec2 texCoord;
    vec3 normal;
} processed_vertex;

out vec4 color;

uniform sampler2D tex_object;
uniform float time;
uniform mat4 rotation;

uniform uint customParameter1, customParameter2, customParameter3;
const uint lightingVersion = customParameter1 % 3U;

//*******************************************************
//External Noise Function Declarations
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
//******************************************************

float computeCustom1(in const uint seed) {
    float custom1 = clamp(tan(float(customParameter1)),
                    -150.,
                    150.);
    return custom1;
}

float computeCustom2(in const uint seed, in const float seed2) {
    float custom2;
    const float seedf = float(seed);
    const float trigInput = (time * (11. / 17.71)*seedf) + seed2 + processed_vertex.instanceID;
    custom2 = seed2 * sin(2.*cos(3.*tan(1.+8.*sin(cos(tan(1.+5.*sin(cos(tan(trigInput)))))))));
    return custom2;
}


//                          TWO WAVE GENERATOR FUNCTIONS
//Calculation is inspired by the Fourier expansion of
//Sawtooth waves. see https://en.wikipedia.org/wiki/Square_wave#Fourier_analysis
//Calling 'sinePartialSummation( 
float sinePartialSummation(float angularFreq, //angular frequency is (2 * PI * Frequency)  
                           int terms, //How many terms in series to compute. Must be 1 or greater 
                           int harmonicMultiple) {//Distance Between Frequency Multiples) {
    
    const float SUMMATION_FACTOR = 2. / 3.14159;
    const int termsToCompute = max(1, terms);
    const int harmonicIntervalSpacing = max(1, harmonicMultiple);
    float summation = sin(angularFreq * time);
    for (int i = 1; i <= termsToCompute; i++) {
        const float harmonic = 1. + float(i * harmonicIntervalSpacing);
        const float sign = pow(1., float(i * harmonicIntervalSpacing));
        summation += sign*(sin((harmonic*angularFreq)*time) / harmonic);
    }
    return  SUMMATION_FACTOR*summation;
}
/* DISABLED FOR NOW
float cosinePartialSummation(float x, int termsToCompute, float harmonicIntervalSpacing) {
    float summation = cos(x);
    for (int i = 1; i <= max(1, termsToCompute); i++) {
        summation += (1. / (2. * i + 1.)) * cos((1. + i * harmonicIntervalSpacing) * x);
    }
    return ((4. / 3.1416) * summation);
} */




//My own noise functions defined in 'PracticeNoise.glsl'
float fsmJankyPolarNoiseF_00(vec3 samplePoint,float freq,float jankyness); 
vec3 fsmJankyPolarNoiseV3_00(vec3 samplePoint,float freq,float jankyness); 





void main() {

//This first one is the best one so far for demonstrating a solid texture mapped model
#if 0

color = texture(tex_object, processed_vertex.texCoord) - vec4(.2, 0., 0., .5);

#elif 0

//These here are for playing around with the noise function I wrote in 
//the GLSL file "PracticeNoise.glsl"

color = mix(texture(tex_object, processed_vertex.texCoord),
            vec4(fsmJankyPolarNoiseV3_00(processed_vertex.position.xyz, 17.4, .77 + .31*cos(time)), .45),
            0.25 + .25*sin(time));

//const float noiseVal1 = fsmJankyPolarNoiseF_00(processed_vertex.position.xyz, 17.4, .13);
//color = vec4(noiseVal1, 1. - noiseVal1, .5 + .5*noiseVal1, .45);


#elif 0

//Hmm this was just an experiment but I can't see any effect...
vec2 dPdx = 5.*vec2(cos(time + float(customParameter3)), sin(time));
vec2 dPdy = -1. * dPdx;
color = textureGrad(tex_object, processed_vertex.texCoord, dPdx, dPdy);

#elif 0

const vec4 ambient = vec4(.0511, .0311, .0911, .107);


const vec4 textureColor = texture(tex_object, processed_vertex.texCoord);


vec3 lightDirection = normalize(vec3(10.*cos(0.35*time), 12. * -sin(time - 2.14), 2.0 * cos(time)));
float diffuseStrength = dot(lightDirection, normalize(processed_vertex.normal));
diffuseStrength = clamp(diffuseStrength, 0.0, 1.0);
vec4 diffuse = diffuseStrength * textureColor;


color = normalize(ambient + diffuse);


#else 

    //Declare some variables that depend on the customParameters
    const float custom1 = computeCustom1(customParameter1);
    
    const float custom2 = computeCustom2(customParameter2 % (15u+customParameter3), custom1);


    //Set-up a fake light source to compute diffuse lighting
    const vec3 lightPos = vec3(15. * sin(time), 17. * cos(time * 1.171), 5. * sin(time+cos(.25*time)));

    const vec3 lightColor = vec3( .5795 + .381*cos(time - time*(17./37.)),
                                  .3595 + .4*cos(time + (3.1415 / 1.5)),
                                  .4667 + .3*sin(.781*time));

    const float diffuse = dot(normalize(processed_vertex.position.xyz),
                              normalize(lightPos));
    const float diffuseMag = .4335;
    const float oneMinusDiffuseMag = 1. - diffuseMag;
    


    const vec4 computedDiffuse = vec4((diffuseMag * diffuse) * lightColor,      //R,G,B
                                       .75 - .01*processed_vertex.instanceID);  //A

    const vec4 computedTexture = oneMinusDiffuseMag * texture(tex_object,
                                                              processed_vertex.instanceID*processed_vertex.texCoord); //+ vec2(.01*processed_vertex.instanceID*time + sin(processed_vertex.vertID * 17. / 43. * 3.14)));
        
        
    vec4 computedColor = computedDiffuse + computedTexture;
    computedColor.a /= (.01*processed_vertex.vertID + 1.);
                               
    ///////////////////////////////////////////////////////////
    /////  Remap All Alpha Values That Are Close To '0.'  /////
    ///////////////////////////////////////////////////////////
    //We want to avoid having to deal with computedColor.a being close to '0.'
    //so we do this following trick to set it to a value other than '0.' if it
    //is '0.'
    const float minAlphaCutoff = .0025;
    const float zeroUnlessAlphaIsBelowCutoff = (1. - step(minAlphaCutoff, abs(computedColor.a))); 
                      
    const float newAlphaValue = (.55 + .25 * cos(time-length(processed_vertex.position)));                  
    computedColor.a += zeroUnlessAlphaIsBelowCutoff * newAlphaValue;

    ////////////////////////////////////////////////////////////////////
    /////  Remap All Alpha Values That Are Greater Than ALPHA_MAX  /////
    ////////////////////////////////////////////////////////////////////
    #define ALPHA_MAX 1.05
    //Here we want to see if the computed alpha value is greater than 1.
    //and if it is we do an additional computation to place it back below 1.0    
    const float alphaInverse = 1./(computedColor.a + (1. - ALPHA_MAX));
    
    //'step()' will return '1.' if 'a > .95' and otherwise will return '0.'
    const float alphaInverseMultiplier = step(ALPHA_MAX, computedColor.a);
    
    //'alphaMultiplier' will be '1.' whenever 'alphaInverseMultiplier' is '0.' 
    //and vice-versa
    const float alphaMultiplier = 1. - alphaInverseMultiplier;

    computedColor.a = (alphaMultiplier * computedColor.a) + 
                      (alphaInverseMultiplier * alphaInverse);

    //Finally we are ready to write the fragment color
    vec4 finalColor = clamp(computedColor,
                            vec4(0.), 
                            vec4(1.));

    //PSYCH! We are not quite yet ready
    
    if (customParameter1 % 5u == 1u) { 
        float instanceFactor = processed_vertex.instanceID + (abs(sin(time - log(min(3., processed_vertex.instanceID)))) / 2.);
        float vertexFactor = sinePartialSummation(min(log(17.3*processed_vertex.vertID), 2.66),
                                                  7, 1);
        float combinedFactor = (instanceFactor + vertexFactor);
        float trigNoise = abs(sin(time+cnoise(vec4(instanceFactor *(finalColor.g / finalColor.b),
                                                   vertexFactor * (finalColor.g / finalColor.r),
                                                   combinedFactor * (finalColor.b / finalColor.g),
                                                   snoise(time*vec3(instanceFactor+time,
                                                                    vertexFactor + time,
                                                                    combinedFactor * time)
                                                         )
                                                  )
                                             )
                                 )
                             );
        finalColor.r = (3. * finalColor.r / 7.);
        finalColor.r = smoothstep(0.2,
                                  finalColor.r + (1.-finalColor.r)*abs(cos(time + trigNoise)),
                                  abs(sinePartialSummation(.1019 + 0.*sqrt(abs(processed_vertex.vertID 
                                      - sqrt(time*snoise(vec2(cos(time), sin(time)))))), 9, 2)));
        finalColor.g = (6.0*finalColor.g*abs(sin(trigNoise)) / 7.0);
        finalColor.b = smoothstep(min(finalColor.r / 2., finalColor.g / 2.),
                             max(finalColor.r + finalColor.g + .4, length(finalColor)),
                             finalColor.b);
    }
    else if (customParameter1 % 5u == 2u) {
        const float temp = finalColor.b;
        finalColor.b = finalColor.g;
        finalColor.g = finalColor.r;
        finalColor.r = clamp((temp + finalColor.b + finalColor.g) / 2.75,
                              0., 1.);
    }
    else if (customParameter1 % 5u == 3u) {
        
        const float noiseInputAmplitude = 3.;
        const vec3 trigNoiseInput_Steady = noiseInputAmplitude * vec3( sinePartialSummation(8., 8, 3),
                                                                       sinePartialSummation(11.4, 8, 1),
                                                                       sinePartialSummation(27.1, 8, 4));
        const float rFactor = min(finalColor.r, 1. - finalColor.r);
        const float gFactor = min(finalColor.g, 1. - finalColor.g);
        const float bFactor = min(finalColor.b, 1. - finalColor.b);

        const float rNoise = rFactor * clamp(sinePartialSummation(snoise(trigNoiseInput_Steady),5, 7),
                                             0.0,
                                             1.0);
        const float alphaNoise = 0.65 + 0.25 * sin(cos(tan(snoise(trigNoiseInput_Steady))));
        finalColor.a = .5 * (finalColor.a + alphaNoise);
    }

    /*
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
    */


    else if (customParameter1 % 5u == 4u) {
        finalColor.rgb = mix(finalColor.rgb, vec3(0.5 + 0.5 * sin(snoise(vec3(time) + finalColor.rgb)),
                                                  0.6 - abs(0.4 * cos(snoise(time * finalColor))),
                                                  0.8 - abs(cos(sin(snoise(vec2(time, processed_vertex.vertIDMod)))))),
                             0.6 + .4*cos(time + 30.*snoise(gl_FragCoord.xy)));

    }

    
    if (customParameter2 % 2 == 1) {
        finalColor.r += cos(processed_vertex.position.y + processed_vertex.vertIDMod + time);
        finalColor.r = clamp(finalColor.r, -.1, 1.);
        finalColor.g *= 1.65*finalColor.b*cos(processed_vertex.vertID - time);
        finalColor.b *= 1.605*finalColor.g*sin(1.-time);
        finalColor.rgb = normalize(finalColor.rgb);
    }

    if (customParameter3 % 3u == 1u) {
        if (finalColor.g < finalColor.r) {
            finalColor.g *= 2.0;
            finalColor.r *= 0.65;
            finalColor.b *= 0.65;
        }
        else {
            finalColor.r *= 2.25;
            finalColor.g *= 0.65;
            finalColor.b *= 0.65;
        }
    }
    else if (customParameter3 % 3u == 2u) {
        finalColor.g *= (.8 + .15*sin(.1*time));
        finalColor.r *= max(finalColor.g, finalColor.b);
        finalColor.b *= 2.75;
    }

    if (length(finalColor.rb) < (.25 + .17*cos(time + processed_vertex.vertIDMod))) {
        finalColor.r *= 3.;
        finalColor.b *= 2.*(1. + abs(finalColor.r - finalColor.g));
    }
    finalColor.a *= .244*diffuse;

    finalColor.a = clamp(finalColor.a, .135, .92);
    finalColor.a += .000195*processed_vertex.vertID;
    color = finalColor;
    #endif 
}
