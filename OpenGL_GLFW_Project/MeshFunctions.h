//File:                  MeshFunctions.h
//
//Description:           Contains helpful functions for mesh processing.
//
//Programmer:            Forrest Miller
//Date:                  November 28, 2018


#pragma once

#ifndef MESH_FUNCTIONS_H_
#define MESH_FUNCTIONS_H_

#include "ProjectSetup.h"

namespace MeshFunc {

	//Computes normalized Vertex Normals for a triangle T = (v0, v1, v2) 
	glm::vec3 computeVertexNormalsForTriangle(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2);


} //namespace MeshFunc

#endif //MESH_FUNCTIONS_H_
