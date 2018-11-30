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
//
//           - The constructor that takes a std::string only to pass it on to 
//             the constructor that takes the traditional c_string is only legal 
//             in C++11 [Known as a Delegating Constructor]. 


#pragma once

#ifndef FRAGMENT_SHADER_H_
#define FRAGMENT_SHADER_H_

#include "CompiledShader.h"

namespace ShaderInterface {

	class FragmentShader final : public CompiledShader {
	public:
		FragmentShader(const char * filePath);
		FragmentShader(std::string filePath) : FragmentShader(filePath.c_str()) { ; } //Requires C++11
		FragmentShader(const FragmentShader&) = delete; //No Copying
		FragmentShader(FragmentShader&&); //Moving is okay though
		FragmentShader(const CompiledShader&) = delete; //Contruction is not allowed from CompiledShaders not of type Fragment
		FragmentShader(CompiledShader&&) = delete; //This line explicitly deletes the move constructor as well for other Non-Fragment CompiledShader types
		virtual ~FragmentShader() override;

		//Restores this shader if it was decomissioned 
		virtual void reinstate() override;

		FragmentShader& operator=(const FragmentShader&) = delete;
		FragmentShader& operator=(FragmentShader&&); //Moving of derived types is okay
		FragmentShader& operator=(const CompiledShader&) = delete;
		FragmentShader& operator=(CompiledShader&&) = delete;

	protected:
		//virtual void aquireShaderID() override;

	private:
		//Helper function that is called as part of reinstate()
		void makeSureShaderSourceTextIsLoaded(); //Will reopen and parse the file if need be

	};

} //namespace ShaderInterface


#endif //FRAGMENT_SHADER_H_