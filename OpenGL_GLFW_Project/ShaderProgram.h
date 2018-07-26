//Shader Program that is composed of a set of compiled shaders. 
//This class is essentially a wrapper for a glShaderProgram
//Created by Forrest Miller on July 24, 2018

#pragma once

#ifndef SHADER_PROGRAM_H_
#define SHADER_PROGRAM_H_

#include "UniformLocationTracker.h"
#include "VertexShader.h"


//namespace ShaderInterface {

//see: https://www.khronos.org/opengl/wiki/GLAPI/glGetAttachedShaders

		class ShaderProgram {
		public:
			ShaderProgram();
			~ShaderProgram();


			

			//Returns the OpenGL-assigned ID number for this shader program
			GLuint ID() const { return mID; }

			//Returns true when this object is ready for use
			bool valid() const { return mValid; }

			//Returns true when an error has occured during the lifetime of this object, rendering it's behavior as undefined
			bool error() const { return mError; }

			//The ShaderProgram will not be ready to link until this function returns true
			bool readyToLink() const { return mReadyToLink; }

			bool hasVert() const { return mHasVert; }
			bool hasGeom() const { return mHasGeom; }
			bool hasTessE() const { return mHasTessE; }
			bool hasTessC() const { return mHasTessC; }
			bool hasFrag() const { return mHasFrag; }


			//Public variable:
			std::unique_ptr<ShaderInterface::UniformLocationTracker> uniformLocations; //Make this public? Or make private and write a whole new set of functions to interface with it 

		private:
			
			GLuint mID; //GL ShaderProgram ID

			bool mValid;
			bool mHasVert, mHasGeom, mHasTessE, mHasTessC, mHasFrag;
			bool mError;
			bool mReadyToLink;





			void initialize();

		};




//} //namespace ShaderInterface

#endif //#ifndef SHADER_PROGRAM_H_
