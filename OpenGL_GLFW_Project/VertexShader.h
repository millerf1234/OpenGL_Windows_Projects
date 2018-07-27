//Class that handles loading and compiling a vertex shader source file.
//Intended to contain just a single shader source file.
//
//File:        VertexShader.h
//Class:       VertexShader
//Base class:  CompiledShader
//
// Code was written by Forrest Miller on July 26, 2018

#pragma once

#ifndef VERTEX_SHADER_H_
#define VERTEX_SHADER_H_

#include "ProjectConstants.h"
#include "ProjectParameters.h"
#include "CompiledShader.h"

namespace ShaderInterface {

	class VertexShader final : public CompiledShader {
	public:
		VertexShader(const char *);
		VertexShader(const VertexShader&);
		VertexShader(VertexShader&&);
		VertexShader(const CompiledShader&) = delete; //This delete the move constructor as well
		virtual ~VertexShader();

		//Decomissions this vertex shader object
		virtual void decommision() override;
		//Restores the decomissioned shader
		virtual void reinstate() override;


		VertexShader& operator=(const VertexShader&);
		VertexShader& operator=(VertexShader&&);
		VertexShader& operator=(const CompiledShader&) = delete;

	protected:
		 virtual void aquireShaderID() override;

	private:

	};

} //namespace ShaderInterface


#endif //VERTEX_SHADER_H_