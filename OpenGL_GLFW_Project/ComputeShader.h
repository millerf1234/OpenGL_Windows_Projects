//Class: Compute Shader
//Namespace: ShaderInterface
//Programmer: Forrest Miller
//Date(s): 7/24/2018 - 7/31/2018  
//
// Description: This class inherits from CompiledShader and represents a 
//				compute shader within the OpenGL Context. Most of the 
//				implementation is shared amongst shader types and can be
//				found within the CompiledShader class. 
//
// Notes:     -Copying is not allowed but moving is allowed. Moving a
//			    Compute Shader will result in the source object being invalidated
//				(which is expected behavior and means that extra calls to
//				glDeleteShader(shaderID) are not accidentally made during object
//				destruction).
//
//           - The constructor that takes a std::string only to pass it on to 
//             the constructor that takes the traditional c_string is only legal 
//             in C++11 [Known as a Delegating Constructor]. 


#pragma once

#ifndef COMPUTE_SHADER_H_
#define COMPUTE_SHADER_H_

#include "CompiledShader.h"

namespace ShaderInterface {

	class ComputeShader final : public CompiledShader {
	public:
		ComputeShader(const char * filePath);
		ComputeShader(std::string filePath) : ComputeShader(filePath.c_str()) { ; } //Requires C++11
		ComputeShader(const ComputeShader&) = delete; //No Copying
		ComputeShader(ComputeShader&&); //Moving is okay though
		ComputeShader(const CompiledShader&) = delete; //Contruction is not allowed from CompiledShaders not of type Compute
		ComputeShader(CompiledShader&&) = delete; //This line explicitly deletes the move constructor as well for other Non-Compute CompiledShader types
		virtual ~ComputeShader() override;

		//Restores this shader if it was decomissioned 
		virtual void reinstate() override;

		ComputeShader& operator=(const ComputeShader&) = delete;
		ComputeShader& operator=(ComputeShader&&);
		ComputeShader& operator=(const CompiledShader&) = delete;
		ComputeShader& operator=(CompiledShader&&) = delete;

	protected:
		//virtual void aquireShaderID() override;

	private:
		//Helper function that is called as part of reinstate()
		void makeSureShaderSourceTextIsLoaded(); //Will reopen and parse the file if need be

	};

} //namespace ShaderInterface


#endif //COMPUTE_SHADER_H_