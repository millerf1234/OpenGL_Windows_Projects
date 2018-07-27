//Class that handles loading and compiling a geometry shader source file.
//Intended to contain just a single shader source file.
//
// For documentation of Geometry shaders, see: https://www.khronos.org/opengl/wiki/Geometry_Shader
//A geometry shader can output to multiple rendertargets for transformation feedback
//
//
//File:        GeometryShader.h
//Class:       GeometryShader
//Base class:  CompiledShader
//
// Code was written by Forrest Miller on July 26, 2018

#pragma once

#ifndef GEOMETRY_SHADER_H_
#define GEOMETRY_SHADER_H_

#include "ProjectConstants.h"
#include "ProjectParameters.h"
#include "CompiledShader.h"

namespace ShaderInterface {

	class GeometryShader final : public CompiledShader {
	public:
		GeometryShader(const char *);
		GeometryShader(const GeometryShader&);
		GeometryShader(GeometryShader&&);
		GeometryShader(const CompiledShader&) = delete; //This delete the move constructor as well
		virtual ~GeometryShader();

		//Decomissions this geom shader object
		virtual void decommision() override;
		virtual void reinstate() override;


		GeometryShader& operator=(const GeometryShader&);
		GeometryShader& operator=(GeometryShader&&);
		GeometryShader& operator=(const CompiledShader&) = delete;

	protected:
		virtual void aquireShaderID() override;

	private:
		
	};

} //namespace ShaderInterface


#endif //GEOMETRY_SHADER_H_