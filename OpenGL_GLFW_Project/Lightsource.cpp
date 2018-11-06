#include "Lightsource.h"


Lightsource::Lightsource() {
	mColor_ = glm::vec3(253.0f / 255.0f, 184.0f / 255.0f, 17.0f / 255.0f);
	mPosition_ = glm::vec3(3.0f, 15.0f, -0.5f); //Make it overhead somewhere...
}

Lightsource::Lightsource(glm::vec3 color, glm::vec3 position) : mColor_(color), mPosition_(position) { ; }

Lightsource::~Lightsource() {

}


void Lightsource::translateX(float x) {
	mPosition_.x += x;
}

void Lightsource::translateY(float y) {
	mPosition_.y += y;
}

void Lightsource::translateZ(float z) {
	mPosition_.z += z;
}

void Lightsource::setPosition(float x, float y, float z) {
	mPosition_ = glm::vec3(x, y, z);
}

void Lightsource::setPosition(glm::vec3 pos) {
	mPosition_ = pos;
}

void Lightsource::rotateAboutZ(float theta) {

	glm::mat3 zRotationMatrix = glm::mat3(cos(theta), -sin(theta),  0.0f,
									      sin(theta), cos(theta) ,  0.0f,
											  0.0f  ,    0.0f    ,  1.0f);
	mPosition_ = (zRotationMatrix * mPosition_);
}

glm::vec3 Lightsource::getPosition() const {
	return mPosition_;
}



void Lightsource::setColor(glm::vec3 color) {
	mColor_ = glm::clamp(color, 0.0f, 1.0f);
}

void Lightsource::setColor(float r, float g, float b) {
	glm::vec3 newColor(r, g, b);
	mColor_ = glm::clamp(newColor, 0.0f, 1.0f);
}

glm::vec3 Lightsource::getColor() const {
	return mColor_;
}