//This is a simple VertexAttributeSet class for handling VAO configuration for vertex data format
//for OpenGL. This class's main limitation is that it does not support interleaved vertex data.
//Since currently all of the model loading classes put their vertex data into an interleaved format,
//this class doesn't have much use outside of for the teapot explosion demo.

// This is just a simple ShaderInput class that is able to take 
// data from the application with a layout location for the vertex
// input and link them together. Multiple locations can be accepted 
// by this object. This object also handles the VAO. 
//
//  Code by Forrest Miller
//  Date:  September 24, 2018

#pragma once

#ifndef TEAPOT_EXPLOSION_DEMO_GENERIC_VERTEX_ATTRIBUTE_SET_H_
#define TEAPOT_EXPLOSION_DEMO_GENERIC_VERTEX_ATTRIBUTE_SET_H_

#include <vector>

#include "GlobalConstants.h"

namespace ShaderInterface {

	class TeapotExplosionDemo_GenericVertexAttributeSet {
	public:
		TeapotExplosionDemo_GenericVertexAttributeSet(int layoutLocations = 1);
		~TeapotExplosionDemo_GenericVertexAttributeSet();

		//No Copying allowed
		TeapotExplosionDemo_GenericVertexAttributeSet(const TeapotExplosionDemo_GenericVertexAttributeSet&) = delete;
		TeapotExplosionDemo_GenericVertexAttributeSet& operator=(const TeapotExplosionDemo_GenericVertexAttributeSet&) = delete;

		//Call this function to send data to a vertex buffer. This function is optimizied for 
		//static rendering, which means the data sent to the vertex buffer is not intended to be 
		//updated frequently. 
		//The parameters for this function are:
		//  (1)  int  bindingPoint    this is the vertex buffer binding point, which corresponds to the number 
		//							  set within the shader code for vertex input. (i.e. this number should match the x
		//                            within the destination given with the syntax "location ( layout = x )" within the vert shader.
		//  (2)  vector<GLfloat> data   This is a vector (currently of only GLfloat) which contains the input data. Please don't provide empty
		//								data (doing so results in an error being logged and this function returning).
		//  (3)  int   vertexSize     The number of data points contained within one vertex. For example, a vec3 will consist of 3 floats per
		//						      instance.
		//  (4)  GLsizei  vertexStride  This is the distance between successive vertices... This might not make sense actually and might not be needed...
		//                              Chances are the input will be tightly packed, which means this parameter should be left at 0.
		//  (5)  GLvoid&  offset      Distance into vertex where data starts
		void sendDataToVertexBuffer(int binding, const std::vector<GLfloat> &data, int vertexSize,
			GLsizei vertexStride, GLvoid* offset = 0);

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

} //namespace ShaderInterface

#endif //TEAPOT_EXPLOSION_DEMO_GENERIC_VERTEX_ATTRIBUTE_SET_H_