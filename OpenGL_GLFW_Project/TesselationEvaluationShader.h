//Class that handles loading and compiling a Tesselation Evaluation shader source file.
//Intended to contain just a single shader source file.
//
//File:        TesselationEvaluationShader.h
//Class:       TesselationEvaluationShader
//Base class:  CompiledShader
//
// Code was written by Forrest Miller on July 26, 2018

#pragma once

#ifndef TESSELATION_EVALUATION_SHADER_H_
#define TESSELATION_EVALUATION_SHADER_H_

#include "ProjectConstants.h"
#include "ProjectParameters.h"
#include "CompiledShader.h"

namespace ShaderInterface {

	class TesselationEvaluationShader final : public CompiledShader {
	public:
		TesselationEvaluationShader(const char *);
		TesselationEvaluationShader(const TesselationEvaluationShader&);
		TesselationEvaluationShader(TesselationEvaluationShader&&);
		TesselationEvaluationShader(const CompiledShader&) = delete; //This delete the move constructor as well
		virtual ~TesselationEvaluationShader();

		virtual void reinstate() override;


		TesselationEvaluationShader& operator=(const TesselationEvaluationShader&);
		TesselationEvaluationShader& operator=(TesselationEvaluationShader&&);
		TesselationEvaluationShader& operator=(const CompiledShader&) = delete;

	protected:
		virtual void aquireShaderID() override;

	private:

	};

} //namespace ShaderInterface


#endif //TESSELATION_EVALUATION_SHADER_H_