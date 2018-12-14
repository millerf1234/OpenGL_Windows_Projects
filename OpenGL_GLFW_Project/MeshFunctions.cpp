//File:                  MeshFunctions.cpp
//Description:           Contains implementations for some helpful functions
//                       for mesh processing.
//
//Programmer:            Forrest Miller
//Date:                  November 28, 2018

#include "MeshFunctions.h"

#include "LoggingMessageTargets.h"
#include "FloatingPointTolerance.h"

namespace MeshFunc {

	

	//Computes the normal vector for a triangle
	glm::vec3 computeNormalizedVertexNormalsForTriangle(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2) {
		glm::vec3 edge1 = (v1 - v0);
		glm::vec3 edge2 = (v2 - v0);

		glm::vec3 normal = glm::cross(edge1, edge2);
		
		float normalMagnitude = glm::length(normal);
		
		//Check to make sure computed normal is not degenerate
		if (normalMagnitude <= FP_TOLERANCE) {
			fprintf(WRNLOG, "\nWARNING! Degenerate Normal Calculated for triangle:\n\t\t\tv0 = {%.3f, %.3f, %.3f}, "
				"v1 = {%.3f, %.3f, %.3f}, v2 = {%.3f, %.3f, %.3f}\n", v0.x, v0.y, v0.z,
				v1.x, v1.y, v1.z, v2.x, v2.y, v2.z);
			if (glm::length(v0) < FP_TOLERANCE) {
				return glm::vec3(0.0f, 0.0f, 1.0f); //This is just an arbitrary value so that this function returns something reasonable
			}
			else {
				return ( (1.0f / glm::length(v0)) * v0); //Normalize v0 and use that as the normal
			}
		}
		else {
			return ( (1.0f / normalMagnitude) * normal);
		}
	}




	glm::vec3 computeNormalizedVertexNormalsForTriangle_Unsafe(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2) {
		glm::vec3 edge1 = (v1 - v0);
		glm::vec3 edge2 = (v2 - v0);

		//Compute normal
		glm::vec3 normal = glm::cross(edge1, edge2);
		
		return ( (1.0f / glm::length(normal)) * normal);
	}

} //namespace MeshFunc
