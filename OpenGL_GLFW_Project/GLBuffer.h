//Abstract Representation of a GLBuffer
//
//Created by Forrest Miller on July 25, 2018

#pragma once

#include "ProjectConstants.h"
#include "ProjectParameters.h"

//See: https://www.khronos.org/opengl/wiki/Buffer_Object#General_use
//VBO is Vertex Buffer Object
//EBO is Element Buffer Object
//UBO is Uniform Buffer Object
//TBO is Texture Buffer Object
//TFBO is Transformation Feedback Buffer Object
//ACBO is Atomic Counter Buffer Object (For atomic uniforms for shaders)  (requires OpenGL 4.2 or later)
//DIBO is Draw Indirect Buffer Object (for indirect drawing)
enum class BufferType {VBO, EBO, UBO, TBO, TFBO, ACBO, DIBO};
class GLBuffer {
public:
	GLBuffer() : mVAO(0u) { ; }
	GLBuffer(const GLuint VAO) : mVAO(VAO) { ; }
	~GLBuffer();


private:
	GLuint mID;
	GLuint mVAO; //If has an associated VAO, value will be VAO's value. 0 represents no VAO set.
};

