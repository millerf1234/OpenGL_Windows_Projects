//This file contains constant values that have global scope. It is recommended to edit these values with care.

//Created by Forrest Miller on July 16, 2018

#pragma once

#ifndef PROJECT_CONSTANTS_H_
#define PROJECT_CONSTANTS_H_

#include <array>
#include <string>
#include <stdexcept>  //For exceptions

#include "ProjectSetup.h"


static constexpr const char * NAME_OF_GAME = "OpenGL Render Demo";

//Used in shader code for shader inputs
//Actually stuff like this is what happens when I only get several hours a day to code, I wind up with 
//stuff I forget I am supposed to use. This enum is not used anywhere in this project...
enum class VertexAttribPointerType {
	BYTE, UNSIGNED_BYTE, SHORT, UNSIGNED_SHORT, INT, UNSIGNED_INT, HALF_FLOAT,
	FLOAT, FIXED, INT_2_10_10_10_REV, UNSIGNED_INT_2_10_10_10_REV, UNSIGNED_INT_10f_11f_11f_REV,
};




//Useful Constants
constexpr size_t INDEX_SHIFT = 1ull; //Useful for array indexing
constexpr GLfloat PI = 3.14159265f;



//Limits on input rate [measured in frames]
static constexpr unsigned long long DELAY_LENGTH_OF_PAUSE_CHECKING_AFTER_UNPAUSE = 8ull;
static constexpr unsigned long long DELAY_BETWEEN_SCREEN_COLOR_RECORDINGS_IN_RENDER_PROJECTS = 15ull;


//OpenGL Version:
static constexpr int DEFAULT_OPENGL_VERSION_MAJOR = 4;
static constexpr int DEFAULT_OPENGL_VERSION_MINOR = 5;

//Screen resolutions  (see: https://en.wikipedia.org/wiki/Graphics_display_resolution#QHD_(2560%C3%971440)  )
static const std::array<int, 2> SCREEN_RESOLUTION_HD =  { 1280,  720 };    //High Definition
static const std::array<int, 2> SCREEN_RESOLUTION_FHD = { 1920, 1080 };  // Full High Definition
static const std::array<int, 2> SCREEN_RESOLUTION_QHD = { 2560, 1440 };  // Quad High Definition
static const std::array<int, 2> SCREEN_RESOLUTION_UHD = { 3840, 2160 };  // Ultra High Definition







#endif //define PROJECT_CONSTANTS_H_