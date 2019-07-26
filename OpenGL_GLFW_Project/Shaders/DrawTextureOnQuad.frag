//Fragment shader for the asset loading demo
//
// Programmer:  Forrest Miller
// Date:        November 28, 2018

#version 450 core


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



void main() {

    //Declare some variables that depend on the customParameters
    const float custom1 = computeCustom1(customParameter1);
    
    const float custom2 = computeCustom2(customParameter2 % (15u+customParameter3), custom1);


    //Set-up a fake light source to compute diffuse lighting
    const vec3 lightPos = vec3(15. * sin(time), 17. * cos(time * 1.171), 5. * sin(time+cos(.25*time)));

    const vec3 lightColor = vec3( .795 + .381*cos(time - time*(17./37.)),
                                    .595 + .4*cos(time + (3.1415 / 1.5)),
                                    .667 + .3*sin(.781*time));

    const float diffuse = dot(normalize(processed_vertex.position.xyz),
                              normalize(lightPos));
    const float diffuseMag = .335;
    const float oneMinusDiffuseMag = 1. - diffuseMag;
    


    const vec4 computedDiffuse = vec4((diffuseMag * diffuse) * lightColor,      //R,G,B
                                       .75 - .01*processed_vertex.instanceID);  //A

    const vec4 computedTexture = oneMinusDiffuseMag * texture(tex_object,
                                                              processed_vertex.instanceID*processed_vertex.texCoord + vec2(.01*processed_vertex.instanceID*time));
        
        
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
    
    if (customParameter1 % 4u == 1u) {
        
        finalColor.r = finalColor.g/finalColor.b;
        finalColor.g = finalColor.b/finalColor.r;
        finalColor.b = smoothstep(min(finalColor.r / 2., finalColor.g / 2.),
                             max(finalColor.r + finalColor.g + .4, length(finalColor)),
                             finalColor.b);

    }
    else if (customParameter1 % 4u == 2u) {
        const float temp = finalColor.b;
        finalColor.b = finalColor.g;
        finalColor.g = finalColor.r;
        finalColor.r = clamp((temp + finalColor.b + finalColor.g) / 2.75,
                              0., 1.);
    }
    else if (customParameter1 % 4u == 3u) {
        finalColor.rgba = finalColor.brag;
    }
    
    if (customParameter2 % 2 == 1) {
        finalColor.r += cos(processed_vertex.position.y + processed_vertex.vertIDMod + time);
        finalColor.r = clamp(finalColor.r, -.1, 1.);
        finalColor.g *= 1.65*finalColor.b*cos(processed_vertex.vertID - time);
        finalColor.b *= 1.605*finalColor.g*sin(1.-time);
        finalColor.rgb = normalize(finalColor.rgb);
    }

    finalColor.a += .44*diffuse;

    finalColor.a = clamp(finalColor.a, .235, 2.);
    color = finalColor;

}