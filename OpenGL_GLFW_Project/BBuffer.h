//A better buffer
//     (or so I hope)
//
//  Sept 8, 2018   Forrest Miller

#pragma once

#ifndef BBUFFER_H_
#define BBUFFER_H_

#include "VertexArrayObject.h"
#include "ProjectConstants.h"
#include "ProjectParameters.h"

namespace ShaderInterface {

	typedef struct BufferMappingParameters {
		bool mappable;             //Determines if the buffer has been set to be mappable
		//These next variables matter only if the 'mappable' is true
		bool readEnabled;          //Determines if the buffer is mapped to support reading
		bool writeEnabled;         //Determines if the buffer is mapped to support writing
		bool persistentMapping; 
		bool coherentMapping;      //If true, this tells OpenGL that very tight synchronization between CPU and GPU is desired. If this is false, OpenGL must be notified each time data is updated.
		bool clientStoragePrefered;  //
		
		bool isCurrentlyMapped;

	};

	enum class ContextBindPoint { ARRAY_BUFFER, 
								  ATOMIC_COUNTER_BUFFER,				 //-- Not Yet Implemented
								  COPY_READ_BUFFER,						 //-- Not Yet Implemented
								  COPY_WRITE_BUFFER,					 //-- Not Yet Implemented
								  DISPATCH_INDIRECT_BUFFER,				 //-- Not Yet Implemented
								  DRAW_INDIRECT_BUFFER,					 //-- Not Yet Implemented
								  ELEMENT_ARRAY_BUFFER,					 //-- Not Yet Implemented
								  PIXEL_PACK_BUFFER,					 //-- Not Yet Implemented	
								  PIXEL_UNPACK_BUFFER,					 //-- Not Yet Implemented
								  QUERY_BUFFER,                          //-- Not Yet Implemented
								  SHADER_STORAGE_BUFFER,                 //-- Not Yet Implemented	
								  TEXTURE_BUFFER,						 //-- Not Yet Implemented
								  TRANSFORM_FEEDBACK_BUFFER,             //-- Not Yet Implemented	
								  UNIFORM_BUFFER,						 //-- Not Yet Implemented
	};

	class BBuffer {
	public:
		//////////////////////////////////////////////////////////////////////////
		//          Constructor, Destructor, Copy, Move operators              ///
		//////////////////////////////////////////////////////////////////////////
		BBuffer();
		BBuffer(GLuint bufferID);
		~BBuffer();

		//No Copying
		BBuffer(const BBuffer &) = delete;
		BBuffer& operator=(const BBuffer &) = delete;

		//Moving is allowed
		BBuffer(BBuffer &&);
		BBuffer& operator=(BBuffer &&);
		


		//////////////////////////////////////////////////////////////////////////
		///	    	                 Public interface                          ///
		//////////////////////////////////////////////////////////////////////////
		
		///////////////////////////////////
		/// State-Independent functions ///
		///////////////////////////////////
		//setPreferedBindingPoint()

		///////////////////////////////////////
		///  State Dependent functionality  ///
		///////////////////////////////////////

		//State Dependence: No ID assigned yet
		void assignID(GLuint);

		//State Dependence: Buffer has not yet been allocated
		//Set Storage Flags
		//Set storage space
		//allocate memory (locks object state)
		
		//State Dependence: Memory has been allocated
		//Bind to context (with a binding point as optional parameter)
		//Additional Dependence: Buffer is mappable
		void map();
		void unmap();
		void flushBufferUpdates();

	private:
		std::unique_ptr<VertexArrayObject> mVAO;
		
		
		void * initialData;
		
		//State
		bool mHasID;
		bool mHasMemoryAllocate;
		
		BufferMappingParameters mBufferMappingState;
	};

} //namespace ShaderInterface

#endif BBUFFER_H_