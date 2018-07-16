//This file contains headers that are meant to be included globally only once. 

//Created by Forrest Miller on July 16, 2018

#pragma once

#ifndef PROJECT_SETUP
#define PROJECT_SETUP

//Glad version link: http://glad.dav1d.de/#profile=core&specification=gl&api=gl%3D4.5&api=gles1%3Dnone&api=gles2%3Dnone&api=glsc2%3Dnone&language=c&loader=on
#include "glad.h"



#include "glfw_config.h"  //Not sure if this is explicitly necessary
#include "glfw3.h"


//Use the gl math library
#include "glm/glm/glm.hpp"  //Well... It works so I am not going to fix it  //see: https://stackoverflow.com/questions/17905794/how-to-setup-the-glm-library-in-visual-studio-2012







#endif //define PROJECT_SETUP



