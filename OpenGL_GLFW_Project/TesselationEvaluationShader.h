//Class: TesselationEvaluationShader
//Namespace: ShaderInterface
//Programmer: Forrest Miller
//Date(s): 7/24/2018 - 7/31/2018  
//
// Description: This class inherits from CompiledShader and represents a 
//				Tesselation evaluation shader within the OpenGL Context. Most of the 
//				implementation is shared amongst shader types and can be
//				found within the CompiledShader class. 
//
// Notes:     -Copying is not allowed but moving is allowed. Moving a
//			    Tesselation Evaluation Shader will result in the source object 
//              being invalidated (which is expected behavior and means that 
//              extra calls to glDeleteShader(shaderID) are not accidentally 
//              made during object destruction).
//
//            -A tesselation evaluation shader must be paired with a tesselation
//				control shader


#pragma once

#ifndef TESSELATION_EVALUATION_SHADER_H_
#define TESSELATION_EVALUATION_SHADER_H_

#include "CompiledShader.h"

namespace ShaderInterface {

	class TesselationEvaluationShader final : public CompiledShader {
	public:
		TesselationEvaluationShader(const char * filePath);
		TesselationEvaluationShader(const TesselationEvaluationShader&) = delete; //No Copying
		TesselationEvaluationShader(TesselationEvaluationShader&&); //Moving is okay though
		TesselationEvaluationShader(const CompiledShader&) = delete; //This delete the move constructor as well
		TesselationEvaluationShader(CompiledShader&&) = delete;
		virtual ~TesselationEvaluationShader();

		//Restores this shader if it was decomissioned 
		virtual void reinstate() override;

		TesselationEvaluationShader& operator=(const TesselationEvaluationShader&) = delete;
		TesselationEvaluationShader& operator=(TesselationEvaluationShader&&); //Moving of derived types is okay
		TesselationEvaluationShader& operator=(const CompiledShader&) = delete;
		TesselationEvaluationShader& operator=(CompiledShader&&) = delete;

	protected:
		virtual void aquireShaderID() override;

	private:
		//Helper function that is called as part of reinstate()
		void makeSureShaderSourceTextIsLoaded(); //Will reopen and parse the file if need be

	};

} //namespace ShaderInterface


#endif //TESSELATION_EVALUATION_SHADER_H_