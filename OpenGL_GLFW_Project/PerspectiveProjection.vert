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