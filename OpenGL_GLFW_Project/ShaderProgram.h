//File:    ShaderProgram.h
//Class:   ShaderProgram
//Programmer: Forrest Miller
//Date(s): 7/24/2018 - 8/1/2018  
//
//
//
//Description:		Represents an OpenGL shader program. Is expected to have CompiledShader objects 
//					attached to it and also has a UniformLocationTracker to manage updating program
//					uniforms. 
//   Usage:
//					The steps for creating a usable CompiledShader object are:
//						1) Attach Shaders     (see rules*)
//						2) Link 
//						3) call 'use()' to set this ShaderProgram to active
//
//			*(rules):  A ShaderProgram must meet the following conditions to be eligible to be linked:
//						- Must have either a ComputeShader attached or both a Vertex and a Fragment shader
//						     attached. (In the future: it might actually be possible to get by with just a vert shader?)
//						- If a ComputeShader is attached, then no other shader type can be attached, and
//						- If any shader type other than compute is attached, then a compute shader can not be attached
//						- If a tessellation control is attached, then a tessellation evaluation must also be attached,
//						      and vice-versa.
//						- It is possible to mark CompiledShaders as secondary, which by doing so means you
//						      can guarantee the shader does not contain a 'main()' function. There are then special 
//						      functions for attaching secondary shaders to a ShaderProgram. Secondary shaders do 
//							  not count towards occupying the primary shader spot of their type (duh), and there 
//							  is (currently) no limit on the number of secondary shaders that can be attached. The
//							  same rules apply towards what types of secondary shaders can be attached as towards primary
//							  (i.e. if a compute is attached then no other type can be attached, and vice versa; must have 
//							  both types of tessellation if one is present).
//
//
//  Justification for using c_strings over C++'s std::strings:
//                      - OpenGL is a C api and as such, each string filepath used by this object will need to be 
//                        cast to a c_string to be sent to the Graphics Context. As such, it made sense to include 
//                        functions that operate with c_strings. 
//
// UPDATES/Changes:
//			 September 14-15, 2018  --  Added support for attaching secondary shaders (i.e. shaders without a 'main()' function)
//										to programs so that multiple shaders of the same type can be used.
//
//           December 3-5 (or so)  2018  -- (MAYBE) Add support for having each shader program's shaders query the operating system [using C++17/C++20's  
//                                          std::filesystem::last_write_time()] to determine if the files containing their source code
//                                          have been modified from the version they were built with, and then having them rebuild themselves from
//                                          the updated source. Note that this functionality will be disabled for both unlinked/incomplete shader programs
//                                          and for shader programs that have been released from the context (by having their member function 'released()' 
//                                          called). 
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
//  Known Issues:  It is currently possible to attach a primary shader to a shader program, then mark the shader as
//				   a secondary and reattach it using the appropriate 'attachSecondary' function. The ShaderProgram 
//				   will not be able to link if this happens, but this class will not catch it ahead of time. 
//
//


#pragma once

#ifndef SHADER_PROGRAM_H_
#define SHADER_PROGRAM_H_

#include <unordered_map>  //Used to track already-attached (secondary) shaders  

#include "UniformLocationInterface.h"

#include "ComputeShader.h"
#include "VertexShader.h"
#include "GeometryShader.h"
#include "TessellationControlShader.h"
#include "TessellationEvaluationShader.h"
#include "FragmentShader.h"
#include "FilepathWrapper.h" //Provides useful static functions for allowing dynamic rebuilding of shaders



//namespace ShaderInterface { 

//see: https://www.khronos.org/opengl/wiki/GLAPI/glGetAttachedShaders

    class ShaderProgram final {
        public:
            //-------------------------------
            // Construction/Destruction/Copying/Moving
            //-------------------------------

            ShaderProgram() noexcept;
            ~ShaderProgram() noexcept;
            
            //Allow moving but disallow copying
            ShaderProgram(const ShaderProgram&) = delete;
            ShaderProgram(ShaderProgram&&) noexcept; 
            ShaderProgram& operator=(const ShaderProgram&) = delete;
            ShaderProgram& operator=(ShaderProgram&&) noexcept;

            //-------------------------------
            // Public Interface Functions
            //-------------------------------

            //Creates an object-local Vertex shader and attaches it to this program
            inline bool attachVert(std::string vert) {  return (attachVert(vert.c_str()));  }
            //Creates an object-local Vertex shader and attaches it to this program
            bool attachVert(const char* vert);
            //Uses an already-created Vertex shader and attaches it to this program.
            //This object does not assume control/ownership of the vertex shader
            void attachVert(const ShaderInterface::VertexShader* vert);

            //Attach secondary VertexShader to this ShaderProgram. Will reject any 
            //VertexShader that has not been marked as secondary. A shader should 
            //only be marked as secondary if it does not define a 'main' function.
            //Otherwise, multiple 'main' functions defined in vertex shaders 
            //attached to this program will automatically guarantee a GLSL compiler
            //fail when program is linked.
            void attachSecondaryVert(const ShaderInterface::VertexShader* vert);


            //Creates an object-local Geometry shader and attaches it to this program
            inline bool attachGeom(std::string geom) {  return (attachGeom(geom.c_str()));  }
            //Creates an object-local Geometry shader and attaches it to this program
            bool attachGeom(const char* geom);
            //Uses an already-created geometry shader and attaches it to this program. This object does
            //not assume control of the vertex shader
            void attachGeom(const ShaderInterface::GeometryShader* geom);
            //Attach secondary GeometryShader to this ShaderProgram. Will reject any 
            //GeometryShader that has not been marked as secondary. A shader should 
            //only be marked as secondary if it does not define a 'main' function.
            //Otherwise, multiple 'main' function definitions across all attached
            //GeometryShader automatically guarantees a GLSL compiler fail when
            //program is linked.
            void attachSecondaryGeom(const ShaderInterface::GeometryShader* geom);


            //Creates object-local Tessellation Compute and Evaluation shaders and attaches them to the program
            inline bool attachTess(std::string tesse, std::string tessc) { return ((attachTesse(tesse.c_str())) && (attachTessc(tessc.c_str())));   }
            //Creates object-local Tessellation Compute and Evaluation shaders and attaches them to the program
            bool attachTess(const char* tesse, const char* tessc) { return(attachTesse(tesse) && attachTessc(tessc)); }
            //Uses already created Tessellation Control and Evaluation shaders and attaches them to this program. 
            //This object does not assume control of the shaders.
            void attachTess(const ShaderInterface::TessellationControlShader* tessc, const ShaderInterface::TessellationEvaluationShader* tesse);
            

            //Creates an object-local Tessellation Evaluation shader and attaches it to this program
            inline bool attachTesse(std::string tesse) {   return (attachTesse(tesse.c_str()));   }
            //Creates an object-local Tessellation Evaluation shader and attaches it to this program
            bool attachTesse(const char * tesse);
            //Uses an already-created Tessellation Evaluation shader and attaches it to this program. Does not
            //assume control of the tesse shader. (i.e. this object's mTessellationEvaluationShader remains
            //nullptr)
            void attachTesse(const ShaderInterface::TessellationEvaluationShader* tesse);
            //Attach secondary TesselationEvaluationShader to this ShaderProgram. Will reject any 
            //TesselationEvaluationShader to this ShaderProgram. that has not been marked as 
            //secondary. A shader should only be marked as secondary if it does not define a
            //'main' function. Otherwise, multiple 'main' function definitions across all attached
            //TesselationEvaluationShader to this ShaderProgram. automatically guarantees a GLSL
            //compiler fail when program is linked.
            void attachSecondaryTesse(const ShaderInterface::TessellationEvaluationShader* tesse);
            

            //Creates an object-local Tessellation Control shader and attaches it to this program
            inline bool attachTessc(std::string tessc) {   return (attachTessc(tessc.c_str()));   }
            //Creates an object-local Tessellation Control shader and attaches it to this program
            bool attachTessc(const char * tessc);
            //Uses an already-created Tessellation Control shader and attaches it to this program. Does not
            //assume control of the tessc shader. (i.e. this object's mTessellationControlShader remains
            //nullptr)
            void attachTessc(const ShaderInterface::TessellationControlShader* tessc);
            //Uses an already created Tessc shader that does not contain a 'main' function and has
            //been marked as secondary using the 'makeSecondary()' function and attaches it to this
            //shader program. It is possible to attach as many secondaries as are available, however it is an
            //error to try to attach the same secondary shader multiple times to one program. 
            void attachSecondaryTessc(const ShaderInterface::TessellationControlShader* tessc);
            

            //Creates an object-local Fragment shader and attaches it to this program.
            inline bool attachFrag(std::string frag) {   return (attachFrag(frag.c_str()));   }
            //Creates an object-local Fragment shader and attaches it to this program
            bool attachFrag(const char * frag);
            //Uses an already-created Fragment shader and attaches it to this program. Does not assume control
            //of the Fragment shader. (i.e. this object's mFragmentShader remains nullptr)
            void attachFrag(const ShaderInterface::FragmentShader* frag );
            //Uses an already created Frag shader that does not contain a 'main' function and has
            //been marked as secondary using the 'makeSecondary()' function and attaches it to this
            //ShaderProgram. It is possible to attach as many secondaries as are available, however it is an
            //error to try to attach the same secondary shader multiple times to one program. 
            void attachSecondaryFrag(const ShaderInterface::FragmentShader* frag);
            

            //Creates an object-local Compute shader and attaches it to this program. This function should
            //only be called in no other Shaders have been attached to this object, and after calling
            //this function no additional shaders may be attached.
            inline bool attachCompute(std::string compute) {   return (attachCompute(compute.c_str()));   }
            //Creates an object-local Compute shader and attaches it to this program. This function should
            //only be called in no other Shaders have been attached to this object, and after calling
            //this function no additional shaders may be attached.
            bool attachCompute(const char* compute);
            //Uses an already-created Compute shader and attaches it to this program. This function should
            //only be called in no other Shaders have been attached to this object, and after calling
            //this function no additional shaders may be attached.
            void attachCompute(const ShaderInterface::ComputeShader* compute);
            //Uses an already created Compute shader that does not contain a 'main' function and has
            //been marked as secondary using the 'makeSecondary()' function and attaches it to this
            //ShaderProgram. Compute shaders have the additional requirement that they must be the only type of
            //shader attached to any ShaderProgram. Attaching a compute shader to a program
            //that already has shaders of another type attached is an error, as is trying to attach shaders
            //of a different type to a program that already has compute shaders attached.
            //It is possible to attach as many secondaries as are available, however it is an
            //error to try to attach the same secondary shader multiple times to one program. 
            void attachSecondaryCompute(const ShaderInterface::ComputeShader* compute);


            //Checks to see if this ShaderProgram is linked
            bool checkIfLinked() const noexcept { return mState.mLinked; }

            //Links this shader program object. This shader program must have either solely a Compute
            //shader or both a Vertex and Fragment shader attached before linking. Additionally,
            //using the Tessellation shaders requires both a Tessellation control and Tessellation 
            //evaluation shader to be attached.
            void link() noexcept;

            //Binds this ShaderProgram to the active program spot in the GL Context. This ShaderProgram
            //must be linked and valid before this function call will modify the GL Context's state.
            void use() const noexcept;

            //Allows this ShaderProgram to be called like a function with no parameters.
            //void operator()() const { this->use(); } //This is probably unsafe and shouldn't be used...

            //Returns the OpenGL-assigned ID number for this shader program
            GLuint ID() const noexcept { return mProgramID; }

            //Deletes this object from the graphics language context
            void release() noexcept {
                if (!mState.mReleased) {
                    glDeleteProgram(mProgramID);
                    mState.mReleased = true;
                    mProgramID = 0u;
                    mState.mValid = false;
                }
            }

            //-------------------------------
            //	Object State Accessors                (Getters)
            //-------------------------------

            //Queries the current GL context to see if this object is a valid shader program
            //Queries the current GL context to see if this object is a valid shader program
            bool isProgram() const noexcept { return glIsProgram(mProgramID); }

            //Checks to see if this program has had itself deleted from the GL Context's state 
            //through the OpenGL function 'glDeleteProgram()'
            bool released() const noexcept { return mState.mReleased; }

            //Returns true when this object is ready for use
            bool valid() const noexcept { return mState.mValid; }

            //Returns true when an error has occurred during the lifetime of this object, rendering it's behavior as undefined
            bool error() const noexcept { return mState.mError; }

            //The ShaderProgram will not be ready to link until this function returns true
            bool readyToLink() const noexcept { return mState.mReadyToLink; }

            bool hasVertAttached() const noexcept { return mState.mHasVert; }
            bool hasCachedVert() const noexcept { return (mVertexShader != nullptr); }
            bool hasGeom() const noexcept { return mState.mHasGeom; }
            bool hasCachedGeom() const noexcept { return (mGeometryShader != nullptr); }
            bool hasTessE() const noexcept { return mState.mHasTesse; }
            bool hasCachedTesse() const noexcept { return (mTessellationEvaluationShader != nullptr); }
            bool hasTessC() const noexcept { return mState.mHasTessc; }
            bool hasCachedTessc() const noexcept { return (mTessellationEvaluationShader != nullptr); }
            bool hasFrag() const noexcept { return mState.mHasFrag; }
            bool hasCachedFrag() const noexcept { return (mFragmentShader != nullptr); }
            bool hasCompute() const noexcept { return mState.mHasCompute; }
            bool hasCachedCompute() const noexcept { return (mComputeShader != nullptr); }

            //Gets the number of attached secondary vertex shaders to this ShaderProgram
            int getAttachedSecondaryVertCount() const noexcept { return mState.mAttachedSecondaryVertCount; }
            //Gets the number of attached secondary geometry shaders to this ShaderProgram
            int getAttachedSecondaryGeomCount() const noexcept { return mState.mAttachedSecondaryGeomCount; }
            //Gets the number of attached secondary tesse shaders to this ShaderProgram
            int getAttachedSecondaryTesseCount() const noexcept { return mState.mAttachedSecondaryTesseCount; }
            //Gets the number of attached secondary tessc shaders to this ShaderProgram
            int getAttachedSecondaryTesscCount() const noexcept { return mState.mAttachedSecondaryTesscCount; }
            //Gets the number of attached secondary fragment shaders to this ShaderProgram
            int getAttachedSecondaryFragCount() const noexcept { return mState.mAttachedSecondaryFragCount; }
            //Gets the number of attached secondary compute shaders to this ShaderProgram
            int getAttachedSecondaryComputeCount() const noexcept { return mState.mAttachedSecondaryComputeCount; }


            //Uniform processing is handled through a UniformLocationInterface member object
            ShaderInterface::UniformLocationInterface uniforms;
            
        private:
            GLuint mProgramID; //ID assigned by GL Context
            class ProgramState {
            public:
                bool mValid;
                bool mLinked;
                bool mHasVert, mHasGeom, mHasTesse, mHasTessc, mHasFrag, mHasCompute; //These booleans represent attached shaders, not cached shaders
                int mAttachedSecondaryVertCount, mAttachedSecondaryGeomCount, mAttachedSecondaryTesseCount, mAttachedSecondaryTesscCount;
                int mAttachedSecondaryFragCount, mAttachedSecondaryComputeCount;
                std::unordered_map<GLuint, std::string> mAttachedSecondaries;
                bool mError;
                bool mReadyToLink;
                bool mReleased;

                //Constructor
                ProgramState() noexcept;
            };

            ProgramState mState;
            
            //Primary shaders (can only have 1 of each at a time)
            std::unique_ptr<ShaderInterface::VertexShader> mVertexShader;
            std::unique_ptr<ShaderInterface::GeometryShader> mGeometryShader;
            std::unique_ptr<ShaderInterface::TessellationControlShader> mTessellationControlShader;
            std::unique_ptr<ShaderInterface::TessellationEvaluationShader> mTessellationEvaluationShader;
            std::unique_ptr<ShaderInterface::FragmentShader> mFragmentShader;
            std::unique_ptr<ShaderInterface::ComputeShader> mComputeShader;

        
            //Private helper functions
            void initialize() noexcept;
            void generateGLProgramHandle() noexcept;
            void initializeUniformLocationTracker() noexcept;

            bool checkToSeeIfReadyToLinkAfterAttachingVert() const noexcept;
            bool checkToSeeIfReadyToLinkAfterAttachingTesse() const noexcept;
            bool checkToSeeIfReadyToLinkAfterAttachingTessc() const noexcept;
            bool checkToSeeIfReadyToLinkAfterAttachingFrag() const noexcept;
            
            bool checkAttachedSecondariesForMatch(const ShaderInterface::ShaderID&) const;
            void addShaderToAttachedSecondaries(const ShaderInterface::CompiledShader* secondaryShader); //Do not pass nullptr to this function!
        };

//} //namespace ShaderInterface


#endif //#ifndef SHADER_PROGRAM_H_
