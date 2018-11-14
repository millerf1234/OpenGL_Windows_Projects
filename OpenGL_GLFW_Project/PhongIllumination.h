//File:                      PhongIllumination.h
//
//Description:               Header-only struct that defines the Phong Illumination data for a model.
//
//Programmer:                Forrest Miller
//Date:                      November 14, 2018
//
//Reference:                 https://en.wikipedia.org/wiki/Phong_reflection_model
//                           https://en.wikipedia.org/wiki/Phong_shading
//
//Notes:                     This will work for Blinn-Phong illumination as well. Model must have normal 
//                           vectors available in fragment shader for proper illumination.


#pragma once


#ifndef PHONG_ILLUMINATION_H_
#define PHONG_ILLUMINATION_H_

#include <array>


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

inline static constexpr const std::array<float, 3> PHONG_DEFAULT_AMBIENT  = { PHONG_DEFAULT_AMBIENT_R,  PHONG_DEFAULT_AMBIENT_G,  PHONG_DEFAULT_AMBIENT_B };
inline static constexpr const std::array<float, 3> PHONG_DEFAULT_DIFFUSE  = { PHONG_DEFAULT_DIFFUSE_R,  PHONG_DEFAULT_DIFFUSE_G,  PHONG_DEFAULT_DIFFUSE_B };
inline static constexpr const std::array<float, 3> PHONG_DEFAULT_SPECULAR = { PHONG_DEFAULT_SPECULAR_R, PHONG_DEFAULT_SPECULAR_G, PHONG_DEFAULT_SPECULAR_B };



typedef struct PhongIlluminationValues {
	std::array<float, 3> kA;
	std::array<float, 3> kD;
	std::array<float, 3> kS;
	float shiny;
	PhongIlluminationValues() { kA = kD = kS = { 0.0f, 0.0f, 0.0f }; shiny = 0.0f; }
	PhongIlluminationValues(std::array<float, 3> kAmb, std::array<float, 3> kDiff, std::array<float, 3> kSpec, float shiny = 30.0f) {
		kA = kAmb;
		kD = kDiff;
		kS = kSpec;
		this->shiny = shiny;
	}
} PhongIlluminationWeights;


static const PhongIlluminationWeights DEFAULT_PHONG_WEIGHTS(PHONG_DEFAULT_AMBIENT, PHONG_DEFAULT_DIFFUSE, PHONG_DEFAULT_SPECULAR, PHONG_DEFAULT_SHINY);


#endif //PHONG_ILLUMINATION_H_