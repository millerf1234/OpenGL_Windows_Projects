// File:       Lightsource.h
// Dinky implementation of a light, for use with frag shaders

#pragma once

#ifndef LIGHTSOURCE_H_
#define LIGHTSOURCE_H_


#include "ProjectSetup.h"

class Lightsource {
public:
	Lightsource();
	Lightsource(glm::vec3 Color, glm::vec3 Position);
	~Lightsource();

	Lightsource(const Lightsource& that) = default;
	Lightsource(Lightsource&& that) = default;

	Lightsource& operator=(const Lightsource& that) = default;
	Lightsource& operator=(Lightsource&& that) = default;

	void translateX(float x);
	void translateY(float y);
	void translateZ(float z);
	void setPosition(float x, float y, float z);
	void setPosition(glm::vec3);
	void rotateAboutZ(float theta);
	glm::vec3 getPosition() const;

	void setColor(glm::vec3 color);
	void setColor(float r, float g, float b);
	glm::vec3 getColor() const;
	

private:
	glm::vec3 mColor_;
	glm::vec3 mPosition_;

};




#endif //LIGHTSOURCE_H_