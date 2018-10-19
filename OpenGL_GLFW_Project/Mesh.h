//File:         Mesh.h
//Description:  A wrapper class for 3D mesh data for an object
//
//Programmer:   Forrest Miller
//Date:         October 14, 2018

#pragma once

#ifndef MESH_H_
#define MESH_H_

#include <vector>

#include "LoggingMessageTargets.h"
#include "Vertex.h"

using Offset = size_t;
using DatapointCount = size_t;

//A type is introduced to allow SubMeshes to be quickly extracted from a large 
//mesh containing the entire model
typedef std::pair<Offset, DatapointCount> SubMeshBounds;

enum class VertType { BASIC, FULL, EXTENDED };

class Mesh {
public:
	Mesh();
	~Mesh();

	//Returns the mesh as a contiguous interleaved array of floats from
	//whatever interal storage representation the object currently has.
	std::vector<float> get() const;
	//Copies data from the mesh as a contiguous interleaved array of floats from
	//whatever interal storage representation the object currently has onto the
	//end of the vector given as parameter.
	void addSelfTo(std::vector<float> &) const;

	//Returns the mesh as a contiguous interleaved array of floats with the 
	//number of components per vertex being reduced to just the components that
	//actually are used to store data.
	std::vector<float> getCondensed() const;

	////Copies data from the mesh as a contiguous interleaved array of floats from
	////whatever interal storage representation the object currently has onto the
	////end of the vector given as parameter.
	//void addCondensedSelfTo(std::vector<float> &) const;
	
	//Returns a submesh from this mesh. Illegal bounds result in an empty mesh
	//getting returned
	std::unique_ptr<Mesh> getSubmesh(SubMeshBounds subMeshBounds);

	//Copies data from a submesh of this mesh into the vector. Will copy 0 data
	//if given illegal mesh bounds.
	void addSubmeshTo(std::vector<float> &, SubMeshBounds subMeshBounds);

	DatapointCount getUsedVertexComponentsCount() const { return mActualVertexSize_; }

	//Returns the number of floating point values stored within the mesh.
	//Largest number that can be used for 'DatapointCount' in SubMeshBounds.
	DatapointCount getMeshSize() const { return mMeshSize_; }

	//Returns the number of actual floating point values stored within the 
	//mesh based off the number of components per vertex.
	DatapointCount getCondensedMeshSize() const;

	//Gets the remaining size of the mesh past a given offset.
	DatapointCount getMeshSizeBeyondOffset(Offset offset) const;

	//Gets the condensed remaining number of floats in the mesh beyond the
	//uncondensed offset value.
	DatapointCount getCondensedMeshSizeBeyondOffset(Offset uncondensedOffset) const;

	//Retrieves the type of data stored by this object
	VertType getVertType() const { return mVertType_; }


private:
	VertType mVertType_;
	DatapointCount mMeshSize_; //Number of floating point values in the Mesh
	DatapointCount mActualVertexSize_; //Actual number of stored components per vertex

	/*union Vertices {
		std::vector<Vertex> mBasicVertices_;
		std::vector<FullVertex> mFullVertices_;
		std::vector<ExtendedVertex> mExtendedVertices_;
	};
	union Vertices mVertices_;*/

	//or
	//std::vector<Vertex> mVertices_;
	//std::vector<FullVertex> mFullVertices_;
	//std::vector<ExtendedVertex> mExtendedVertices_;
};

#endif //MESH_H_