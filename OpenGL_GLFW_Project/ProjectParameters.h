//This file contains parameter values that have global scope. If values should remain in a certain range,
//hopefully a comment has been written advising on the range. Editing these values will change program behavior.
//
//Created by Forrest Miller on July 16, 2018

#pragma once

#ifndef PROJECT_PARAMETERS_H_
#define PROJECT_PARAMETERS_H_

#include "ProjectSetup.h"

#define PRINT_SHADER_COMPILE_MESSAGES  //Used within the CompiledShader classes


static constexpr int MONITOR_TO_USE = 0;  //Index of monitor in array of available monitors


static constexpr bool USE_FULLSCREEN = false;

static constexpr bool USE_VSYNC = true;

static constexpr int DEFAULT_AA_SAMPLES = 4;



static constexpr int FONT_SIZE = 24;
static constexpr float MARGIN = FONT_SIZE * 0.5f;




#endif //define PROJECT_PARAMETERS_H_