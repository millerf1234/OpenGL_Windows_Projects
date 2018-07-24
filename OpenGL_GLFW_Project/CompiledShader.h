//Abstract base class that the four GL shaders derive from

#pragma once

#ifndef COMPILED_SHADER_H_
#define COMPILED_SHADER_H_

#include "ProjectConstants.h"
#include "ProjectParameters.h"

namespace ShaderInterface {
	class CompiledShader
	{
	public:
		CompiledShader();
		~CompiledShader();

		const GLchar* getFilepath() const { return mFilepath; }

		bool valid() const { return mValid; }
		bool validFilepath() const { return mValidFilepath; }
		bool error() const { return mError; }

	private:
		const GLchar * mFilepath;
		GLuint mID;

		//Variables representing object's state:
		bool mValid; //True on successful shader compilation
		bool mValidFilepath; //True on successfuly opening of file handle 
		bool mError; //True on encountering an error

	};
} //namespace ShaderInterface

#endif //COMPILED_SHADER_H_