#if 0

#include "VertexShader1.h"

#include <utility>
#include <functional>

namespace ShaderInterface {


	GLint VertexLayoutSpecifier::getAttribLocationInProgram(GLuint programID) {
		//If program is invalid, return
		if (programID == 0)
			return -1;
		//Check to see if we have this attribute location cached for this programID
		if (programAttributeLocations.size() > 0) {
			std::map<GLuint, GLint>::iterator it;
			it = programAttributeLocations.find(programID);
			if (it != programAttributeLocations.end())
				return it->second;
		}
		//So if it wasn't in the cache, then we need to aquire it from the OpenGL context and cache it
		GLint attribLocation = glGetAttribLocation(programID, attributeNameInShader);
		programAttributeLocations.try_emplace( programID, attribLocation);
		
		

	}

	VertexShader::VertexShader() {
	}


	VertexShader::~VertexShader() {
	}

	

	void VertexShader::compile() {
		
	}


	void VertexShader::initializeSelfAsVertShader() {

	}


} //namespace ShaderInterface 


#endif //"#if 0"