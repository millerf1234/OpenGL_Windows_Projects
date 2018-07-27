//Class that handles loading and compiling a compute shader source file.
//Intended to contain just a single shader source file.
//
//
//File:        ComputeShader.h
//Class:       ComputeShader
//Base class:  CompiledShader
//
// Code was written by Forrest Miller on July 26, 2018

#pragma once

#ifndef COMPUTE_SHADER_H_
#define COMPUTE_SHADER_H_

#include "ProjectConstants.h"
#include "ProjectParameters.h"
#include "CompiledShader.h"

namespace ShaderInterface {

	class ComputeShader final : public CompiledShader {
	public:
		ComputeShader(const char *);
		ComputeShader(const ComputeShader&);
		ComputeShader(ComputeShader&&);
		ComputeShader(const ComputeShader&) = delete; //This delete the move constructor as well
		virtual ~ComputeShader();

		//Decomissions this compute shader object
		virtual void decommision() override;
		//Reinstate's the shader to allow for attachment to programs again. ShaderID may be different than this objects previous ShaderIDs
		virtual void reinstate() override;


		ComputeShader& operator=(const ComputeShader&);
		ComputeShader& operator=(ComputeShader&&);
		ComputeShader& operator=(const CompiledShader&) = delete;

	protected:
		virtual void aquireShaderID() override;

	private:

	};

} //namespace ShaderInterface


#endif //COMPUTE_SHADER_H_