//File:    CompiledShader.h
//Class:   CompiledShader
//Programmer: Forrest Miller
//Date(s): 7/24/2018 - 8/1/2018  (and counting)
//
//
//
//Description:		Represents an OpenGL shader program. Is expected to have CompiledShader objects 
//					attached to it and also has a UniformLocationTracker to manage updating program
//					uniforms. 
//   Usage:
//					The steps for creating a useable CompiledShader object are:
//						1) Attach Shaders     (see rules*)
//						2) Link 
//						3) call 'use()' to set this ShaderProgram to active
//
//			*(rules):  A ShaderProgram must meet the following conditions to be eligable to be linked:
//						- Must have either a ComputeShader attached or both a Vertex and a Fragment shader
//						     attached. (In the future: it might actually be possible to get by with just a vert shader?)
//						- If a ComputeShader is attached, then no other shader type can be attached, and
//						- If any shader type other than compute is attached, then a compute shader can not be attached
//						- If a tesselation control is attached, then a tesselation evaluation must also be attached,
//						      and vice-versa.
//
//
// UPDATES/Changes:
//			 September 14-15, 2018  --  Added support for attaching secondary shaders (i.e. shaders without a 'main()' function)
//										to programs so that multiple shaders of the same type can be used.
//
//Notes:   
//
//
//Errors to watch for:  (None are explicitly implemented yet)
//					Ideas for what the errors could include:
//                      --Invalid Shader Filepath
//						--Error within the Context
//						--Shader Compilation Error
//						--Attaching non-secondary Shader using an attachSecondary function
//						--Attaching a secondary shader to a primary shader spot
//						--Attaching a shader to a spot meant for shaders of a different kind
//
//

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

#include <unordered_map>  //Used to track already-attached shaders 

//namespace ShaderInterface { 

//see: https://www.khronos.org/opengl/wiki/GLAPI/glGetAttachedShaders

		class ShaderProgram {
		public:
			//-------------------------------
			// Construction/Destruction/Copying/Moving
			//-------------------------------

			ShaderProgram();
			~ShaderProgram();
			
			//Allow moving but disallow copying
			ShaderProgram(const ShaderProgram&) = delete;
			ShaderProgram(ShaderProgram&&); 
			ShaderProgram& operator=(const ShaderProgram&) = delete;
			ShaderProgram& operator=(ShaderProgram&&);

			//-------------------------------
			// Public Interface Functions
			//-------------------------------
			
			//Creates an object-local Vertex shader and attaches it to this program.
			bool attachVert(const char * vert);
			//Uses an already-created vertex shader and attachs it to this program. Does not assume control of the vertex shader.
			//(i.e. this object's mVertexShader remains nullptr)
			void attachVert(const ShaderInterface::VertexShader * vert);
			//Uses an already created vertex shader that does not contain a 'main' function and has 
			//been marked as secondary using the 'makeSecondary()' function and attaches it to this
			//shader. It is possible to attach as many secondarys as are available, though it is an
			//error to attach the same secondary twice. 
			void attachSecondaryVert(const ShaderInterface::VertexShader * vert);

			//Creates an object-local Geometry shader and attaches it to this program
			bool attachGeom(const char * geom);
			//Uses an already-created geometry shader and attachs it to this program. Does not assume control of the vertex shader.
			//(i.e. this object's mGeometryShader remains nullptr)
			void attachGeom(const ShaderInterface::GeometryShader * geom);
			//Uses an already created Geometry shader that does not contain a 'main' function and has
			//been marked as secondary using the 'makeSecondary()' function and attaches it to this
			//shader program. It is possible to attach as many secondarys as are available, though it is an
			//error to attach the same secondary twice. 
			void attachSecondaryGeom(const ShaderInterface::GeometryShader * geom);

			//Creates object-local Tesselation Compute and Evaluation shaders and attachs them to the program
			bool attachTess(const char * tesse, const char * tessc) { return(attachTesse(tesse) && attachTessc(tessc)); }
			//Uses already created Tesselation Control and Evaluation shaders and attachs them to this program. 
			//This object does not assume control of the shaders (thus leaving this objects mTesselationControlShader and
			//mTesselationEvaluationShader as nullptr)
			void attachTess(const ShaderInterface::TesselationControlShader* tessc, const ShaderInterface::TesselationEvaluationShader* tesse);
			
			//Creates an object-local Tesselation Evaluation shader and attaches it to this program
			bool attachTesse(const char * tesse);
			//Uses an already-created Tesselation Evaluation shader and attachs it to this program. Does not
			//assume control of the tesse shader. (i.e. this object's mTesselationEvaluationShader remains
			//nullptr)
			void attachTesse(const ShaderInterface::TesselationEvaluationShader * tesse);
			//Uses an already created Tesse shader that does not contain a 'main' function and has
			//been marked as secondary using the 'makeSecondary()' function and attaches it to this
			//shader program. It is possible to attach as many secondarys as are available, though it is an
			//error to attach the same secondary twice. 
			void attachSecondaryTesse(const ShaderInterface::TesselationEvaluationShader * tesse);
			
			//Creates an object-local Tesselation Control shader and attaches it to this program
			bool attachTessc(const char * tessc);
			//Uses an already-created Tesselation Control shader and attachs it to this program. Does not
			//assume control of the tessc shader. (i.e. this object's mTesselationControlShader remains
			//nullptr)
			void attachTessc(const ShaderInterface::TesselationControlShader * tessc);
			//Uses an already created Tessc shader that does not contain a 'main' function and has
			//been marked as secondary using the 'makeSecondary()' function and attaches it to this
			//shader program. It is possible to attach as many secondaries as are available, however it is an
			//error to try to attach the same secondary shader multiple times to one program. 
			void attachSecondaryTessc(const ShaderInterface::TesselationControlShader * tessc);
			
			//Creates an object-local Fragment shader and attaches it to this program
			bool attachFrag(const char * frag);
			//Uses an already-created Fragment shader and attachs it to this program. Does not assume control
			//of the Fragment shader. (i.e. this object's mFragmentShader remains nullptr)
			void attachFrag(const ShaderInterface::FragmentShader * frag );
			//Uses an already created Frag shader that does not contain a 'main' function and has
			//been marked as secondary using the 'makeSecondary()' function and attaches it to this
			//ShaderProgram. It is possible to attach as many secondaries as are available, however it is an
			//error to try to attach the same secondary shader multiple times to one program. 
			void attachSecondaryFrag(const ShaderInterface::FragmentShader * frag);
			
			//Creates an object-local Compute shader and attaches it to this program. This function should
			//only be called in no other Shaders have been attached to this object, and after calling
			//this function no additional shaders may be attached.
			bool attachCompute(const char * compute);
			//Uses an already-created Compute shader and attaches it to this program. This function should
			//only be called in no other Shaders have been attached to this object, and after calling
			//this function no additional shaders may be attached.
			void attachCompute(const ShaderInterface::ComputeShader * compute);
			//Uses an already created Compute shader that does not contain a 'main' function and has
			//been marked as secondary using the 'makeSecondary()' function and attaches it to this
			//ShaderProgram. Compute shaders have the additional requirement that they must be the only type of
			//shader attached to any ShaderProgram. Attaching a compute shader to a program
			//that already has shaders of another type attached is an error, as is trying to attach shaders
			//of a different type to a program that already has compute shaders attached.
			//It is possible to attach as many secondaries as are available, however it is an
			//error to try to attach the same secondary shader multiple times to one program. 
			void attachSecondaryCompute(const ShaderInterface::ComputeShader * compute);


			//Checks to see if this ShaderProgram is linked
			bool checkIfLinked() const { return mState.mLinked; }

			//Links this shader program object. This shader program must have either solely a Compute
			//shader or both a Vertex and Fragment shader attached before linking. Additionally,
			//using the tesselation shaders requires both a tesselation control and tesselation 
			//evaluation shader to be attached.
			void link(bool cacheLocalShaders = false);

			//Binds this ShaderProgram to the active program spot in the GL Context. This ShaderProgram
			//must be linked before this function call will work correctly.
			void use() const;

			//Allows this ShaderProgram to be called like a function with no parameters.
			void operator()() const { this->use(); } //This is probably unsafe and shouldn't be used...

			//Returns the OpenGL-assigned ID number for this shader program
			GLuint ID() const { return mProgramID; }

			//-------------------------------
			//	Object State Accessors                (Getters)
			//-------------------------------

			//Returns true when this object is ready for use
			bool valid() const { return mState.mValid; }

			//Returns true when an error has occured during the lifetime of this object, rendering it's behavior as undefined
			bool error() const { return mState.mError; }

			//The ShaderProgram will not be ready to link until this function returns true
			bool readyToLink() const { return mState.mReadyToLink; }

			bool hasVertAttached() const { return mState.mHasVert; }
			bool hasCachedVert() const { return (mVertexShader != nullptr); }
			bool hasGeom() const { return mState.mHasGeom; }
			bool hasCachedGeom() const { return (mGeometryShader != nullptr); }
			bool hasTessE() const { return mState.mHasTesse; }
			bool hasCachedTesse() const { return (mTesselationEvaluationShader != nullptr); }
			bool hasTessC() const { return mState.mHasTessc; }
			bool hasCachedTessc() const { return (mTesselationEvaluationShader != nullptr); }
			bool hasFrag() const { return mState.mHasFrag; }
			bool hasCachedFrag() const { return (mFragmentShader != nullptr); }
			bool hasCompute() const { return mState.mHasCompute; }
			bool hasCachedCompute() const { return (mComputeShader != nullptr); }

			//Gets the number of attached secondary vertex shaders to this ShaderProgram
			int getAttachedSecondaryVertCount() const { return mState.mAttachedSecondaryVertCount; }
			//Gets the number of attached secondary geometry shaders to this ShaderProgram
			int getAttachedSecondaryGeomCount() const { return mState.mAttachedSecondaryGeomCount; }
			//Gets the number of attached secondary tesse shaders to this ShaderProgram
			int getAttachedSecondaryTesseCount() const { return mState.mAttachedSecondaryTesseCount; }
			//Gets the number of attached secondary tessc shaders to this ShaderProgram
			int getAttachedSecondaryTesscCount() const { return mState.mAttachedSecondaryTesscCount; }
			//Gets the number of attached secondary fragment shaders to this ShaderProgram
			int getAttachedSecondaryFragCount() const { return mState.mAttachedSecondaryFragCount; }
			//Gets the number of attached secondary compute shaders to this ShaderProgram
			int getAttachedSecondaryComputeCount() const { return mState.mAttachedSecondaryComputeCount; }


			//Public variable(s):
			std::unique_ptr<ShaderInterface::UniformLocationTracker> uniforms; //Make this public? Or make private and write a whole new set of functions to interface with it 

		private:
			GLuint mProgramID; //GL ShaderProgram ID

			struct ProgramState {
				bool mValid;
				bool mLinked;
				bool mHasVert, mHasGeom, mHasTesse, mHasTessc, mHasFrag, mHasCompute; //These bools represent attached shaders, not cached shaders
				int mAttachedSecondaryVertCount, mAttachedSecondaryGeomCount, mAttachedSecondaryTesseCount, mAttachedSecondaryTesscCount;
				int mAttachedSecondaryFragCount, mAttachedSecondaryComputeCount;
				std::unordered_map<GLuint, std::string> mAttachedSecondaries;
				bool mError;
				bool mReadyToLink;

				//Constructor
				ProgramState() {
					mValid = false;
					mLinked = false;
					mHasVert = false;
					mHasGeom = false;
					mHasTesse = false;
					mHasTessc = false;
					mHasFrag = false;
					mHasCompute = false;
					mAttachedSecondaryVertCount = 0;
					mAttachedSecondaryGeomCount = 0;
					mAttachedSecondaryTesseCount = 0;
					mAttachedSecondaryTesscCount = 0;
					mAttachedSecondaryFragCount = 0;
					mAttachedSecondaryComputeCount = 0;
					
					mError = false;
					mReadyToLink = false;
				}
			};

			ProgramState mState;
			
			std::unique_ptr<ShaderInterface::VertexShader> mVertexShader;
			std::unique_ptr<ShaderInterface::GeometryShader> mGeometryShader;
			std::unique_ptr<ShaderInterface::TesselationControlShader> mTesselationControlShader;
			std::unique_ptr<ShaderInterface::TesselationEvaluationShader> mTesselationEvaluationShader;
			std::unique_ptr<ShaderInterface::FragmentShader> mFragmentShader;
			std::unique_ptr<ShaderInterface::ComputeShader> mComputeShader;

			

			//Private helped functions
			void initialize();
			void generateGLProgramHandle();
			void initializeUniformLocationTracker();

			bool checkToSeeIfReadyToLinkAfterAttachingVert() const;
			bool checkToSeeIfReadyToLinkAfterAttachingTesse() const;
			bool checkToSeeIfReadyToLinkAfterAttachingTessc() const;
			bool checkToSeeIfReadyToLinkAfterAttachingFrag() const;
			
			bool checkAttachedSecondariesForMatch(const ShaderInterface::ShaderID&) const;
			void addShaderToAttachedSecondaries(const ShaderInterface::CompiledShader* secondaryShader); //Do not pass nullptr to this function!
		};

//} //namespace ShaderInterface


#endif //#ifndef SHADER_PROGRAM_H_
