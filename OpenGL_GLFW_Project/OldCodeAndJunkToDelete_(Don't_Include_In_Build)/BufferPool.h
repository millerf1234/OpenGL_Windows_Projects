//File created on September 7, 2018  by Forrest Miller

#pragma once

#ifndef BUFFER_POOL_H_
#define BUFFER_POOL_H_

#include <array>

#include "ProjectConstants.h"
#include "ProjectParameters.h"
#include "MappableBuffer.h"




//Right now this part will only work on single-GPU setups 

//See https://stackoverflow.com/a/5695427
static std::array<int, 4> memoryAvailable = { 0, 0, 0, 0 };




#endif //BUFFER_POOL_H_