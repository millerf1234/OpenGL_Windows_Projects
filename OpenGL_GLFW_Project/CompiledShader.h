//Abstract base class that the four (five if counting tesselation as 2) GL shaders derive from
//
// Created by Forrest Miller on 7/24/2018 but most of the coding completed on 7/25/2018

#pragma once

#ifndef COMPILED_SHADER_H_
#define COMPILED_SHADER_H_

#include <fstream>
#include <list>

#include "ProjectConstants.h"
#include "ProjectParameters.h"



namespace ShaderInterface {

	//Enum for the various shader types
	enum class ShaderType {VERTEX, GEOMETRY, TESSELATION_CONTROL, TESSELATION_EVALUATION, FRAGMENT, UNSPECIFIED};


	class CompiledShader {
	public:
		CompiledShader();
		virtual ~CompiledShader();

		const char* getFilepath() const { return mFilepath; }

		bool valid() const { return mValid; }
		bool validFilepath() const { return mValidFilepath; }
		bool error() const { return mError; }
		ShaderType type() const { return mType; }

		bool operator==(const CompiledShader&); //For comparing shaders...
		bool operator!=(const CompiledShader&); 

		//Copying not allowed but moving is okay
		CompiledShader(const CompiledShader &) = delete;
		CompiledShader& operator=(const CompiledShader &) = delete;
		//CompiledShader(CompiledShader && that); //Not sure about this one... It can't be virtual since it's a constructor
		//virtual CompiledShader& operator=(CompiledShader &&) = 0;

		virtual void compile() = 0; //To be implemented by derived classes

		virtual void deleteShader();
		
	protected:
		ShaderType mType; 

		//Loads the text of a file located at filepath. Useful for loading shaders
		std::string loadSourceFile(char * filepath) const;
		
	
		const char * mFilepath;
		GLchar infoLog[512]; //Buffer for storing shader-compilation error messages
	
		GLuint mID;

		//Variables representing object's state:
		bool mValid; //True on successful shader compilation
		bool mValidFilepath; //True on successfuly opening of file handle 
		bool mError; //True on encountering an error
	private:
		//initialize should only be called from a constructor
		void initialize();

	};
} //namespace ShaderInterface

#endif //COMPILED_SHADER_H_