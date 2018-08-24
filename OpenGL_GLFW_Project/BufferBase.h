//  (TWO WAYS OF DOING THIS. SEE GLBufferWrapper.h AS WELL)
//Abstract base buffer for all the various buffer types that can be created with OpenGL.
//	
//
//		Classes that derive from this class will include:   (see https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glBindBuffer.xhtml)
//
//				ARRAY_BUFFER						 -- Not Yet Implemented
//				ATOMIC_COUNTER_BUFFER                -- Not Yet Implemented
//				COPY_READ_BUFFER					 -- Not Yet Implemented
//				COPY_WRITE_BUFFER					 -- Not Yet Implemented
//				DISPATCH_INDIRECT_BUFFER             -- Not Yet Implemented
//				DRAW_INDIRECT_BUFFER	             -- Not Yet Implemented
//				ELEMENT_ARRAY_BUFFER	             -- Not Yet Implemented
//				PIXEL_PACK_BUFFER                    -- Not Yet Implemented	
//				PIXEL_UNPACK_BUFFER	                 -- Not Yet Implemented
//				QUERY_BUFFER                         -- Not Yet Implemented
//				SHADER_STORAGE_BUFFER                -- Not Yet Implemented	
//				TEXTURE_BUFFER						 -- Not Yet Implemented
//				TRANSFORM_FEEDBACK_BUFFER            -- Not Yet Implemented	
//				UNIFORM_BUFFER						 -- Not Yet Implemented
//				
//
// File created 8/24/2018  by Forrest Miller but there is no way it will be completed this day


#pragma once

#ifndef BUFFER_BASE_H_
#define BUFFER_BASE_H_

namespace ShaderInterface {

	class BufferBase {
	public:
		BufferBase();
		~BufferBase();

		//Copying is not allowed
		BufferBase(const BufferBase& that) = delete;
		BufferBase& operator=(const BufferBase& that) = delete;

		

	};

}  //namespace ShaderInterface



#endif //BUFFER_BASE_H_