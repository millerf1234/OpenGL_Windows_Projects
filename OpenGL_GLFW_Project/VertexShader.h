//Class that represents a compiled vertex shader. Inherits from CompiledShader
//Created by Forrest Miller on July 24, 2018

#pragma once

#ifndef VERTEX_SHADER_H_
#define VERTEX_SHADER_H_


#include "CompiledShader.h"

namespace ShaderInterface {

	class VertexShader : public CompiledShader {
	public:
		VertexShader();
		~VertexShader();

	private:

	};



} //namespace ShaderInterface 


#endif VERTEX_SHADER_H_