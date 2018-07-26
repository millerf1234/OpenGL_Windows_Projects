//This is a header-only struct that represents that information contained in an OpenGL VertexAttribPointer.
//This struct is used to interface between the Vertex shader of a shaderprogram and input buffer data.
//
// The struct in this file will contain the same information as what goes into the function calls listed at:
// https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glVertexAttribPointer.xhtml
//
//Created by Forrest Miller on July 25, 2018

#pragma once

#ifndef VERTEX_ATTRIB_POINTER_DATA_H_
#define VERTEX_ATTRIB_POINTER_DATA_H_

#include "ProjectConstants.h"
#include "ProjectParameters.h"

namespace ShaderInterface {

	enum class VertexAttribPointerType {BYTE, UNSIGNED_BYTE, SHORT, UNSIGNED_SHORT, INT, UNSIGNED_INT, HALF_FLOAT,
		FLOAT, FIXED, INT_2_10_10_10_REV, UNSIGNED_INT_2_10_10_10_REV, UNSIGNED_INT_10f_11f_11f_REV};



	//This struct holds the extra information that goes along with a VertexAttribPointer. It does not have 
	//the index or the actual pointer itself, however, and just represents the abstract properties of the pointer
	typedef struct VertexAttribPointerData {
		//mSize can be 1, 2, 3, or 4 only!
		GLint attribSize;
		VertexAttribPointerType attribType;
		GLsizei attribStride;
		GLboolean attribNormalized;
		VertexAttribPointerData() = delete;
		VertexAttribPointerData(const GLint size, const VertexAttribPointerType type, const GLsizei stride = 0, const GLboolean normalized = GL_FALSE) {
			if (size < 1 || size > 4) {
				fprintf(WRNLOG, "Warning! VertexAttribPointerData struct was attempted to be constructed with an invalid size!\n");
				fprintf(WRNLOG, "The requested size was %d but size MUST be a value between 1 and 4!\nSize will be set to 4.\n", size);
				attribSize = 4; //Default option is to set size to 4
			}
			else {
				attribSize = size;
			}
			
			attribType = type;
			attribStride = stride;
			attribNormalized = normalized;
		}

		//Copying and moving allowed
		VertexAttribPointerData(const VertexAttribPointerData& that) = default;
		VertexAttribPointerData(VertexAttribPointerData&& that) = default;
		VertexAttribPointerData& operator=(const VertexAttribPointerData& that) = default;
		VertexAttribPointerData& operator=(VertexAttribPointerData&& that) = default;

		bool operator==(const VertexAttribPointerData& that) const {
			if (this == &that) 
				return true;
			else if ( ((attribType == that.attribType) && (attribSize == that.attribSize)) && (attribNormalized == that.attribNormalized) ) 
				return true;
			return false;
		}

		bool operator!=(const VertexAttribPointerData& that) const {
			if (this == &that)
				return false;
			else if ( ((attribType == that.attribType) && (attribSize == that.attribSize)) && (attribNormalized == that.attribNormalized) ) {
				return false;
			}
			return true;
		}
	} VertexAttribPointerData;

} // namespace ShaderInterface



#endif //VERTEX_ATTRIB_POINTER_DATA_H_