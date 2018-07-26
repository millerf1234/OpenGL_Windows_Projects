//Class that represents a compiled vertex shader. Inherits from CompiledShader
//Created by Forrest Miller on July 24, 2018

#pragma once

#ifndef VERTEX_SHADER_H_
#define VERTEX_SHADER_H_


#include "CompiledShader.h"
#include "VertexAttribPointerData.h"


namespace ShaderInterface {

	class VertexShader : public CompiledShader {
	public:
		VertexShader();
		VertexShader(const char *);

		virtual ~VertexShader(); 

		bool hasPositionAttribPointerData() const;
		std::shared_ptr<VertexAttribPointerData> getPositionAttribPointerData();
		bool hasColorAttribPointerData() const;
		std::shared_ptr<VertexAttribPointerData> getColorAttribPointerData();
		bool hasTextureAttribPointerData() const;
		std::shared_ptr<VertexAttribPointerData> getTextureAttribPointerData();
		bool hasNormalAttribPointerData() const;
		std::shared_ptr<VertexAttribPointerData> getNormalAttribPointerData();

		GLchar * getVertexAttribName() const;
		GLchar * getColorAttribName();

		virtual void compile() override;

	private:
		//InputAttributeNames
		GLchar vertexAttribName[256];
		GLchar colorAttribName[256];
		GLchar textureAttribName[256];
		GLchar normalAttribName[256];

		//Pointers to AttribPointerData structures
		std::shared_ptr<VertexAttribPointerData> positionAttribPointerData;
		std::shared_ptr<VertexAttribPointerData> colorAttribPointerData;
		std::shared_ptr<VertexAttribPointerData> textureAttribPointerData;
		std::shared_ptr<VertexAttribPointerData> normalAttribPointerData;



	};



} //namespace ShaderInterface 


#endif VERTEX_SHADER_H_


/*

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
virtual CompiledShader& operator=(CompiledShader &&) = 0;

void deleteShader();

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


*/