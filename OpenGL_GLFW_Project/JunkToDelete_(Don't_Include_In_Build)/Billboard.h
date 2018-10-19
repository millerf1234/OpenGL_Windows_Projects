//Represents a 2D billboard to have textures sampled onto
// Uses the Quad class
//File created by Forrest Miller on 7/20/2018

#pragma once

#ifndef BILLBOARD_H_
#define BILLBOARD_H_

#include "ProjectConstants.h"
#include "ProjectParameters.h"
#include "Quad.h"


class Billboard {
public:
	Billboard();
	~Billboard();

private:

	std::unique_ptr<Quad> quad;

};

#endif // BILLBOARD_H_
