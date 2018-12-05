//This file contains parameter values that have global scope. If values should remain in a certain range,
//hopefully a comment has been written advising on the range. Editing these values will change program behavior.
//
//Created by Forrest Miller on July 16, 2018

#pragma once

#ifndef PROJECT_PARAMETERS_H_
#define PROJECT_PARAMETERS_H_


//--------------------------------------------------------------------------
//    Change the following variables to modify program behavior. Note that this will most likely
//      necessitate a lengthy recompilation of the entire project. Eventually this file will be 
//      replaced by a way to load settings from a file (plus generate new settings if file is 
//      unlocateable).
//--------------------------------------------------------------------------



#define USE_DEBUG_ 
#define FORCE_DEBUG_CONTEXT_APP_SYNCHRONIZATION_  //Defining this will force synchnization between the graphics context and the app, which will affect performance but improve debugging 


static constexpr const int MONITOR_TO_USE = 0;  //Index of monitor in array of available monitors

static constexpr const bool USE_FULLSCREEN = false;

static constexpr const bool USE_VSYNC = true;

static constexpr int DEFAULT_AA_SAMPLES = 8;



//static constexpr int FONT_SIZE = 24;
//static constexpr float MARGIN = FONT_SIZE * 0.5f;



#endif //PROJECT_PARAMETERS_H_