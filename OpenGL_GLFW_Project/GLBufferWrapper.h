//  (TWO WAYS OF DOING THIS. SEE BaseBuffer.h AS WELL)
//This class is intended to store all of the necessary data and interface for 
//	any GLBuffer object which results from a glBindBuffers call (see list). 
//	This class will be owned by the specific buffer types.
//
//		Classes that will have an instance of this class will include:   (see https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glBindBuffer.xhtml)
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

#ifndef GL_BUFFER_WRAPPER_H_
#define GL_BUFFER_WRAPPER_H_

namespace ShaderInterface {

	namespace BufferInternal {


		class GLBuffer {
		public:
			GLBuffer();
			~GLBuffer();


		};


	} //Namespace BufferInternal

} //Namespace ShaderInterface



#endif //GL_BUFFER_WRAPPER_H_