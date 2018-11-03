//File:                             PerspectiveProjection.vert
//Programmer:                       Forrest Miller
//Date:                             October 30, 2018
//Description:                      Definition for a function that performs a perspective
//                                  projection transformation on a vertex.
//
//									 Note that this is not a full Model-View-Projection matrix operation. 
//                                    (Or at least, not yet...)
//
// This is my first attempt at the perspective projection. This is based on "Mathematics
//  For 3D Game Programming and COmputer Graphics" pages 105-115 (matrix on page 115). 


//For refernce, from Vulkan project:
/* #include "PrepareAPerspectiveProjectionMatrix.h"

namespace VulkanSetup {
	//See pages 515-519
	Matrix4x4 PreparePerspectiveProjectionMatrix(float aspect_ratio,
												 float field_of_view,
												 float near_plane,
												 float far_plane) {
		float f = 1.0f / tan(Deg2Rad(0.5f * field_of_view));

		Matrix4x4 perspective_projection_matrix = {
			f / aspect_ratio,
			0.0f,
			0.0f,
			0.0f,

			0.0f,
			-f,
			0.0f,
			0.0f,

			0.0f,
			0.0f,
			far_plane / (near_plane - far_plane),
			-1.0f,

			0.0f,
			0.0f,
			(near_plane * far_plane) / (near_plane - far_plane),
			0.0f
		};
		return perspective_projection_matrix;
	}

} // namespace VulkanSetup */

#version 450 core

void frustrumMult(inout vec4 vert, in float near, in float far, in float left, in float right, in float bottom, in float top) {
	
	mat4 frustrum = mat4 (   ( 2.0*near / (right - left)),                       0.0 ,                   0.0,                     0.0,
							             0.0,                    ( 2.0*near / (top - bottom)),           0.0,                     0.0,
		                    (right + left) / (right - left),     (top + bottom) / (top - bottom),  -(far+near) / (far-near),     -1.0,
		                                 0.0,                                    0.0,             -2.0*near*far / (far-near),     0.0);

	vert = frustrum * vert;
}



//void computePerspective(inout vec3 vert, in float zoom, in float near, in float far, in float focalLength, in float aspectRatio) {
//
//}

//void computePerspective(inout vec4 vert, in float near, in float far, in float )