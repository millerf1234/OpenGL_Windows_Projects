// This class is a wrapper for a UniformLocation that can be requested by outside code to obtain a faster uniform
// update route. While faster than making requests to the UniformLocationTracker directly each time a uniform update 
// is required, aquiring and managing one of these objects allows for a direct update for the uniform without
// having to look up it's location
//
//This class makes heavy use of the functions at: https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glProgramUniform.xhtml\
//
//Created by Forrest Miller on July 23, 2018



#pragma once

#ifndef CACHED_UNIFORM_LOCATION_H_
#define CACHED_UNIFORM_LOCATION_H_

#include "GlobalIncludes.h"
#include "UniformLocationBucket.h"
#include "UniformLocationTracker.h"

namespace ShaderInterface {

	class UniformLocationTracker; //Defined later, declared here to let that class be friend of this one

	class CachedUniformLocation final {
	public:
		friend class UniformLocationTracker; //Based off example at https://www.codingunit.com/cplusplus-tutorial-friend-function-and-friend-class
											 //friend bool UniformLocationTracker::checkIfInitialized() const { return mWasInitialized; }

		CachedUniformLocation(); //Default Constructs an invalid object that needs to be validated. See UniformLocationTracker
		CachedUniformLocation(const GLchar * name, GLuint programID, UniformType type, GLsizei count);
		~CachedUniformLocation();

		//Nothing fancy to do when it comes to copying...
		CachedUniformLocation(const CachedUniformLocation &) = default;
		CachedUniformLocation(CachedUniformLocation&&) = default;
		CachedUniformLocation& operator=(const CachedUniformLocation&) = default;
		CachedUniformLocation& operator=(CachedUniformLocation&&) = delete;

		//Can use this CachedLocation object to update shader-code uniforms directly with the following function calls
		void updateUniform(void * data);
		void updateUniform(void * xPtr, void * yPtr); //For uniform2i, uniform2ui, uniform2f only
		void updateUniform(void * xPtr, void * yPtr, void * zPtr); //For uniform3i, uniform3ui, uniform3f only
		void updateUniform(void * xPtr, void * yPtr, void * zPtr, void * wPtr); ////For uniform4i, uniform4ui, uniform4f only

		const GLchar * getUniformName() const { return mUniformName; }
		GLint uniformLocation() const { return mUniformLocation; }
		GLuint programID() const { return mProgramID; }
		bool checkIfValidUniformLocation() { return mValidLocation; }
		UniformType getType() const { return mUniformType; }

		//Matrix and Vector only:
		GLsizei getCount() const { return mCount; }


	private:
		const GLchar * mUniformName;
		GLint mUniformLocation;
		GLuint mProgramID;
		UniformType mUniformType;
		bool mWasInitialized;
		bool mValidLocation;
		GLsizei mCount; //Only for vectors and matrices

	};


} // namespace ShaderInterface


#endif // CACHED_UNIFORM_LOCATION_H_