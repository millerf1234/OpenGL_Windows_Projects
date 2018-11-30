//File:                FloatingPointTolerance.h
//
//Description:         Defines the floating point tolerance to use. The main intention of this value is to provide a 
//                     minimum magnitude for floating point calculations, with all values less than this being treated
//                     as 0. 
//                     By defining this value in a seperate header, it is possible to globally tweak program behavior 
//                     without having to hunt down every single file that requires floating-point tolerance safety-checks.
//
//Programme:           Forrest Miller

#pragma once

#ifndef FLOATING_POINT_TOLERANCE_H_
#define FLOATING_POINT_TOLERANCE_H_





static constexpr const float FP_TOLERANCE = 0.0000001f;




#endif //FLOATING_POINT_TOLERANCE_H_