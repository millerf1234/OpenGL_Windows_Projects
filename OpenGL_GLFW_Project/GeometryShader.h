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


#pragma once

#ifndef GEOMETRY_SHADER_H_
#define GEOMETRY_SHADER_H_

#include "CompiledShader.h"

namespace ShaderInterface {

	class GeometryShader final : public CompiledShader {
	public:
		GeometryShader(const char * filePath);
		GeometryShader(const GeometryShader&) = delete; //No Copying
		GeometryShader(GeometryShader&&); //Moving is okay though
		GeometryShader(const CompiledShader&) = delete; //This delete the move constructor as well
		GeometryShader(CompiledShader&&) = delete;
		virtual ~GeometryShader();

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