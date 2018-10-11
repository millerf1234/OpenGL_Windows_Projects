//Right now this only includes a single function. See the .cpp file for implementation.
//
//Note that the file_exists() function may use a non-standard non-portable implementation, 
//but several implementations for it are given so it shouldn't be an issue to switch implementations
//if need be.

#pragma once

#include <string>

//Verify if a filepath actually corresponds to a valid file
bool file_exists(const std::string& fn);




