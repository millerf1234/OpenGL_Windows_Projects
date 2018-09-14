//After multiple failed attempts to design and code a robust buffer object 
//implementation, I believe I have now worked out most of the kinks. Due to
//the nature of modern OpenGL, I have decided to limit my buffer storage flags
//to mappable-only buffers (read, write, and read + write). My current intention
//is for there to be a class that handles creating these objects within a pool.
//
//Created by Forrest Miller on 9/7/2018
//
//

#ifndef MAPPABLE_BUFFER_H_
#define MAPPABLE_BUFFER_H_

#pragma once

//#include <vector>

#include "VertexArrayObject.h"

#include "ProjectConstants.h"
#include "ProjectParameters.h"




namespace ShaderInterface {

	enum class BufferState { NO_BUFFER_ID,
							 AQUIRING_BUFFER_STORAGE_FLAGS,
							 REQUIRES_VERTEX_LAYOUT_SPECIFICATION,    //i.e. setup VAO
							 READY_FOR_MAPPING,                        
							 MAPPED,
							 UNMAPPED,
							 INVALID };

	class MappableBuffer {
	public:
		MappableBuffer();
		MappableBuffer(GLuint ID);
		MappableBuffer(GLuint ID, bool persistant);
		//MappableBuffer(GLuint ID, bool coherent);

		//Copying is illegal
		MappableBuffer(const MappableBuffer &) = delete;
		MappableBuffer& operator=(const MappableBuffer &) = delete;

		//But moving is allowed
		MappableBuffer(MappableBuffer &&);
		MappableBuffer& operator=(MappableBuffer &&);

		//////////////////////////
		///   Public Interface 
		//////////////////////////
		
		//The public interface functions are meant to be called based on the 
		//state of this object. 
		

		//In BufferState::NO_BUFFER_ID
		void setBufferID(GLuint ID);


		//In BufferState::AQUIRING_BUFFER_STORAGE_FLAGS
		void mapPersistent(bool enable = true);
		void mapForReading(bool enable = true);
		void mapForWriting(bool enable = true);
		void mapCoherent(bool enable = true);



		//Allocates memory for this buffer, causing state to change from
		//AQUIRING_BUFFER_STORAGE_FLAGS to REQUIRES_VERTEX_LAYOUT_SPECIFICATION.
		//After allocation, none of the buffer storage flags can be modified.
		void allocateMemory();

		
		//In BufferState::REQUIRES_VERTEX_LAYOUT_SPECIFICATION
		/*    Put functions here that interface with the VertexArrayObject*/





	private:
		//Member Variables
		GLuint mID;
		bool mError;
		BufferState mState;

		std::unique_ptr<VertexArrayObject> mVertexLayout;
		
		GLbitfield mBufferStorageFlags;




		//Helper Functions
		void initialize();


		//To prevent the buffer's storage flags from getting modified once
		//memory has been allocated, function pointers will be employed
		//to deactivate the mapX() functions functionality
		void (MappableBuffer::*funcPtrToMapPersistentImplementation)(bool);
		void (MappableBuffer::*funcPtrToMapForReadingImplementation)(bool);
		void (MappableBuffer::*funcPtrToMapForWritingImplementation)(bool);
		void (MappableBuffer::*funcPtrToMapCoherentImplementation)(bool);


		//If memory has already been allocated
		void preventChangesToBufferStorageFlags(bool); 
		//If memory has not yet been allocated
		void mapPersistentImplementation(bool);
		void mapForReadingImplementation(bool);
		void mapForWritingImplementation(bool);
		void mapCoherentImplementation(bool);
	};




} //namespace ShaderInterface

#endif //MAPPABLE_BUFFER_H_