//Class: CompiledShader
//Namespace: ShaderInterface
//Programmer: Forrest Miller
//Date(s): 7/24/2018 - 7/31/2018  
//
//Description: The CompiledShader class is an abstract base class that
//			   is intended to cover the common logic shared between all
//			   OpenGL shader types. Since all OpenGL shaders have a lot 
//			   in common, as such most of the logic for the derived classes
//			   can be found in this class. This class requires a filepath
//			   to be constructed. Copying and Moving of the abstract base
//			   type (i.e. this class) is not allowed. 
//			   This class handles managing the Shader within both the application
//			   side and with the GL context. Creating the shader within the GL
//			   context is handled by this class as is the cleanup for the shader.
//
// UPDATES/Changes:
//					August 1, 2018            Changed this class's constructor so that it now requires an additional GLenum 
//											  parameter which this class then stores and uses as the macro to use when
//											  glCreateShader() is called. This effectivly is replacing the protected abstract 
//											  member function  aquireShaderID(), which was intended to be implemented by 
//											  derived classes so that they each could implement their own macro.
//											  In other words, the GLContext Shader creation logic was moved from the derived
//											  classes to this class.
//
//			
//Notes:   -While it is possible to recover the shader's ID number, it is 
//			highly advised that no calls are made to the GL Context that 
//			will affect the state of this object (i.e. don't call glDeleteProgram() 
//			with the id value aquired by the getter function ID()  ).
//       
//		   -Since it is advised to delete the compiled shader objects once the linking of
//			shader programs is completed, this class provides a function called decomission()
//			which handles cleaning up this shader. Once a shader is decomissioned, it will
//			need to be reinstated in order to be attached again. Reinstating involves
//			reaquiring the text and recompiling the shader, and as such is not a cheap operation.
//
//		   -Both '==' and '!=' operators are defined between two CompiledShader instances.
//			Comparison will just look to see that the shaders are of the same type and that
//			they have the same filepath, any other differences in state will not impact equality.
//
//         -Since 0u will never be assigned to a valid shader within the GLContext, the way this
//			object represents a valid shader is any shaderID that is not 0u. Thus, when the desctructor
//			for this object is called, its shaderID number is checked. If the shaderID is not 0, then it
//			is assumed that this shader represents a still-valid shader within the GLContext and a call
//			to glDeleteShader(shaderID) is made, and the shaderID is changed to be 0u. Thus when copying/moving
//			types derived from this class, it is important to invalidate one of the objects by setting its
//			ShaderID to 0u. 
//
//		   -Comparison operatos < and > are defined to allow for the sorting of an array/vector of CompiledShaders.
//			The ordering is first done by derived type in the order:
//					Compute < Fragment < Geometry < TessC < TessE < Vertex < UNASSIGNED < ShaderWithError
//          and within shaders of the same type the ordering is done alphabetically by filepath name
//
//
//Errors to watch for:  Invalid Filepath
//						Error within the Context/Compilation error
//
//
//Additional Notes:     There is a protected default constructor that was necessary to allow for
//						the definition of move constructors in the derived types. Note that this
//						parameterless constructor should not be called outside of move operations 
//						by derived classes and that calling this constructor will not result in a
//					    valid object.

#pragma once

#ifndef COMPILED_SHADER_H_
#define COMPILED_SHADER_H_

#include <fstream>

#include "ProjectConstants.h"
#include "ProjectParameters.h"

namespace ShaderInterface {

	//-------------------------------
	//Enum for the various shader types
	//-------------------------------
	enum class ShaderType {VERTEX, GEOMETRY, TESSELATION_CONTROL,
						   TESSELATION_EVALUATION, FRAGMENT,
						   COMPUTE, UNASSIGNED};

	//-------------------------------
	// Struct to tie ShaderID and ShaderType together
	//-------------------------------
	typedef struct ShaderID {
		GLuint mID;
		ShaderType mType;
		ShaderID(GLuint id = 0u, ShaderType type = ShaderType::UNASSIGNED) : mID(id), mType(type) { ; }
	} ShaderID;


	//-------------------------------
	//  Abstract base class for compiled shaders
	//-------------------------------
	class CompiledShader {
	public:
		//-------------------------------
		// Constructors / Destructor  
		//-------------------------------
		//Default constructor is Protected 
		CompiledShader(const char * sourceFilepath, GLenum type);
		CompiledShader(const CompiledShader& that) = delete; //No copying from abstract base type
		CompiledShader(CompiledShader&& that) = delete; //No moving an abstract base type

		virtual ~CompiledShader();

		//-------------------------------
		// Operators
		//-------------------------------
		CompiledShader& operator=(const CompiledShader& that) = delete; //No copying from abstract base type
		CompiledShader& operator=(CompiledShader&& that) = delete; //No moving an abstract base type 

		//Compares ShaderTypes and Filepaths for equality
		bool operator==(const CompiledShader&) const;

		//Compares ShaderTypes and Filepaths for inequality
		bool operator!=(const CompiledShader&) const;

		//Two comparison operators to allow for the sorting of shaders
		bool operator<(const CompiledShader&) const;
		
		bool operator>(const CompiledShader&) const;


		//-------------------------------
		//Public Interface Functions  
		//-------------------------------

		//Decomission marks a shader as no-longer needed for more programs and releases this objects resources.
		//After being decommisioned, a shader must be reinstated in order to be attached to new shader programs.
		void decommision();  //Alternative function names: abolish(), deactivate(), unload(), 

		//Reinstate recompiles the shader and allows it to be attached to ShaderPrograms again. Could change this
		//objects GL-Context-Assigned ID Number  (I am unsure if this will invalidate old shaders still bound with the old ID number of this shader?)
		virtual void reinstate() = 0; //Alternative function names: reestablish(), restore(), rehabilitate(), revive(), reload(), recompile(),

		//-------------------------------
		//Member field getters
		//-------------------------------
		const char * getFilepath() const { return mFilepath; }
		bool seeIfIsDecomissioned() const { return mIsDecomissioned; }
		bool readyToBeAttached() const { return mReadyToBeAttached; }
		bool validFilepath() const { return mValidFilepath; }
		//Checks with the GLContext to see if the shader is compiled
		bool isCompiled() const; 
		bool error() const { return mError; }
		ShaderType type() const { return mShaderID.mType; }
		GLuint ID() const { return mShaderID.mID; }

	protected:
		//-------------------------------
		// Protected member variables (fields)
		//-------------------------------
		ShaderID mShaderID;
		bool mIsDecomissioned;
		const char * mFilepath;
		std::unique_ptr<std::string> mSourceText;
		bool mError;

		//-------------------------------
		// Protected Default Constructor (see notes at top of this file)
		//-------------------------------
		//Don't call this constructor! Use the one which takes a filepath instead!
		CompiledShader(); //Default constructor should only be called by derived types since it will create an invalid object

		//----------------------------
		//Protected functions:
		//----------------------------
		//Compiles the shader within OpenGL. Expects for there to be the loaded source text stored within a valid string pointed to by mSourceText 
		bool compile(GLenum type);

		//Creates a new shader within the GL Context and aquires a GLuint handle to the shader object. 
		//Note that 0u will never be used as a shader handle
		void aquireShaderID(GLenum type);

		//Loads the text of a file located at filepath. Useful for loading shaders
		bool loadSourceFile();

		//This function is to faciliate copying/moving amongst derived objects of this type. 
		//Will copy all of the non-pointer member variables from Source to the object calling this function.
		//Does not copy mSourceText since that is a pointer.
		void copyMemberVariables(CompiledShader& source);

		//This function is meant to be called after the copyPrivateMemberVariables function to invalidate
		//the source object so that ownership of the GL shader is retained by a single object
		void invalidateCompiledShaderAfterCopying();

	private:
		//----------------------------
		// Private member variables
		//----------------------------

		bool mReadyToBeAttached;
		bool mValidFilepath;

		//----------------------------
		//Private functions
		//----------------------------
		//Gives values to all of this classes member variables but doesn't necessarly put the object
		//in a valid state
		void initialize();
		
		//Checks to see if there is a "std::ios_base::failure" within the ifstream 
		bool checkIfValidFilepath(std::ifstream& inFileStream);

		//Checks with the GL context for compilation errors for the shader
		bool checkForCompilationErrors();

		//Used as part of the implementation for operator< and operator>
		static bool compareFilepaths(const char * fp1, const char * fp2);
	};

}  //namespace ShaderInterface

#endif //COMPILED_SHADER_2_H_