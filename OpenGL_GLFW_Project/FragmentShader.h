//Class that handles loading and compiling a fragment shader source file.
//Intended to contain just a single shader source file.
//
//File:        FragmentShader.h
//Class:       FragmentShader
//Base class:  CompiledShader
//
// Code was written by Forrest Miller on July 26, 2018

#pragma once

#ifndef FRAGMENT_SHADER_H_
#define FRAGMENT_SHADER_H_

#include "ProjectConstants.h"
#include "ProjectParameters.h"
#include "CompiledShader.h"

namespace ShaderInterface {

	class FragmentShader final : public CompiledShader {
	public:
		FragmentShader(const char *);
		FragmentShader(const FragmentShader&);
		FragmentShader(FragmentShader&&);
		FragmentShader(const CompiledShader&) = delete; //This delete the move constructor as well
		virtual ~FragmentShader();
		
		//Decomissions this frag shader object
		virtual void decommision() override;
		virtual void reinstate() override;


		FragmentShader& operator=(const FragmentShader&);
		FragmentShader& operator=(FragmentShader&&);
		FragmentShader& operator=(const CompiledShader&) = delete;

	protected:
		virtual void aquireShaderID() override;

	private:
		
	};

} //namespace ShaderInterface







#endif //FRAGMENT_SHADER_H_