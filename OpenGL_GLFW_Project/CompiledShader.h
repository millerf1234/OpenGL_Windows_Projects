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
	constexpr size_t SHADER_COMPILATION_INFO_LOG_BUFFER_SIZE = 768ull;

	//Enum for the various shader types
	enum class ShaderType {VERTEX, GEOMETRY, TESSELATION_CONTROL, TESSELATION_EVALUATION, FRAGMENT, UNSPECIFIED};

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
		//Successful calls to this function should also call setDecomissioned() to reflect the state change.
		virtual void decommision() = 0;  //Alternative function names: abolish(), deactivate(), unload(), 
		//This function is expected to be called on a 
		//Successful calls to this function should also call setRecomissioned() to reflect the state change.
		virtual void reinstate() = 0; //Alternative function names: reestablish(), restore(), rehabilitate(), revive(), reload(), recompile(),
		
		
		//-------------------------------
		//Operators
		//-------------------------------
		CompiledShader& operator=(const CompiledShader&) = default;
		CompiledShader& operator=(CompiledShader&&) = default;
		//Compares type and filepath strings for equality
		bool operator==(const CompiledShader&) const; 
		//Compares type and filepath strings for inequality
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
		bool mHasBeenCompiled; //Becomes true once the shader gets compiled
		bool mWasDecomissioned; //Becomes true if the shader is decomissioned

		//-------------------------------
		//Protected Functions
		//-------------------------------
		//Compiles the shader
		virtual bool compile() = 0;
		

	private:
		void initialize(const char * filepath); //Initializes variables
		void initializeBooleans(); //Initializes all of the boolean members of this object

		//Loads the text of a file located at filepath. Useful for loading shaders
		void loadSourceFile(const char * filepath, std::string& textBuffer);
		
		//Check the validity of the filepath 
		bool validateFilepath(std::ifstream&);
	};

#if 0  //I am going to try to clean up this class a bit
	class CompiledShader {
	public:
		//CompiledShader();
		CompiledShader(const char * mFilepath);
		virtual ~CompiledShader();

		const char* getFilepath() const { return mFilepath; }

		bool valid() const { return mValid; }
		bool validFilepath() const { return mValidFilepath; }
		bool error() const { return mError; }
		ShaderType type() const { return mType; }

		bool operator==(const CompiledShader&); //For comparing shaders...
		bool operator!=(const CompiledShader&); 

		//Copying should be redefined in derived classes as discussed at here given at: https://stackoverflow.com/questions/669818/virtual-assignment-operator-c   
		//Also see Page 773-774 of the C++ Primer Plus book.  
		CompiledShader(const CompiledShader &);
		CompiledShader& operator=(const CompiledShader &);
		CompiledShader(CompiledShader && that); //Not sure about this one... It can't be virtual since it's a constructor
		CompiledShader& operator=(CompiledShader &&) = 0;

		std::string getSourceText() const { return mSourceText; }

		virtual void compile() = 0; //To be implemented by derived classes

		virtual void deleteShader();
		
	protected:
		ShaderType mType; 
	
		const char * mFilepath;
		std::string mSourceText;
		GLchar infoLog[512]; //Buffer for storing shader-compilation error messages
	
		GLuint mID;

		//Variables representing object's state:
		bool mValid; //True on successful shader compilation
		bool mValidFilepath; //True on successfuly opening of file handle 
		bool mError; //True on encountering an error

		//Protected functions
		//Loads the text of a file located at filepath. Useful for loading shaders
		std::string loadSourceFile(char * filepath) const;

	private:
		//initialize should only be called from a constructor
		void initialize();

	};

#endif //#if 0
} //namespace ShaderInterface

#endif //COMPILED_SHADER_H_