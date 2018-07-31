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
		VertexShader(const VertexShader&) = delete; //No Copying
		VertexShader(VertexShader&&); //Moving is okay though
		VertexShader(const CompiledShader&) = delete; //This delete the move constructor as well
		VertexShader(CompiledShader&&) = delete;
		virtual ~VertexShader();

		//Restores this shader if it was decomissioned 
		virtual void reinstate() override;


		VertexShader& operator=(const VertexShader&) = delete;
		VertexShader& operator=(VertexShader&&);
		VertexShader& operator=(const CompiledShader&) = delete;
		VertexShader& operator=(CompiledShader&&) = delete;

	protected:
		 virtual void aquireShaderID() override;

	private:
		//Helper function that is called as part of reinstate()
		bool makeSureShaderSourceTextIsLoaded();

	};

} //namespace ShaderInterface


#endif //VERTEX_SHADER_H_