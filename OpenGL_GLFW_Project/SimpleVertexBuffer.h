//Designed to be quick to use (and quick and dirty to implement).
//This is a self-contained simple vertex buffer with very basic functionality.
//This class will be useful for quickly getting something working for testing.
//Eventaully as more robust buffers come into existance, this class's use will
//begin to fade out. 
//
//This class currently does not work by mapping the buffer, and as such is not 
//the preferred implementation.
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
// Got a rough version sorta almost working by August 24, 2018, but the 
// overall design is clunky and instead of testing I decided to rewrite and refactor 
// large chunks of this class. 
//
// August 29th today and I might have a working version by the end of it. Let us hope...
//

#pragma once
#ifndef SIMPLE_BUFFER_H_
#define SIMPLE_BUFFER_H_

#include <vector>


#include "ProjectConstants.h"
#include "ProjectParameters.h"


namespace ShaderInterface {

	enum class VertLayout { VERT2, VERT3};

	class SimpleVertexBuffer {
	public:
		SimpleVertexBuffer();
		SimpleVertexBuffer(GLsizeiptr size);
		~SimpleVertexBuffer();

		SimpleVertexBuffer(const SimpleVertexBuffer&) = delete;
		SimpleVertexBuffer(SimpleVertexBuffer&&);

		//Public Interface
		void create(); 
		void attach(const std::vector<GLfloat> &); 
		void setLayout(VertLayout layout);
		void loadBufferToGPU(); //bufferSubData
		void bindToContext() const;

		//Clears the buffer to 0 or a specified value
		void clear(const GLfloat value = 0.0f);
		void cleanupSelf(); //for now have this be public...

		GLuint VAO() const { return mVAO; }
		GLuint ID() const { return mBufferID;  }
		bool hasValidBufferID() const;
		bool hasValidVAO() const;
		size_t vertDataSize() const { return mVertData.size(); } 

		SimpleVertexBuffer& operator=(const SimpleVertexBuffer&) = delete;
		SimpleVertexBuffer& operator=(SimpleVertexBuffer&&);

	private:
		GLuint mVAO;
		GLuint mBufferID;
		GLsizeiptr mBufferSize; 
		std::vector<GLfloat> mVertData;
		bool mDataWasLoadedToGpu;


		void initialize();
		bool hasVertData() const { return (vertDataSize() > 0u); }
		
		
	};
	
} //namespace ShaderInterface




#endif //SIMPLE_BUFFER_H_
