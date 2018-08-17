//Designed to be quick to use (and quick and dirty to implement).
//This is a self-contained simple buffer complete with full OpenGL calls. 
//This object will be useful for quickly getting something working for testing.
//
//This object will create 1 buffer per object instance, which is an inefficient way to
//create multiple buffers. 
//
// See OpenGl SuperBible 7th edition around pages 100-101 
//
// Created August 6, 2018  by Forrest Miller
// Left stagnant and unfinished for several weeks due to other (work) commitments.
// Finished on (NEVER! UNTIL I QUIT MY JOB!)
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

		void createGLBuffer(); 
		void attachVertices(const std::vector<GLfloat> &);
		void loadBufferToGPU();
		void bindToContext() const;
		void cleanupSelf(); //for now have this be public...

		GLuint ID() const { return mBufferID;  }
		size_t vertDataSize() const { return mVertData.size(); }

		SimpleBuffer& operator=(const SimpleBuffer&) = delete;
		SimpleBuffer& operator=(SimpleBuffer&&);

	private:
		//Right now only a vertice buffer
		GLuint mBufferID;
		std::vector<GLfloat> mVertData;
		bool mDataWasLoadedToGpu;

		bool hasValidBufferID() const;
		bool hasVertData() const { return (vertDataSize() > 0u); }

		
	};
	
} //namespace ShaderInterface




#endif //SIMPLE_BUFFER_H_
