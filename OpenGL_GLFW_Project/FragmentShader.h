//Class: Fragment Shader
//Namespace: ShaderInterface
//Programmer: Forrest Miller
//Date(s): 7/24/2018 - 7/31/2018  
//
// Description: This class inherits from CompiledShader and represents a 
//				fragment shader within the OpenGL Context. Most of the 
//				implementation is shared amongst shader types and can be
//				found within the CompiledShader class. 
//
// Notes:     -Copying is not allowed but moving is allowed. Moving a
//			    FragmentShader will result in the source object being invalidated
//				(which is expected behavior and means that extra calls to
//				glDeleteShader(shaderID) are not accidentally made during object
//				destruction).


#pragma once

#ifndef FRAGMENT_SHADER_H_
#define FRAGMENT_SHADER_H_

#include "CompiledShader.h"

namespace ShaderInterface {

	class FragmentShader final : public CompiledShader {
	public:
		FragmentShader(const char * filePath);
		FragmentShader(const FragmentShader&) = delete; //No Copying
		FragmentShader(FragmentShader&&); //Moving is okay though
		FragmentShader(const CompiledShader&) = delete; //This delete the move constructor as well
		FragmentShader(CompiledShader&&) = delete;
		virtual ~FragmentShader();

		//Restores this shader if it was decomissioned 
		virtual void reinstate() override;

		FragmentShader& operator=(const FragmentShader&) = delete;
		FragmentShader& operator=(FragmentShader&&); //Moving of derived types is okay
		FragmentShader& operator=(const CompiledShader&) = delete;
		FragmentShader& operator=(CompiledShader&&) = delete;

	protected:
		virtual void aquireShaderID() override;

	private:
		//Helper function that is called as part of reinstate()
		bool makeSureShaderSourceTextIsLoaded(); //Will reopen and parse the file if need be

	};

} //namespace ShaderInterface


#endif //FRAGMENT_SHADER_H_