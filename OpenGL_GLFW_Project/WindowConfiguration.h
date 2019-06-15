


#pragma once

#ifndef WINDOW_CONFIGURATION_H_
#define WINDOW_CONFIGURATION_H_


#include <array>


static constexpr const int MONITOR_TO_USE = 0;  //Index of monitor in array of available monitors

static constexpr const bool USE_FULLSCREEN = true;

static constexpr const bool USE_VSYNC = true;

static constexpr int DEFAULT_AA_SAMPLES = 0;


//Screen resolutions  (see: https://en.wikipedia.org/wiki/Graphics_display_resolution#QHD_(2560%C3%971440)  )
static constexpr const std::array<int, 2> SCREEN_RESOLUTION_HD = { 1280,  720 };  //  High Definition
static constexpr const std::array<int, 2> SCREEN_RESOLUTION_FHD = { 1920, 1080 };  // Full High Definition
static constexpr const std::array<int, 2> SCREEN_RESOLUTION_QHD = { 2560, 1440 };  // Quad High Definition
static constexpr const std::array<int, 2> SCREEN_RESOLUTION_UHD = { 3840, 2160 };  // Ultra High Definition












#endif // WINDOW_CONFIGURATION_H_