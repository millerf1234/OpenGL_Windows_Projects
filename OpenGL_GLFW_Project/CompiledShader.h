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

		bool operator==(const CompiledShader&); //For comparing shaders...
		bool operator!=(const CompiledShader&); 

	private:
		const GLchar * mFilepath;
		GLchar infoLog[512]; //Buffer for storing shader-compilation error messages
		GLuint mID;

		//Variables representing object's state:
		bool mValid; //True on successful shader compilation
		bool mValidFilepath; //True on successfuly opening of file handle 
		bool mError; //True on encountering an error

	};
} //namespace ShaderInterface

#endif //COMPILED_SHADER_H_