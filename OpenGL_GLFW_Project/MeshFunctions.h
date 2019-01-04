//File:                  MeshFunctions.h
//
//Description:           Contains helpful functions for mesh processing.
//
//Programmer:            Forrest Miller
//Date:                  November 28, 2018


#pragma once

#ifndef MESH_FUNCTIONS_H_
#define MESH_FUNCTIONS_H_


#include "GlobalIncludes.h"


namespace MeshFunc {

	//Computes normalized Vertex Normals for a triangle T = (V0, V1, V2), where each Vi is a 3-dimensional position vector.
	//    [This function performs additional safety checks to prevent a division by 0 if a degenerate triangle is passed in.
	//     If it can be guarenteed that the passed in triangle is not ill-formed, then consider using the function 
	//           'computeNormalizedVertexNormalsForTriangle_Unsafe()' 
	//     which will calculate the normalized triangle normal without any additional safety checks]
	glm::vec3 computeNormalizedVertexNormalsForTriangle(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2);


	//Computes normalized Vertex Normals for a triangle T = (V0, V1, V2), where each Vi is a 3-dimensional position vector.
	//    [This version of the function performs the normal calculation directly based off the parameter vertices without 
	//     verifying triangle validity. Passing in a degenerate triangle will cause bad things to happen, so only call
    //     this function when dealing with meshes that are guarenteed/known to be well behaved]
	glm::vec3 computeNormalizedVertexNormalsForTriangle_Unsafe(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2);
	

} //namespace MeshFunc

#endif //MESH_FUNCTIONS_H_
