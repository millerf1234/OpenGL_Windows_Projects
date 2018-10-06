//This file contains parameter values that have global scope. If values should remain in a certain range,
//hopefully a comment has been written advising on the range. Editing these values will change program behavior.
//
//Created by Forrest Miller on July 16, 2018

#pragma once

#ifndef PROJECT_PARAMETERS_H_
#define PROJECT_PARAMETERS_H_

#include "ProjectSetup.h"

#define PRINT_SHADER_COMPILE_MESSAGES  //Used within the CompiledShader classes

#define BUILD_WITH_APPLICATION_DEBUGGING


static constexpr int MONITOR_TO_USE = 0;  //Index of monitor in array of available monitors


static constexpr bool USE_FULLSCREEN = true;

static constexpr bool USE_VSYNC = true;

static constexpr int DEFAULT_AA_SAMPLES = 4;



//static constexpr int FONT_SIZE = 24;
//static constexpr float MARGIN = FONT_SIZE * 0.5f;




//Mostly constant parameters (these parameters can be tweaked but usually doing so will have a negative effect)
static constexpr unsigned long long DELAY_LENGTH_OF_PAUSE_CHECKING_AFTER_UNPAUSE = 8ull;
static constexpr unsigned long long DELAY_BETWEEN_SCREEN_COLOR_RECORDINGS_IN_RENDER_PROJECT_1 = 15ull;


#endif //define PROJECT_PARAMETERS_H_