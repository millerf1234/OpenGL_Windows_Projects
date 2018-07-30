//Shader Program that is composed of a set of compiled shaders. 
//This class is essentially a wrapper for a glShaderProgram
//Created by Forrest Miller on July 24, 2018

#pragma once

#ifndef SHADER_PROGRAM_H_
#define SHADER_PROGRAM_H_

#include "UniformLocationTracker.h"
#include "ComputeShader.h"
#include "VertexShader.h"
#include "GeometryShader.h"
#include "TesselationControlShader.h"
#include "TesselationEvaluationShader.h"
#include "FragmentShader.h"


//namespace ShaderInterface {

//see: https://www.khronos.org/opengl/wiki/GLAPI/glGetAttachedShaders

		class ShaderProgram {
		public:
			ShaderProgram();
			~ShaderProgram();
			
			//Allow moving but disallow copying
			ShaderProgram(const ShaderProgram&) = delete;
			ShaderProgram(ShaderProgram&&); 
			ShaderProgram& operator=(const ShaderProgram&) = delete;
			ShaderProgram& operator=(ShaderProgram&&);

			//Interface functions
			bool attachVert(const char * vert);
			void attachVert(const ShaderInterface::VertexShader * vert);
			bool attachGeom(const char * geom);
			void attachGeom(const ShaderInterface::GeometryShader * geom);
			bool attachTess(const char * tesse, const char * tessc) { return(attachTesse(tesse) && attachTessc(tessc)); }
			void attachTess(const ShaderInterface::TesselationControlShader* tessc, const ShaderInterface::TesselationEvaluationShader* tesse);
			bool attachTesse(const char * tesse);
			void attachTesse(const ShaderInterface::TesselationEvaluationShader * tesse);
			bool attachTessc(const char * tessc);
			void attachTessc(const ShaderInterface::TesselationControlShader * tessc);
			bool attachFrag(const char * frag);
			void attachFrag(const ShaderInterface::FragmentShader * frag );
			bool attachCompute(const char * compute);
			void attachCompute(const ShaderInterface::ComputeShader * compute);

			bool checkIfLinked() const { return mLinked; }
			void link(bool cacheLocalShaders = false);


			//
			void use();

			//Returns the OpenGL-assigned ID number for this shader program
			GLuint ID() const { return mProgramID; }

			//Returns true when this object is ready for use
			bool valid() const { return mValid; }

			//Returns true when an error has occured during the lifetime of this object, rendering it's behavior as undefined
			bool error() const { return mError; }

			//The ShaderProgram will not be ready to link until this function returns true
			bool readyToLink() const { return mReadyToLink; }

			bool hasVertAttached() const { return mHasVert; }
			bool hasCachedVert() const { return (mVertexShader != nullptr); }
			bool hasGeom() const { return mHasGeom; }
			bool hasCachedGeom() const { return (mGeometryShader != nullptr); }
			bool hasTessE() const { return mHasTesse; }
			bool hasCachedTesse() const { return (mTesselationEvaluationShader != nullptr); }
			bool hasTessC() const { return mHasTessc; }
			bool hasCachedTessc() const { return (mTesselationEvaluationShader != nullptr); }
			bool hasFrag() const { return mHasFrag; }
			bool hasCachedFrag() const { return (mFragmentShader != nullptr); }
			bool hasCompute() const { return mHasCompute; }
			bool hasCachedCompute() const { return (mComputeShader != nullptr); }


			//Public variable(s):
			std::unique_ptr<ShaderInterface::UniformLocationTracker> uniforms; //Make this public? Or make private and write a whole new set of functions to interface with it 


		private:
			GLuint mProgramID; //GL ShaderProgram ID

			bool mValid;
			bool mLinked;
			bool mHasVert, mHasGeom, mHasTesse, mHasTessc, mHasFrag, mHasCompute; //These bools represent attached shaders, not cached shaders
			bool mError;
			bool mReadyToLink;
			
			std::unique_ptr<ShaderInterface::VertexShader> mVertexShader;
			std::unique_ptr<ShaderInterface::GeometryShader> mGeometryShader;
			std::unique_ptr<ShaderInterface::TesselationControlShader> mTesselationControlShader;
			std::unique_ptr<ShaderInterface::TesselationEvaluationShader> mTesselationEvaluationShader;
			std::unique_ptr<ShaderInterface::FragmentShader> mFragmentShader;
			std::unique_ptr<ShaderInterface::ComputeShader> mComputeShader;

			//Private helped functions
			void initialize();
			void initializeBooleans();
			void generateGLProgramHandle();
			void initializeUniformLocationTracker();
		};

//} //namespace ShaderInterface

#endif //#ifndef SHADER_PROGRAM_H_
