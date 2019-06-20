
//Todo
//Write more of a header here in the future...

//Description: Defines a number of screen resolutions as constexpr global
//             which can be selected from amongst whilest operating this 
//             Application in windowed mode. And actually I just checked 
//             my code and it looks like these could be used for fullscreen
//             mode as well, but currently the implementation for opening
//             in fullscreen mode querries the display to get its prefered
//             native resolution and operates with it.
//                                                            
//                                     
//                                                                       
//             

#pragma once

#ifndef SUPPORTED_WINDOW_RESOLUTIONS_YO_H_
#define SUPPORTED_WINDOW_RESOLUTIONS_YO_H_



//Screen resolutions  (see: https://en.wikipedia.org/wiki/Graphics_display_resolution#QHD_(2560%C3%971440)  )
static constexpr const std::array<int, 2> SCREEN_RESOLUTION_HD = { 1280,  720 };  //  High Definition
static constexpr const std::array<int, 2> SCREEN_RESOLUTION_FHD = { 1920, 1080 };  // Full High Definition
static constexpr const std::array<int, 2> SCREEN_RESOLUTION_QHD = { 2560, 1440 };  // Quad High Definition
static constexpr const std::array<int, 2> SCREEN_RESOLUTION_UHD = { 3840, 2160 };  // Ultra High Definition




#endif //SUPPORTED_WINDOW_RESOLUTIONS_YO_H_