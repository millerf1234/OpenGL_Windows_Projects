//Abstract base class that the 5 shader types derive from.
//
// Thoughts:
//		It may be possible that this base class is over-complicated and yet also
//		at the same time doesn't go far enough. I feel like I set things up so that
//		these classes do a lot of their tracking of their state on their own, which
//		instead could be done largly by the OpenGL context since it is doing the work
//		anyways. For example, see this link: //see: https://www.khronos.org/opengl/wiki/GLAPI/glGetShader
//		See how many queries are available there? It's too soon for me to want to rewrite all these
//		classes but I believe there is a cleaner, simpler implementation waiting to be coded.
//
//		One thought I had was to condense the CompiledShader class down into just the bare minimum,
//		and then have a second 'wrapper' class that provides the extra needed tracking/functionality.
//		Here's what that might have looked like:
/* //////////////////////////////////////////////////////////////////////////////////////////////////////
//(Thoughts on ShaderID): Use a std::pair (or std::tuple<>) instead of a struct? //see: http://www.cplusplus.com/reference/tuple/
typedef struct ShaderID {
GLuint mID;
ShaderType mType;
ShaderID(GLuint id, ShaderType type) : mID(id), mType(type) { ; }

bool verifyIsShader() const {
	if (mID != 0u) //0u can't be assigned to a valid shader
		return glIsShader(mID);
	return false;
}
GLint getType() const {
	GLint shaderType;
	glGetShaderiv(mID, GL_SHADER_TYPE, &shaderType);
	return shaderType;
}
//bool verifyType() const { //see: https://www.khronos.org/opengl/wiki/GLAPI/glGetShader
//	if (verifyIsShader() && !(mType == ShaderType::UNSPECIFIED)) {
//		GLint shaderType;
//		glGetShaderiv(mID, GL_SHADER_TYPE, &shaderType);
//	}
//	return GL_VERTEX_SHADER;
//}
} ShaderID;
//////////////////////////////////////////////////////////////////////////////////////////////////////
// */
//
//
// Created by Forrest Miller over the days 7/24/2018 to 7/26/2018

#pragma once

#ifndef COMPILED_SHADER_OLD_H_
#define COMPILED_SHADER_OLD_H_

#include <fstream>
#include <list>

#include "ProjectConstants.h"
#include "ProjectParameters.h"

namespace ShaderInterface {
	constexpr size_t SHADER_COMPILATION_INFO_LOG_BUFFER_SIZE = 768ull;

	
	enum class ShaderType {VERTEX, GEOMETRY, TESSELATION_CONTROL, TESSELATION_EVALUATION, FRAGMENT, COMPUTE, UNSPECIFIED};

	class CompiledShader_OLD {
	public:
		CompiledShader_OLD() = delete; //Filepath required to construct
		CompiledShader_OLD(const char * filepath);
		CompiledShader_OLD(const CompiledShader_OLD &) = default;
		CompiledShader_OLD(CompiledShader_OLD&&) = default;
		virtual ~CompiledShader_OLD();

		//-------------------------------
		//Public Interface Functions  
		//-------------------------------
		//Decomission marks a shader as no-longer needed for more programs and releases this objects resources.
		//After being decommisioned, a shader must be reinstated in order to be attached to new shader programs.
		virtual void decommision();  //Alternative function names: abolish(), deactivate(), unload(), 
		
		//Reinstate recompiles the shader and allows it to be attached to ShaderPrograms again. Could change this
		//objects GL-Context-Assigned ID Number  (I am unsure if this will invalidate old shaders still bound with the old ID number of this shader?)
		virtual void reinstate() = 0; //Alternative function names: reestablish(), restore(), rehabilitate(), revive(), reload(), recompile(),
		
		
		//-------------------------------
		//Operators
		//-------------------------------
		CompiledShader_OLD& operator=(const CompiledShader_OLD&) = default;
		CompiledShader_OLD& operator=(CompiledShader_OLD&&) = default;
		//Compares ShaderTypes and Filepaths for equality
		bool operator==(const CompiledShader_OLD&) const;
		//Compares ShaderTypes and Filepaths for inequality
		bool operator!=(const CompiledShader_OLD&) const;


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

#endif //COMPILED_SHADER_OLD_H_