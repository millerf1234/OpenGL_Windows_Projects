//Designed to be quick to use (and quick and dirty to implement).
//This is a self-contained simple buffer complete with full OpenGL calls. 
//This object will be useful for quickly getting something working for testing.
//
// Currently the buffer will always allocate 1 MiB of memory for itself, and will
// only allow data to be set at offset 0 into the buffer. 
//
//This object will create 1 buffer per object instance, which is an inefficient way to
//create multiple buffers. 
//
// See OpenGl SuperBible 7th edition around pages 100-101 
//
// Implemented using glNamedBufferSubData and 
// Created August 6, 2018  by Forrest Miller
// Left stagnant and unfinished for several weeks due to other (work) commitments.
//
// Got a rought version sorta almost working by August 24, 2018, but the 
// overall design is clunky and instead of testing I decided to rewrite and refactor 
// large chunks of this class. 
//

#pragma once
#ifndef SIMPLE_BUFFER_H_
#define SIMPLE_BUFFER_H_

#include <vector>

#include "ProjectConstants.h"
#include "ProjectParameters.h"


namespace ShaderInterface {

	class SimpleBuffer {
	public:
		SimpleBuffer();
		~SimpleBuffer();

		SimpleBuffer(const SimpleBuffer&) = delete;
		SimpleBuffer(SimpleBuffer&&);


		//Public Interface
		void create(); //void createGLBuffer();
		void attach(const std::vector<GLfloat> &); //void attachVertices(const std::vector<GLfloat> &);
		void loadBufferToGPU(); //
		void bindToContext() const;

		//Clears the buffer to 0 or a specified value
		void clear(const GLfloat value = 0.0f);
		void cleanupSelf(); //for now have this be public...

		GLuint VAO() const { return mVAO; }
		GLuint ID() const { return mBufferID;  }
		size_t vertDataSize() const { return mVertData.size(); } 

		SimpleBuffer& operator=(const SimpleBuffer&) = delete;
		SimpleBuffer& operator=(SimpleBuffer&&);

	private:
		GLuint mVAO;
		GLuint mBufferID;
		GLsizeiptr mBufferSize; 
		std::vector<GLfloat> mVertData;
		bool mDataWasLoadedToGpu;

		bool hasValidBufferID() const;
		bool hasVertData() const { return (vertDataSize() > 0u); }

		
	};
	
} //namespace ShaderInterface




#endif //SIMPLE_BUFFER_H_
