//Class: TessellationControlShader
//Namespace: ShaderInterface
//Programmer: Forrest Miller
//Date(s): 7/24/2018 - 7/31/2018  
//
// Description: This class inherits from CompiledShader and represents a 
//				Tesselation control shader within the OpenGL Context. Most of the 
//				implementation is shared amongst shader types and can be
//				found within the CompiledShader class. 
//
// Notes:     -Copying is not allowed but moving is allowed. Moving a
//			    Tesselation Control Shader will result in the source object 
//              being invalidated (which is expected behavior and means that 
//              extra calls to glDeleteShader(shaderID) are not accidentally 
//              made during object destruction).
//
//            -A tesselation control shader must be paired with a tesselation
//				evaluation shader
//
//           - The constructor that takes a std::string only to pass it on to 
//             the constructor that takes the traditional c_string is only legal 
//             in C++11. 


#pragma once

#ifndef TESSELATION_CONTROL_SHADER_H_
#define TESSELATION_CONTROL_SHADER_H_

#include "CompiledShader.h"

namespace ShaderInterface {

	class TessellationControlShader final : public CompiledShader {
	public:
		TessellationControlShader(const char * filePath);
		TessellationControlShader(std::string filePath) : TessellationControlShader(filePath.c_str()) { ; } //Requires C++11
		TessellationControlShader(const TessellationControlShader&) = delete; //No Copying
		TessellationControlShader(TessellationControlShader&&); //Moving is okay though
		TessellationControlShader(const CompiledShader&) = delete; //Construction is not allowed from CompiledShaders not of type TessControl 
		TessellationControlShader(CompiledShader&&) = delete; //This explicitly deletes the move constructor as well for other CompiledShader types
		virtual ~TessellationControlShader() override;

		//Restores this shader if it was decommissioned 
		virtual void reinstate() override;

		TessellationControlShader& operator=(const TessellationControlShader&) = delete;
		TessellationControlShader& operator=(TessellationControlShader&&); //Moving of derived types is okay
		TessellationControlShader& operator=(const CompiledShader&) = delete;
		TessellationControlShader& operator=(CompiledShader&&) = delete;

	protected:
		//virtual void aquireShaderID() override;

	private:
		//Helper function that is called as part of reinstate()
		void makeSureShaderSourceTextIsLoaded(); //Will reopen and parse the file if need be

	};

} //namespace ShaderInterface


#endif //TESSELATION_CONTROL_SHADER_H_