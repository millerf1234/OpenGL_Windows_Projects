


#pragma once

#ifndef WINDOW_CONFIGURATION_H_
#define WINDOW_CONFIGURATION_H_


//#include <array>
#include "SupportedWindowResolutions.h"

//I have only tested the code with a single monitor  
static constexpr const int MONITOR_TO_USE = 0;  //Index of monitor in array of available monitors


//Running in Fullscreen mode gives complete focus of the entire display to this
//Application while allowing greater control over the displays operation 
//characteristics (refresh rate, resolution, etc...). It should be noted that this  
//is different from running the Application in Windowed-Fullscreen mode. Windowed-Fullscreen
//mode simply has the Application run with whatever configuration the Operating System is
//using, which allows for quicker switching between this Application and other open windows.
static constexpr const bool USE_FULLSCREEN = false;


//Pretty much always want this to be true 
static constexpr const bool DOUBLE_BUFFERING = true;

//Should be a value between 0.0f and 1.0f, with 0.0f representing
//full transparency and 1.0f representing complete opaqueness. 
static constexpr const float INITIAL_WINDOW_OPACITY = 1.0f; 

//Probably want this on unless going for a benchmark/stress-test run
static constexpr const bool USE_VSYNC = true;

//Not Implemented as of Summer 2019, but if it were the acceptable values 
//would range from 0-8 (0-16?) (0-32?) 
static constexpr int DEFAULT_AA_SAMPLES = 4;






#endif // WINDOW_CONFIGURATION_H_