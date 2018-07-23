// This class is a wrapper for a UniformLocation that can be requested to obtain faster uniform
// updates. While faster than making requests to the UniformLocationTracker directly each time a uniform update 
// is required, aquiring and managing one of these objects is 
//
//This class makes heavy use of the functions at: https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glProgramUniform.xhtml\
//
//Created by Forrest Miller on July 23, 2018



#pragma once

#ifndef CACHED_UNIFORM_LOCATION_H_
#define CACHED_UNIFORM_LOCATION_H_

#include "ProjectConstants.h"
#include "ProjectParameters.h"
#include "UniformLocationBucket.h"

class CachedUniformLocation {
public:
	CachedUniformLocation() = delete;
	CachedUniformLocation(const UniformLocationBucket& ulBucket, GLuint programID, UniformType type, GLsizei count = 1, GLboolean transpose = false);
	CachedUniformLocation(const GLchar * name, GLuint programID, UniformType type, GLsizei count = 1, GLboolean transpose = false);
	~CachedUniformLocation();

	void updateUniform(void * data);
	void updateUniform(void * xPtr, void * yPtr);
	void updateUniform(void * xPtr, void * yPtr, void * zPtr);
	void updateUniform(void * xPtr, void * yPtr, void * zPtr, void * wPtr);

	const GLchar * getUniformName() const { return mUniformName; }
	GLuint programID() const { return mProgramID; }
	bool checkIfValidUniformLocation() { return mValidLocation; }
	UniformType getType() const { return mUniformType; }


	//Matrix and Vector only:
	GLsizei getCount() const { return mCount; }

	//Matrices Only:
	void enableTranspose() { mTranspose = true; }
	void disableTranspose() { mTranspose = false; }
	bool checkTranspose() const { return mTranspose; }




private:
	const GLchar * mUniformName;
	const GLuint mProgramID;
	const UniformType mUniformType;
	bool mValidLocation;
	bool mTranspose; //Only matters for matrices
	GLsizei mCount; //Only for vectors and matrices
	
};



#endif // CACHED_UNIFORM_LOCATION_H_