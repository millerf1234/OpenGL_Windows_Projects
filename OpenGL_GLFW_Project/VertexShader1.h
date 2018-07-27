//Class that represents a compiled vertex shader. Inherits from CompiledShader
//Created by Forrest Miller on July 24, 2018

#pragma once

#ifndef VERTEX_SHADER_H_
#define VERTEX_SHADER_H_

#include <vector>
#include <map>

#include "CompiledShader.h"
//#include "VertexAttribPointerData.h"


namespace ShaderInterface {




#if 0
	//static constexpr int ATTRIBUTE_NAME_BUFFER_SIZE = 256; //DELETE THIS?

	//I am rewriting the class again below, in a more clean fashion
//	class VertexShader : public CompiledShader {
//	public:
//		VertexShader() = delete;
//		VertexShader(const char *);
//
//		virtual ~VertexShader(); 
//
//		/*bool hasPositionAttribPointerData() const;
//		std::shared_ptr<VertexAttribPointerData> getPositionAttribPointerData();
//		bool hasColorAttribPointerData() const;
//		std::shared_ptr<VertexAttribPointerData> getColorAttribPointerData();
//		bool hasTextureAttribPointerData() const;
//		std::shared_ptr<VertexAttribPointerData> getTextureAttribPointerData();
//		bool hasNormalAttribPointerData() const;
//		std::shared_ptr<VertexAttribPointerData> getNormalAttribPointerData();*/
//
//		GLchar * getVertexAttribName() const;
//		GLchar * getColorAttribName();
//
//		virtual void compile() override;
//
//	private:
//		//InputAttributeNames
//		GLchar vertexAttribName[256];
//		GLchar colorAttribName[256];
//		GLchar textureAttribName[256];
//		GLchar normalAttribName[256];
//
//		//Pointers to AttribPointerData structures
//		//std::shared_ptr<VertexAttribPointerData> positionAttribPointerData;
//		//std::shared_ptr<VertexAttribPointerData> colorAttribPointerData;
//		//std::shared_ptr<VertexAttribPointerData> textureAttribPointerData;
//		//std::shared_ptr<VertexAttribPointerData> normalAttribPointerData;
//
//
//
//	};
//
//
//
//} //namespace ShaderInterface 
//
//
//#endif VERTEX_SHADER_H_

	//Again, this one has too many pieces to be just a vertex shader...
	typedef struct VertexLayoutSpecifier {
	public:
		GLchar attributeNameInShader[ATTRIBUTE_NAME_BUFFER_SIZE]; //the variable name for a in parameter
		VertexAttribPointerType dataType;
		GLint attribSize; //Must be value in the set {x in Z | x > 1 and x < 4}

		VertexLayoutSpecifier() = delete;
		VertexLayoutSpecifier(GLint size, const GLchar * variableName = "\0", VertexAttribPointerType typeOfData = VertexAttribPointerType::FLOAT) : dataType(typeOfData) {
			if ((size < 1) || (size > 4)) {
				fprintf(WRNLOG, "\nWARNING! A VertexLayoutSpecifier was created with a size that was not in the range of 1-4!\n");
				fprintf(WRNLOG, "The size requested was %d but will instead be defaulted to 4!\n", size);
			}
			GLchar * ptrToName = attributeNameInShader;
			short stringLengthCounter = 0;
			while (*ptrToName++ = *variableName++) { //Loop condition does a string copy
				stringLengthCounter++;
				if (stringLengthCounter > ATTRIBUTE_NAME_BUFFER_SIZE)
					break;
			}
		}
		GLint getAttribLocationInProgram(GLuint); 
		GLint refreshAttribLocationInProgram(GLuint); //In case an attribute locations gets updated for a programID number, this forces a stored location to be reaquired from OpenGL
	private:
		std::map<GLuint, GLint> programAttributeLocations; //<ProgramID, Attribute location>   //This way it is possible to recover a location per program if this shader is used in multiple programs
	} VertexLayoutSpecifier;

	class VertexShader : public CompiledShader {
	public:
		VertexShader() = delete; //Need a filePath to create
		VertexShader(const char *);

		virtual ~VertexShader();

		virtual void compile() override;
		//void addVertexLayoutSpecifier(const GLchar* variableName, )
		//
			//Vertex shader specific functions
		//	std::vector<VertexLayoutSpecifier> getVertexInputSpecifiers() const;

		

	private:
		std::vector<VertexLayoutSpecifier> mVertexInputSpecifiers;

		//This function should only be called by a constructor
		//void initializeSelfAsVertShader() final; 



	};
#endif //#if 0


} //namespace ShaderInterface 


#endif VERTEX_SHADER_H_

