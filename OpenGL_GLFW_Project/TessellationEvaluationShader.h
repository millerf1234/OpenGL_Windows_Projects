//Class: TessellationEvaluationShader
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
//
//           - The constructor that takes a std::string only to pass it on to 
//             the constructor that takes the traditional c_string is only legal 
//             in C++11. 


#pragma once

#ifndef TESSELATION_EVALUATION_SHADER_H_
#define TESSELATION_EVALUATION_SHADER_H_

#include "CompiledShader.h"

namespace ShaderInterface {

	class TessellationEvaluationShader final : public CompiledShader {
	public:
		TessellationEvaluationShader(const char * filePath);
		TessellationEvaluationShader(std::string filePath) : TessellationEvaluationShader(filePath.c_str()) { ; }  //Requires C++11
		TessellationEvaluationShader(const TessellationEvaluationShader&) = delete; //No Copying
		TessellationEvaluationShader(TessellationEvaluationShader&&); //Moving is okay though
		TessellationEvaluationShader(const CompiledShader&) = delete;  //Construction is not allowed from CompiledShaders not of type TessEval  
		TessellationEvaluationShader(CompiledShader&&) = delete; //This explicitly deletes the move constructor as well for other CompiledShader types
		virtual ~TessellationEvaluationShader() override = default;

		//Restores this shader if it was decommissioned 
		virtual void reinstate() override;

		TessellationEvaluationShader& operator=(const TessellationEvaluationShader&) = delete;
		TessellationEvaluationShader& operator=(TessellationEvaluationShader&&); //Moving of derived types is okay
		TessellationEvaluationShader& operator=(const CompiledShader&) = delete;
		TessellationEvaluationShader& operator=(CompiledShader&&) = delete;

	protected:
		//virtual void aquireShaderID() override;

	private:
		//Helper function that is called as part of reinstate()
		void makeSureShaderSourceTextIsLoaded(); //Will reopen and parse the file if need be

	};

} //namespace ShaderInterface


#endif //TESSELATION_EVALUATION_SHADER_H_