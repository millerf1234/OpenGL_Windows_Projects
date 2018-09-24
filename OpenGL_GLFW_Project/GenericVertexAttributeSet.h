// This is just a simple ShaderInput class that is able to take 
// data from the application with a layout location for the vertex
// input and link them together. Multiple locations can be accepted 
// by this object. This object also handles the VAO. 

#pragma once

#ifndef GENERIC_VERTEX_ATTRIBUTE_SET_H_
#define GENERIC_VERTEX_ATTRIBUTE_SET_H_

#include "ProjectConstants.h"
#include "ProjectParameters.h"

#include <vector>


class GenericVertexAttributeSet {
public:
	GenericVertexAttributeSet(int layoutLocations = 1);
	~GenericVertexAttributeSet();

	//No Copying allowed
	GenericVertexAttributeSet(const GenericVertexAttributeSet&) = delete;
	GenericVertexAttributeSet& operator=(const GenericVertexAttributeSet&) = delete;

	void sendDataToVertexBuffer(int binding, const std::vector<GLfloat> &data, int vertexSize,
								 GLsizei vertexStride);
	
	void use() { glBindVertexArray(mVAO); }

private:

	GLuint mVAO;
	int mActiveLocations;

	typedef struct VertexBuffer {
		GLuint id;
		VertexBuffer(GLuint vboID = 0) { id = vboID; }
	} VertexBuffer;

	std::vector<VertexBuffer> mVertexBuffers;

	
	//Helper Functions
	void createVertexBuffers(int buffersToCreate);
	void createVAO();
};



#endif //GENERIC_VERTEX_ATTRIBUTE_SET_H_