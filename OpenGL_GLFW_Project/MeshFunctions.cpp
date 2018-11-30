//File:                  MeshFunctions.cpp
//Description:           Contains implementations for some helpful functions
//                       for mesh processing.
//
//Programmer:            Forrest Miller
//Date:                  November 28, 2018

#include "MeshFunctions.h"

namespace MeshFunc {

	glm::vec3 computeVertexNormalsForTriangle(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2) {
		glm::vec3 edge1 = (v1 - v0);
		glm::vec3 edge2 = (v2 - v0);

		glm::vec3 normal = glm::cross(edge1, edge2);
		
		float normalMagnitude = glm::length(normal);
		
		//Check to make sure computed normal is not degenerate
		if (abs(normalMagnitude) <= 0.000001f) {
			fprintf(MSGLOG, "\nDegenerate Normal Calculated for triangle:\n\t\tv0 = {%.2f, %.2f, %.2f},"
				"v1 = {%.2f, %.2f, %.2f}, v2 = {%.2f, %.2f, %.2f}\n", v0.x, v0.y, v0.z,
				v1.x, v1.y, v1.z, v2.x, v2.y, v2.z);
			if (abs(glm::length(edge1)) < 0.000001f) {
				return glm::vec3(0.0f, 0.0f, 1.0f);
			}
			else {
				return (glm::vec3(edge1.y, edge1.z, edge1.x)); //Return a vector anyways
			}
		}
		else {
			return ( (1.0f / normalMagnitude) * normal);
		}
	}


} //namespace MeshFunc
