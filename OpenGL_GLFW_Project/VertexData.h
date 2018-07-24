//This class is a wrapper for shader input data. I am not sure yet to what extent of the drawing logic will run through this class.
// Right now it's just a VBO/EBO/VAO wrapper
//
//Author: Forrest S Miller
//Date(s):   July 24, 2018         File created


#pragma once

#ifndef VERTEX_DATA_H_
#define VERTEX_DATA_H_

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

#endif //VERTEX_DATA_H_