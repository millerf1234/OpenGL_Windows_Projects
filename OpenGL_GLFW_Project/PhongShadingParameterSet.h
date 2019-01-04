//File:                      PhongShadingParameterSet.h
//
//Description:               Class that stores the Phong illumination parameters to be used 
//                           when computing the Phong lighting/shading of a 3D mesh 
//
//Programmer:                Forrest Miller
//Date:                      November 14, 2018
//
//References:                  https://en.wikipedia.org/wiki/Phong_reflection_model
//                             https://en.wikipedia.org/wiki/Phong_shading
//
//Additions/related_topics:    https://developer.valvesoftware.com/wiki/Phong_materials
//
//
//Notes:                     -This will work for Blinn-Phong shading as well. The mesh must have normal 
//                                vectors available in the fragment/pixel shader for proper illumination.
//                           -'Shiny' Represents the size of the specular reflection, with smaller numbers 
//                                representing a larger specular angle (diameter?)
//                           -Based off an entire minute of in-depth internet research, it appears as though the
//                                default value various Phong shading implementations assign to Shiny is around 
//                                30.0 to 40.0  



#pragma once


#ifndef PHONG_ILLUMINATION_H_
#define PHONG_ILLUMINATION_H_

#include <array>
#include "GlobalIncludes.h"           //For glm
//#include "CachedUniformLocation.h"  //For Quickly loading data to GLSL

class PhongShadingParameterSet {
public:
	PhongShadingParameterSet() {
		kA = kD = kS = { 0.0f, 0.0f, 0.0f };
		shiny = 0.0f;
		opacity = 1.0f;
	}
	PhongShadingParameterSet(std::array<float, 3> kAmb, std::array<float, 3> kDiff, std::array<float, 3> kSpec, float shiny = 35.0f, float opacity = 1.0f) {
		kA = kAmb;
		kD = kDiff;
		kS = kSpec;
		this->shiny = shiny;
		this->opacity = glm::clamp(opacity, 0.0f, 1.0f);
	}

	/*void get(std::array<float, 11>& ) const {
		return 
	}*/

private:
	std::array<float, 3> kA;
	std::array<float, 3> kD;
	std::array<float, 3> kS;
	float shiny;
	float opacity; //Opposite of transparancy
	
};




#endif //PHONG_ILLUMINATION_H_