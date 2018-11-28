//This is a utility shader full of utility functions that 
//are meant to be called from other Vertex shaders. This
//shader does not have a 'main' function nor does it 
//have input or output. This shader should be marked 
//as 'secondary' within the application code.

//NOTE: Read the GLSL rules! https://www.khronos.org/opengl/wiki/Core_Language_(GLSL)

//Also useful for setting up transforms on Application side to
//be passed in as uniforms: https://glm.g-truc.net/0.9.2/api/a00003.html

#version 450 core


//Convert Degrees to Radians 
float dg2Rad(float degrees) {
	return (degrees * 3.1415927 / 180.0f);
}

//Convert radians to degrees
float rad2Dg(float radians) {
	return (radians * 180.0f / 3.1415927);
}

//Scales a vector's y-component to compensate for destortion from the aspect-ratio 
void setAspectRatio(inout vec3 vert, in float aspectRatio) {
	//Simple Implementation
	vert.y *= aspectRatio;

	// As a matrix multiplication, this operation would use the following matrix
	//mat3 aspectCorrection =  mat3(1.0f,    0.0f,     0.0f,
	//					            0.0f, aspectRatio, 0.0f,
	//					            0.0f,    0.0f,     1.0f);
	//

}

 void rotateX(inout vec3 vert, in float theta) {

	mat3 xRot = mat3( 1.0,     0.0    ,     0.0    ,
					  0.0,  cos(theta), -sin(theta),
		              0.0,  sin(theta),  cos(theta));

	vert = xRot * vert;
}

void rotateY(inout vec3 vert, in float theta) {

	mat3 yRot = mat3( cos(theta),  0.0,   -sin(theta),
					     0.0,      1.0,       0.0    ,
					  sin(theta),  0.0,    cos(theta) );

	vert = (yRot * vert);
}

void rotateZ(inout vec3 vert, in float theta) {

	mat3 zRot = mat3( cos(theta), -sin(theta), 0.0,
					  sin(theta),  cos(theta), 0.0,
					     0.0,         0.0,     1.0);

	vert = (zRot * vert);
}

