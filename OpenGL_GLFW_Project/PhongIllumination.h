//File:                      PhongIllumination.h
//
//Description:               Header-only struct that defines the Phong Illumination data for a model.
//
//Programmer:                Forrest Miller
//Date:                      November 14, 2018
//
//References:                 https://en.wikipedia.org/wiki/Phong_reflection_model
//                           https://en.wikipedia.org/wiki/Phong_shading
//
//Additions/related_topics:    https://developer.valvesoftware.com/wiki/Phong_materials
//
//
//Notes:                     This will work for Blinn-Phong illumination as well. Model must have normal 
//                           vectors available in fragment shader for proper illumination.


#pragma once


#ifndef PHONG_ILLUMINATION_H_
#define PHONG_ILLUMINATION_H_

#include <array>



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




#endif //PHONG_ILLUMINATION_H_