//Class: GeometryShader
//Namespace: ShaderInterface
//Programmer: Forrest Miller
//Date(s): 7/24/2018 - 7/31/2018  
//
// Description: This class inherits from CompiledShader and represents a 
//				geometry shader within the OpenGL Context. Most of the 
//				implementation is shared amongst shader types and can be
//				found within the CompiledShader class. 
//
// Notes:     -Copying is not allowed but moving is allowed. Moving a
//			    Geometry Shader will result in the source object being invalidated
//				(which is expected behavior and means that extra calls to
//				glDeleteShader(shaderID) are not accidentally made during object
//				destruction).
//
//           - The constructor that takes a std::string only to pass it on to 
//             the constructor that takes the traditional c_string is only legal 
//             in C++11. 


#pragma once

#ifndef GEOMETRY_SHADER_H_
#define GEOMETRY_SHADER_H_

#include "CompiledShader.h"

namespace ShaderInterface {

	class GeometryShader final : public CompiledShader {
	public:

		GeometryShader(const char * filePath);
		GeometryShader(std::string filePath) : GeometryShader(filePath.c_str()) { ; } //Requires C++11
		GeometryShader(const GeometryShader&) = delete; //No Copying
		GeometryShader(GeometryShader&&); //Move-Constructing from an existing GeometryShader is okay though
		GeometryShader(const CompiledShader&) = delete; //Contruction is not allowed from CompiledShaders not of type Geometry
		GeometryShader(CompiledShader&&) = delete; //This line explicitly deletes the move constructor as well for other Non-Geometry CompiledShader types
		virtual ~GeometryShader() override;

		//Restores this shader if it was decomissioned 
		virtual void reinstate() override;

		GeometryShader& operator=(const GeometryShader&) = delete;
		GeometryShader& operator=(GeometryShader&&); //Moving of derived types is okay
		GeometryShader& operator=(const CompiledShader&) = delete;
		GeometryShader& operator=(CompiledShader&&) = delete;

	protected:
		//virtual void aquireShaderID() override;

	private:
		//Helper function that is called as part of reinstate()
		void makeSureShaderSourceTextIsLoaded(); //Will reopen and parse the file if need be

	};

} //namespace ShaderInterface


#endif //GEOMETRY_SHADER_H_