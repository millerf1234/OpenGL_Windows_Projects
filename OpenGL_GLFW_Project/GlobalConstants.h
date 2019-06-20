//This file contains constant values that have global scope. 
//There is currently a pretty random spread of constant values defined here,
//eventually it may be wise to organize constants like this into seperate 
//categories.

//Created by Forrest Miller on July 16, 2018
//Modified continuously throughout the rest of 2018 and into 2019

#pragma once

#ifndef GLOBAL_CONSTANTS_H_
#define GLOBAL_CONSTANTS_H_

#include <array>
#include <string>
#include <stdexcept>  //For exceptions

#include "GlobalIncludes.h"


//Useful Constants
static constexpr size_t INDEX_SHIFT = 1u; //Useful for array indexing
static constexpr GLfloat PI = 3.14159f; /* = 3.14159265f; //Too Many */

static constexpr const double MILLIMETERS_PER_INCH = 25.4; //Used in screen DPI computations 
static constexpr const double INCHES_PER_MILLIMETER = 0.0393701;

//Limits on input rate [measured in frames]
static constexpr unsigned long long DELAY_LENGTH_OF_PAUSE_CHECKING_AFTER_UNPAUSE = 8ull;
static constexpr unsigned long long DELAY_BETWEEN_SCREEN_COLOR_RECORDINGS_IN_RENDER_PROJECTS = 15ull;  //Not globally used in all RenderProjects


//Initial 





#endif //GLOBAL_CONSTANTS_H_