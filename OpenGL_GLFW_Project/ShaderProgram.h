//Shader Program that is composed of a set of compiled shaders. 
//This class is essentially a wrapper for a glShaderProgram

#pragma once

#ifndef SHADER_PROGRAM_H_
#define SHADER_PROGRAM_H_

#include "UniformLocationTracker.h"

namespace ShaderInterface {

		class ShaderProgram {
		public:
			ShaderProgram();
			~ShaderProgram();

			GLuint ID() const { return mID; }


			std::unique_ptr<UniformLocationTracker> uniformLocations; //Make this public? 
		private:
			
			GLuint mID;
		};




} //namespace ShaderInterface

#endif //#ifndef SHADER_PROGRAM_H_
