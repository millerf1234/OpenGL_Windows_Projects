//Class that handles loading and compiling a Tesselation Control shader source file.
//Intended to contain just a single shader source file.
//
//File:        TesselationControlShader.h
//Class:       TesselationControlShader
//Base class:  CompiledShader
//
// Code was written by Forrest Miller on July 26, 2018

#pragma once

#ifndef TESSELATION_CONTROL_SHADER_H_
#define TESSELATION_CONTROL_SHADER_H_

#include "ProjectConstants.h"
#include "ProjectParameters.h"
#include "CompiledShader.h"

namespace ShaderInterface {

	class TesselationControlShader final : public CompiledShader {
	public:
		TesselationControlShader(const char *);
		TesselationControlShader(const TesselationControlShader&);
		TesselationControlShader(TesselationControlShader&&);
		TesselationControlShader(const CompiledShader&) = delete; //This delete the move constructor as well
		virtual ~TesselationControlShader();

		//Decomissions this Tesselation Control shader object
		virtual void reinstate() override;


		TesselationControlShader& operator=(const TesselationControlShader&);
		TesselationControlShader& operator=(TesselationControlShader&&);
		TesselationControlShader& operator=(const CompiledShader&) = delete;

	protected:
		virtual void aquireShaderID() override;

	private:

	};

} //namespace ShaderInterface


#endif //TESSELATION_CONTROL_SHADER_H_