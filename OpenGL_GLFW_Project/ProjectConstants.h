//This file contains constant values that have global scope. 
//There is currently a pretty random spread of constant values defined here,
//eventually it may be wise to organize constants like this into seperate 
//categories.

//Created by Forrest Miller on July 16, 2018
//Modified continuously throughout the rest of 2018

#pragma once

#ifndef PROJECT_CONSTANTS_H_
#define PROJECT_CONSTANTS_H_

#include <array>
#include <string>
#include <stdexcept>  //For exceptions

#include "ProjectSetup.h"


//Useful Constants
static constexpr size_t INDEX_SHIFT = 1u; //Useful for array indexing
static constexpr GLfloat PI = 3.14159265f;

static constexpr const double MILLIMETERS_PER_INCH = 25.4; //Used in screen DPI computations 
static constexpr const double INCHES_PER_MILLIMETER = 0.0393701;

//Limits on input rate [measured in frames]
static constexpr unsigned long long DELAY_LENGTH_OF_PAUSE_CHECKING_AFTER_UNPAUSE = 8ull;
static constexpr unsigned long long DELAY_BETWEEN_SCREEN_COLOR_RECORDINGS_IN_RENDER_PROJECTS = 15ull;  //Not globally used in all RenderProjects


//Screen resolutions  (see: https://en.wikipedia.org/wiki/Graphics_display_resolution#QHD_(2560%C3%971440)  )
static const std::array<int, 2> SCREEN_RESOLUTION_HD =  { 1280,  720 };  //  High Definition
static const std::array<int, 2> SCREEN_RESOLUTION_FHD = { 1920, 1080 };  // Full High Definition
static const std::array<int, 2> SCREEN_RESOLUTION_QHD = { 2560, 1440 };  // Quad High Definition
static const std::array<int, 2> SCREEN_RESOLUTION_UHD = { 3840, 2160 };  // Ultra High Definition







#endif //PROJECT_CONSTANTS_H_