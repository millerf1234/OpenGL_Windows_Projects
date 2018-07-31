// //Abstract base class that the 5 shader types derive from.
//
//This is a rewritten version of my first version of CompiledShader. The previous
//version put everything in place and worked fine, I was concerned however that it's
//implementation was slightly sloppy and had become bloated (like OpenGL already stores the 
//shader source string so there is no reason to store a local copy). Thus I decided to try
//to rewrite the whole class structure using my previous version as a guide while trying to keep
//everything more streamlined and light-weight.
//
//
// The original version was written by Forrest Miller from between 7/24/2018 and 7/27/2018
// This new version was written by Forrest Miller on 7/29/2018
//
//

#pragma once

#ifndef COMPILED_SHADER_H_
#define COMPILED_SHADER_H_

#include <fstream>

#include "ProjectConstants.h"
#include "ProjectParameters.h"


namespace ShaderInterface {
	//-------------------------------
	//Enum for the various shader types
	//-------------------------------
	enum class ShaderType {VERTEX, GEOMETRY, TESSELATION_CONTROL, TESSELATION_EVALUATION, FRAGMENT, COMPUTE, UNSPECIFIED};

	//-------------------------------
	// Struct to tie ShaderID and ShaderType together
	//-------------------------------
	typedef struct ShaderID {
		GLuint mID;
		ShaderType mType;
		ShaderID(GLuint id = 0u, ShaderType type = ShaderType::UNSPECIFIED) : mID(id), mType(type) { ; }
	} ShaderID;


	//-------------------------------
	//  Abstract base class for compiled shaders
	//-------------------------------
	class CompiledShader {
	public:
		//-------------------------------
		// Constructors / Destructor
		//-------------------------------
		CompiledShader(const char * sourceFilepath);
		CompiledShader(const CompiledShader& that) = delete; //No copying from abstract base type
		CompiledShader(CompiledShader&& that) = delete; //No moving an abstract base type

		virtual ~CompiledShader();

		//-------------------------------
		// Operators
		//-------------------------------
		CompiledShader& operator=(const CompiledShader& that) = delete; //No copying from abstract base type
		CompiledShader& operator=(CompiledShader&& that) = delete; //No moving an abstract base type

		//Compares ShaderTypes and Filepaths for equality
		bool operator==(const CompiledShader&) const;
		//Compares ShaderTypes and Filepaths for inequality
		bool operator!=(const CompiledShader&) const;

		//-------------------------------
		//Public Interface Functions  
		//-------------------------------
		//Give this object a filepath if it doesn't currently have one. 
		//void giveFilepath(const char * filepath);


		//Decomission marks a shader as no-longer needed for more programs and releases this objects resources.
		//After being decommisioned, a shader must be reinstated in order to be attached to new shader programs.
		void decommision();  //Alternative function names: abolish(), deactivate(), unload(), 


		//Reinstate recompiles the shader and allows it to be attached to ShaderPrograms again. Could change this
		//objects GL-Context-Assigned ID Number  (I am unsure if this will invalidate old shaders still bound with the old ID number of this shader?)
		virtual void reinstate() = 0; //Alternative function names: reestablish(), restore(), rehabilitate(), revive(), reload(), recompile(),

		//-------------------------------
		//Member field getters
		//-------------------------------
		const char * getFilepath() const { return mFilepath; }
		bool seeIfIsDecomissioned() const { return mIsDecomissioned; }
		bool readyToBeAttached() const { return mReadyToBeAttached; }
		bool validFilepath() const { return mValidFilepath; }
		bool isCompiled() const;
		bool error() const { return mError; }
		ShaderType type() const { return mShaderID.mType; }
		GLuint shaderID() const { return mShaderID.mID; }


	protected:
		ShaderID mShaderID;
		bool mIsDecomissioned;
		const char * mFilepath;
		std::unique_ptr<std::string> mSourceText;
		bool mError;
		//These next 2 variables should really be 'private', but are protected to allow for derived objects to move themselves properly
		bool mReadyToBeAttached;
		bool mValidFilepath;


		//ProtectedDefaultConstructor
		CompiledShader(); //Default constructor should only be called by derived types since it will create an invalid object

		//Protected functions:
		bool compile();
		virtual void aquireShaderID() = 0; //This function is to be overwritten by the derived types

		//Loads the text of a file located at filepath. Useful for loading shaders
		bool loadSourceFile();

	private:
		//Private functions
		void initialize();
		bool checkIfValidFilepath(std::ifstream& inFileStream);
		bool checkForCompilationErrors();

	};

}  //namespace ShaderInterface

#endif //COMPILED_SHADER_2_H_