//Class representing a rectangle, intended to be used for Glyphs and Billboards (and eventually full-screen post-process effects). 
//This basically just represents a surface that textures can be painted on to.
//
//Created by Forrest Miller on 7/20/2018

#pragma once

#ifndef QUAD_H
#define QUAD_H

#include <vector>
#include <array>

#include "ProjectConstants.h"
#include "ProjectParameters.h"


class Quad {
public:
	Quad();
	~Quad();

	glm::vec2 getPosition() const { return mPosition; }

private:
	std::array<float, 16> mVertices; 
	//std::vector<float> mVertices;

	float mZoom;
	glm::vec2 mPosition;
	glm::vec2 mExtent;

	void initialize();
	void recomputeVertices();

};





#endif //#ifndef QUAH_H