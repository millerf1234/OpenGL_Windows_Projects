//Implementation for the MappableBuffer class
//
//Created by Forrest Miller on  September 7, 2018

#include "MappableBuffer.h"

namespace ShaderInterface {

	void MappableBuffer::initialize() {
		mVertexLayout = nullptr;
		mID = 0u;
		mError = false;
		mState = BufferState::NO_BUFFER_ID;
	}

	MappableBuffer::MappableBuffer() {

	}



	MappableBuffer::MappableBuffer(GLuint ID) {

	}



	MappableBuffer::MappableBuffer(GLuint ID, bool persistant) {


	}


	/*MappableBuffer::MappableBuffer(GLuint ID, bool coherent) {

	}*/
		

} //namespace ShaderInterface

