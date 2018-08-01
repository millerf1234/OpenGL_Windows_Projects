//Class: TesselationControlShader
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


#pragma once

#ifndef TESSELATION_CONTROL_SHADER_H_
#define TESSELATION_CONTROL_SHADER_H_

#include "CompiledShader.h"

namespace ShaderInterface {

	class TesselationControlShader final : public CompiledShader {
	public:
		TesselationControlShader(const char * filePath);
		TesselationControlShader(const TesselationControlShader&) = delete; //No Copying
		TesselationControlShader(TesselationControlShader&&); //Moving is okay though
		TesselationControlShader(const CompiledShader&) = delete; //This delete the move constructor as well
		TesselationControlShader(CompiledShader&&) = delete;
		virtual ~TesselationControlShader();

		//Restores this shader if it was decomissioned 
		virtual void reinstate() override;

		TesselationControlShader& operator=(const TesselationControlShader&) = delete;
		TesselationControlShader& operator=(TesselationControlShader&&); //Moving of derived types is okay
		TesselationControlShader& operator=(const CompiledShader&) = delete;
		TesselationControlShader& operator=(CompiledShader&&) = delete;

	protected:
		//virtual void aquireShaderID() override;

	private:
		//Helper function that is called as part of reinstate()
		void makeSureShaderSourceTextIsLoaded(); //Will reopen and parse the file if need be

	};

} //namespace ShaderInterface


#endif //TESSELATION_CONTROL_SHADER_H_