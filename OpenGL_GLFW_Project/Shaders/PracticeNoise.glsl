#version 460 core                                                              
#pragma debug(on)                                                              
//  FILE:                                                   PracticeNoise.glsl 
//                                                                             
//                    }~~~~~]     OVERVIEW     [~~~~~{                         
// As the filename probably already gave away, this shader file is my active   
// shader noise workspace in which I can study, experiment and learn all about 
// GPU-computed noise through the implementation, modification and composition 
// of noise functions. I even have set out to perhaps even create my own noise 
// function(s) using some ideas I have.                                        
//                                                                             
//                  }~~~~~]     FILE LAYOUT      [~~~~~{                       
//  Currently the noise functions in this file are organized into 2 groups:    
//                                                                             
//        1)   EXTERNALLY INSPIRED                                             
//                    There noise functions are based on an external source    
//                    (be it a tutorial, general discussion or sample          
//                    implementation of shader noise). Each function will      
//                    include documentation of its external influencer and I   
//                    try to make an effort to always provide credit where     
//                    credit is due.                                           
//        2)   INTERNALLY INSPIRED                                             
//                    These are functions that are based upon my own ideas     
//                    about possible ways to create noise. I do not expect at  
//                    this point for these to be as reliable or as visually    
//                    pleasing as the well-known noise functions.              
//                                                                             
//                                                                             
//              }~~~~~]     FUNCTION PROTOTYPES      [~~~~~{                   
//                                                                             
//     [1]  EXTERNALLY INSPIRED                                                
//                                                                            
      /*  As Soon as I get around to attempting a noise-tutorial or I start 
          digging into how the well-known noise-functions are implemented, the
          functions to call will go here                                     */
                                                                               
                                                                               
//     [2]  INTERNALLY INSPIRED                                                
//                                                                             
    float fsmJankyPolarNoiseF_00(vec3 samplePoint,float freq,float jankyness); 
    vec3 fsmJankyPolarNoiseV3_00(vec3 samplePoint,float freq,float jankyness); 






///////////////////////////////////////////////////////////////////////////////
///////////        SECTION 1 NOISE FUNCTION IMPLEMENTATIONS        ////////////
///////////////////////////////////////////////////////////////////////////////

//DISCLAIMER 
//This file contains my own practice noise functions. This file is highly 
//experimental at this point so please excuse the sloppiness.
//Chances are disabled code (both the in-source-commented-out and 
//and the excluded-by-preprocessor-directive variety) will require some
//fiddling to  and as such will probably be quite messy.


//            //////////////////////////////////////////////////////
//   ////////////   NOISE FUNCTIONS BASED UPON NOISE TUTORIALS   //////////////
//   ''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''

/////////////////////////////////////////////////////////////////////////////////
////////////  Part 1 -- Noise Fundamentals           
////////////               [Based off chapter 10 of Graphics Shaders 2e] 

//There are 3 types of noise:  (i) value, (ii) gradient, and (iii) value-gradient. 
//Perlin noise is a classical example based on pseudo-random number gradients. 

//            !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//            !!  WORK ON THIS IS INCOMPLETE!  !!
//   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

///////////////////////////////////////////////////////////////////////////////
///////////                      END SECTION 1                     ////////////
///////////////////////////////////////////////////////////////////////////////




//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~





///////////////////////////////////////////////////////////////////////////////
///////////        SECTION 2 NOISE FUNCTION IMPLEMENTATIONS        ////////////
///////////////////////////////////////////////////////////////////////////////

uniform float time;

//helper func   
float makeJanky(float theta, float jankyness) {
    
    const float jankynessResolution = 1. / jankyness;
    
    const float jankyTheta = trunc(theta * jankynessResolution);

    return jankyTheta / jankynessResolution;
}

float fsmJankyPolarNoiseF_00(vec3 samplePoint,float freq,float jankyness) {

    //This is just a very rough noise proof-of-concept for now..

    //CONSTANTS
    const float PI = 3.142; //The inner mathematician in me is crying out in despair 
    const float MIN_JANKYNESS = PI / 27. ;

    //VALIDATE PARAMETERS
    float actualJankyness = clamp(abs(jankyness), MIN_JANKYNESS, abs(jankyness));

    //COMPUTE NOISE
    //First thing we need to do is compute our theta
    float theta = length(samplePoint) + asinh(2.95+2.85*cos(time)); //This is the first thing that popped into my head
    
    //Then we must introduce noise by making theta janky
    theta = makeJanky(theta, actualJankyness);

    //This value is then fed into the following function:
    float r = sin(cos(tan(sin(cos(tan(sin(cos(tan(freq * theta)))))))));

    //We can convert this to xy-Cartesian coordinates by simply expressing 
    //X and Y in terms of their polar components
    float x = r * cos(theta);
    float y = r * sin(theta);

    vec2 xy = vec2(x, y);

    vec2 xyQuadrant1 = .5*xy + vec2(.5);

    const float noiseOut = length(xyQuadrant1);
    return noiseOut;
}

vec3 fsmJankyPolarNoiseV3_00(vec3 samplePoint, float freq, float jankyness) {

//#define DO_OWN_THING
#define CALL_FLOAT_NOISE_FUNCTION_3_TIMES

#ifdef CALL_FLOAT_NOISE_FUNCTION_3_TIMES
return vec3( fsmJankyPolarNoiseF_00(samplePoint.rrr, freq, jankyness),
             fsmJankyPolarNoiseF_00(samplePoint.ggg, freq, jankyness),
             fsmJankyPolarNoiseF_00(samplePoint.bbb, freq, jankyness));

#endif //CALL_FLOAT_NOISE_FUNCTION_3_TIMES


#ifdef DO_OWN_THING
    //CONSTANTS
    const float PI = 3.142; //The inner mathematician in me is crying out in despair 
    const float MIN_JANKYNESS = PI / 27. ;

    //VALIDATE PARAMETERS
    float actualJankyness = clamp(abs(jankyness), MIN_JANKYNESS, abs(jankyness));

    //COMPUTE NOISE
    //First thing we need to do is compute our theta
    float theta = time + ((PI / 2.) * cos(samplePoint.r) - sin(samplePoint.g + samplePoint.b));
    
    //Then we must introduce noise by making theta janky
    theta = makeJanky(theta, actualJankyness);

    //This value is then fed into the following function:
    float r = sin(cos(tan(sin(cos(tan(sin(cos(tan(freq * theta)))))))));

    //We can convert this to xy-Cartesian coordinates by simply expressing 
    //X and Y in terms of their polar components
    float x = r * cos(theta);
    float y = r * sin(theta);

    vec2 xy = vec2(x, y);

    vec2 xyQuadrant1 = .5*xy + vec2(.5);

    const float noiseOut = length(xyQuadrant1);
    return noiseOut;
    #endif //DO_OWN_THING
}




#pragma debug(off)