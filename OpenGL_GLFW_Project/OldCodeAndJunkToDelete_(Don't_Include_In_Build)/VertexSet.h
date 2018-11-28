//Class that wraps a set of vertices that can be used to represent a model.
//This is designed to be simple and quick, not necessarily fast or efficient.

#pragma once

#ifndef VERTEX_SET_H_
#define VERTEX_SET_H_

#include "AsciiAsset.h"
#include "Vertex.h"
#include <vector>

typedef struct Face {
	std::array<Vertex, 4> positions;
	std::array<Vertex, 4> textureCoords;
	std::array<Vertex, 4> normals;

	Face(int pos1, int tex1, int normal1, int pos2, int tex2, int normal2, int pos3, int tex3, int normal3);
	Face(int pos1, int tex1, int normal1, int pos2, int tex2, int normal2, int pos3, int tex3, int normal3,
		int pos4, int tex4, int normal4);
	
} Face;

class VertexSet {
public:
	/////////////////////////////////////////////////////////
	//  Rule of 5  Functionality
	/////////////////////////////////////////////////////////
	VertexSet(); //Load from asset? 
	~VertexSet();

	VertexSet(const VertexSet& that);
	VertexSet(VertexSet&& that);
	
	VertexSet& operator=(const VertexSet& that);
	VertexSet& operator=(const VertexSet&& that);

	/////////////////////////////////////////////////////////
	// Interface functions
	/////////////////////////////////////////////////////////
	
	void clear();





private:
	std::vector<Vertex> mPositions_;
	std::vector<Vertex> mNormals_;
	std::vector<Vertex> mTexCoords_;
	std::vector<Vertex> mColors_;

	
};



#endif //VERTEX_SET_H_