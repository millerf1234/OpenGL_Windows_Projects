//File:                       DefaultPhongIllumination.h
//
// Idea:                      I am not sure about it yet but I think I am going to add a bunch of these 'default_xyz.h' 
//                            files to give my objects something to initialize themselves to...
//
//Description:                Defines a default Phong Illumination object.
//
//Date:                       November 14, 2018


#pragma once


#ifndef DEFAULT_PHONG_ILLUMINATION_H_
#define DEFAULT_PHONG_ILLUMINATION_H_

#include "PhongShadingParameterSet.h"


inline static constexpr const float PHONG_DEFAULT_AMBIENT_R = 0.25f;
inline static constexpr const float PHONG_DEFAULT_AMBIENT_G = 0.25f;
inline static constexpr const float PHONG_DEFAULT_AMBIENT_B = 0.25f;

inline static constexpr const float PHONG_DEFAULT_DIFFUSE_R = 0.85f;
inline static constexpr const float PHONG_DEFAULT_DIFFUSE_G = 0.89f;
inline static constexpr const float PHONG_DEFAULT_DIFFUSE_B = 0.92f;

inline static constexpr const float PHONG_DEFAULT_SPECULAR_R = 0.00f;
inline static constexpr const float PHONG_DEFAULT_SPECULAR_G = 0.00f;
inline static constexpr const float PHONG_DEFAULT_SPECULAR_B = 0.00f;

inline static constexpr const float PHONG_DEFAULT_SHINY = 0.00f;

inline static constexpr const std::array<float, 3> PHONG_DEFAULT_AMBIENT = { PHONG_DEFAULT_AMBIENT_R,  PHONG_DEFAULT_AMBIENT_G,  PHONG_DEFAULT_AMBIENT_B };
inline static constexpr const std::array<float, 3> PHONG_DEFAULT_DIFFUSE = { PHONG_DEFAULT_DIFFUSE_R,  PHONG_DEFAULT_DIFFUSE_G,  PHONG_DEFAULT_DIFFUSE_B };
inline static constexpr const std::array<float, 3> PHONG_DEFAULT_SPECULAR = { PHONG_DEFAULT_SPECULAR_R, PHONG_DEFAULT_SPECULAR_G, PHONG_DEFAULT_SPECULAR_B };





static const PhongShadingParameterSet DEFAULT_PHONG_ILLUMINATION(PHONG_DEFAULT_AMBIENT, PHONG_DEFAULT_DIFFUSE, PHONG_DEFAULT_SPECULAR, PHONG_DEFAULT_SHINY);


#endif //DEFAULT_PHONG_ILLUMINATION_H_