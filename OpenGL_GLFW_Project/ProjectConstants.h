//This file contains constant values that have global scope. It is recommended to edit these values with care.

//Created by Forrest Miller on July 16, 2018

#pragma once

#ifndef PROJECT_CONSTANTS
#define PROJECT_CONSTANTS

#include <array>
#include <string>
#include <stdexcept>  //For exceptions

#include "ProjectSetup.h"



///For logging 
#ifndef MSGLOG
#define MSGLOG stdout
#endif

#ifndef ERRLOG
#define ERRLOG stderr
#endif 

#ifndef WRNLOG
#define WRNLOG stderr
#endif 


static const char * NAME_OF_GAME = "OpenGL Practice Test\0";


//OpenGL Version:
static constexpr int DEFAULT_OPENGL_VERSION_MAJOR = 4;
static constexpr int DEFAULT_OPENGL_VERSION_MINOR = 5;

//Screen resolutions  (see: https://en.wikipedia.org/wiki/Graphics_display_resolution#QHD_(2560%C3%971440)  )
static const std::array<int, 2> SCREEN_RESOLUTION_HD =  { 1280,  720 };    //High Definition
static const std::array<int, 2> SCREEN_RESOLUTION_FHD = { 1920, 1080 };  // Full High Definition
static const std::array<int, 2> SCREEN_RESOLUTION_QHD = { 2560, 1440 };  // Quad High Definition
static const std::array<int, 2> SCREEN_RESOLUTION_UHD = { 3840, 2160 };  // Ultra High Definition







#endif //define PROJECT_CONSTANTS