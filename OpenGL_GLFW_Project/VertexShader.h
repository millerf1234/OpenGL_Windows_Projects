//Class: VertexShader
//Namespace: ShaderInterface
//Programmer: Forrest Miller
//Date(s): 7/24/2018 - 7/31/2018  
//
// Description: This class inherits from CompiledShader and represents a 
//				vertex shader within the OpenGL Context. Most of the 
//				implementation is shared amongst shader types and can be
//				found within the CompiledShader class. 
//
// Notes:     -Copying is not allowed but moving is allowed. Moving a
//			    Vertex Shader will result in the source object being invalidated
//				(which is expected behavior and means that extra calls to
//				glDeleteShader(shaderID) are not accidentally made during object
//				destruction).


#pragma once

#ifndef VERTEX_SHADER_H_
#define VERTEX_SHADER_H_

#include "CompiledShader.h"

namespace ShaderInterface {

	class VertexShader final : public CompiledShader {
	public:
		VertexShader(const char * filePath);
		VertexShader(const VertexShader&) = delete; //No Copying
		VertexShader(VertexShader&&); //Moving is okay though
		VertexShader(const CompiledShader&) = delete; //This delete the move constructor as well
		VertexShader(CompiledShader&&) = delete;
		virtual ~VertexShader();

		//Restores this shader if it was decomissioned 
		virtual void reinstate() override;

		VertexShader& operator=(const VertexShader&) = delete;
		VertexShader& operator=(VertexShader&&); //Moving of derived types is okay
		VertexShader& operator=(const CompiledShader&) = delete;
		VertexShader& operator=(CompiledShader&&) = delete;

	protected:
		 virtual void aquireShaderID() override;

	private:
		//Helper function that is called as part of reinstate()
		void makeSureShaderSourceTextIsLoaded(); //Will reopen and parse the file if need be

	};

} //namespace ShaderInterface


#endif //VERTEX_SHADER_H_