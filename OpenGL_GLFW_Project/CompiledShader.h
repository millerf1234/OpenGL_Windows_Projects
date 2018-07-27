//Abstract base class that the 5 shader types derive from.
//
// Created by Forrest Miller over the days 7/24/2018 to 7/26/2018

#pragma once

#ifndef COMPILED_SHADER_H_
#define COMPILED_SHADER_H_

#include <fstream>
#include <list>

#include "ProjectConstants.h"
#include "ProjectParameters.h"

namespace ShaderInterface {
	constexpr size_t SHADER_COMPILATION_INFO_LOG_BUFFER_SIZE = 768ull;

	//Enum for the various shader types
	enum class ShaderType {VERTEX, GEOMETRY, TESSELATION_CONTROL, TESSELATION_EVALUATION, FRAGMENT, COMPUTE, UNSPECIFIED};

	class CompiledShader {
	public:
		CompiledShader() = delete; //Filepath required to construct
		CompiledShader(const char * filepath);
		CompiledShader(const CompiledShader &) = default;
		CompiledShader(CompiledShader&&) = default;
		virtual ~CompiledShader();

		//-------------------------------
		//Public Interface Functions  
		//-------------------------------
		//Decomission marks a shader as no-longer needed for more programs and releases this objects resources.
		//After being decommisioned, a shader must be reinstated in order to be attached to new shader programs.
		virtual void decommision() = 0;  //Alternative function names: abolish(), deactivate(), unload(), 
		
		//Reloads the shader and allows it to be attached to ShaderPrograms again. Could change this
		//objects GL-Context-Assigned ID Number  (I am unsure if this will invalidate old shaders still bound with the old ID number of this shader?)
		virtual void reinstate() = 0; //Alternative function names: reestablish(), restore(), rehabilitate(), revive(), reload(), recompile(),
		
		
		//-------------------------------
		//Operators
		//-------------------------------
		CompiledShader& operator=(const CompiledShader&) = default;
		CompiledShader& operator=(CompiledShader&&) = default;
		//Compares ShaderTypes and Filepaths for equality
		bool operator==(const CompiledShader&) const; 
		//Compares ShaderTypes and Filepaths for inequality
		bool operator!=(const CompiledShader&) const;


		//-------------------------------
		//Member field getters
		//-------------------------------
		const char * getFilepath() const { return mFilepath; }
		bool seeIfIsDecomissioned() const { return mWasDecomissioned;  }
		bool valid() const { return mValid; }
		bool validFilepath() const { return mValidFilepath; }
		bool isCompiled() const { return mHasBeenCompiled; }
		bool error() const { return mError; }
		ShaderType type() const { return mType; }
		GLuint shaderID() const { return mShaderID; }

	protected:
		//-------------------------------
		//Protected member variables
		//-------------------------------
		ShaderType mType; 
		GLuint mShaderID;
		const char * mFilepath;
		std::string mSourceText; //The entire text of the source file
		GLchar mCompilationInfoLog[SHADER_COMPILATION_INFO_LOG_BUFFER_SIZE]; //Buffer for storing shader-compilation error messages
		bool mValid; //True on successful shader compilation
		bool mValidFilepath; //True on successfuly opening of file handle 
		bool mError; //True on encountering an error
		bool mHasLoadedSourceText;
		bool mHasBeenCompiled; //Becomes true once the shader gets compiled
		bool mWasDecomissioned; //Becomes true if the shader is decomissioned

		//-------------------------------
		//Protected Functions
		//-------------------------------
		//Compiles the shader
		bool compile();
		virtual void aquireShaderID() = 0;
		

		//Loads the text of a file located at filepath. Useful for loading shaders
		void loadSourceFile(const char * filepath, std::string& textBuffer);
		bool checkForCompilationErrors();

	private:
		void initialize(const char * filepath); //Initializes variables
		void initializeBooleans(); //Initializes all of the boolean members of this object
		//Check the validity of the filepath 
		bool validateFilepath(std::ifstream&);
	};

} //namespace ShaderInterface

#endif //COMPILED_SHADER_H_