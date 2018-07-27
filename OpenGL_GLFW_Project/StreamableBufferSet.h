

//
//Author: Forrest S Miller
//Date(s):   July 25, 2018         File created in place of the simple VBO/EBO/VAO wrapper class I was originally going to write.

#pragma once

#if 0

#ifndef STREAMABLE_BUFFER_SET_H_
#define STREAMABLE_BUFFER_SET_H_

#include "ProjectConstants.h"
#include "ProjectParameters.h"

namespace ShaderInterface {

	enum class VertexFormat { 
		VERT2, VERT3, VERT4, VERT2TEXEL2, VERT2COLOR3TEXEL2,
		VERT3TEXEL2, VERT3TEXEL3, VERT3NORML3, VERT2NORMAL2TEXEL2,
		VERT3NORMAL3TEXEL3, VERT3TEXEL3NORMAL3, VERT3NORMAL3TEXEL2, 
		NOT_SET, /* CUSTOM, */                                      //Custom to be implemented. Will need to tie a name and a program ID together to get attribute locations
	};


	class VertexData {
	public:
		VertexData();
		~VertexData();

	private:

		//This list of variables is no where near complete...
		bool mReady;
		bool mValid;

		bool mHasEbo;
		GLuint mEBO;

		VertexFormat mFormat;


	};


} //namespace ShaderInterface

#endif //'#if 0'

#endif STREAMABLE_BUFFER_SET_H_