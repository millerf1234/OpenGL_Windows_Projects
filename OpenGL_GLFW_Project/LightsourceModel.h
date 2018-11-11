// File:  LightsourceModel.h
// Class: LightsourceModel
// 
// Description: This class is intended to be used with the class 'Lightsource.'
//              The purpose of this class is to provide an inclosed implementation
//              for generating geometry representing a generic light source (essentially
//              this is just a glowing polygon). The number of sides of the polygon can
//              be customized, as can the color of the light source.
//              
// Programmer:  Forrest Miller
// Date:        November 9, 2018

#pragma once

#ifndef LIGHTSOURCE_MODEL_H_
#define LIGHTSOURCE_MODEL_H_

#include <array>
#include <vector>

#include "ProjectSetup.h"


class LightsourceModel {
public:
	//Create's a default light source model
	LightsourceModel(unsigned int numSides = 8, unsigned int numLevels = 24);
	LightsourceModel(std::array<float, 3> color, unsigned int numSides = 8, unsigned int numLevels = 24);
	LightsourceModel(glm::vec3 color, unsigned int numSides = 8, unsigned int numLevels = 24);

	~LightsourceModel();//...

private:
	glm::vec3 mCenter_;
	glm::vec3 mColor_;
	
	std::vector<float> vertices;
};

#endif //LIGHTSOURCE_MODEL_H_