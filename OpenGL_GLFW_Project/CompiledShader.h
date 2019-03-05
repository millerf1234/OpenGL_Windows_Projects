//Class:      CompiledShader    [Abstract Base Class]
//Namespace:  ShaderInterface
//Programmer: Forrest Miller
//Date(s):    7/24/2018 - 7/31/2018  
//
//Description: The CompiledShader class is an abstract base class that
//			   is intended to cover the common logic shared between all
//			   OpenGL shader types. Since all OpenGL shaders are parsed, compiled and 
//			   tracked the exact same way, most of the functions and logic for them
//			   can be found in this class. This class requires a filepath
//			   to be constructed. Copying and Moving of the abstract base
//			   type (i.e. this class) is not allowed. Also, by having a shared base
//			   class, it is possible to have a common comparison operator, so that a
//			   collection of various shaders can be sorted using '<' or '>'. 
//			   This class handles managing the Shader within both the application
//			   side and with the GL context. Creating the shader within the GL
//			   context is handled by this class as is the cleanup for the shader.
//
// UPDATES/Changes:
//					August 1, 2018            Changed this class's constructor so that it now requires an additional GLenum 
//											  parameter which this class then stores and uses as the macro to use when
//											  glCreateShader() is called. This effectively is replacing the protected abstract 
//											  member function  aquireShaderID(), which was intended to be implemented by 
//											  derived classes so that they each could implement their own macro.
//											  In other words, the GLContext Shader creation logic was moved from the derived
//											  classes to this class.
//				
//					September 14, 2018        Added the ability to mark a CompiledShader as secondary, which will allow multiple
//											  shaders of the same type to be attached to one program. The intention is that by 
//											  marking a shader as secondary, the application promises that the shader does not 
//											  contain a 'main' function, since linking a program with 2 shaders of the same type
//											  that both contain a main will lead to a program-linkage error. It is legal to attach
//											  as many secondaries as are available for attaching, and it is legal to attach secondaries
//										      before attaching a primary (i.e. a shader that contains a 'main' function). 
//											  
//			
//Notes:   -While it is possible to recover the shader's ID number, it is 
//			highly advised that no calls are made to the GL Context that 
//			will affect the state of this object (i.e. don't call glDeleteProgram() 
//			with the id value acquired by the getter function 'ID()'  ).
//       
//		   -Since it is advised to delete the compiled shader objects once the linking of
//			shader programs is completed, this class provides a function called decommission()
//			which handles cleaning up this shader. Once a shader is decommissioned, it will
//			need to be reinstated in order to be attached again. Reinstating involves
//			reacquiring the text and recompiling the shader, and as such is not a cheap operation.
//
//		   -Both '==' and '!=' operators are defined between two CompiledShader instances.
//			Comparison will just look to see that the shaders are of the same type and that
//			they have the same filepath, any other differences in state will not impact equality.
//
//         -Since 0u will never be assigned to a valid shader within the GLContext, the way this
//			object represents a valid shader is any shaderID that is not 0u. Thus, when the destructor
//			for this object is called, its shaderID number is checked. If the shaderID is not 0, then it
//			is assumed that this shader represents a still-valid shader within the GLContext and a call
//			to glDeleteShader(shaderID) is made, and the shaderID is changed to be 0u. Thus when copying/moving
//			types derived from this class, it is important to invalidate one of the objects by setting its
//			ShaderID to 0u. 
//
// TODO  (write a better definition here for shader ordering...)
//		   -Comparison operators < and > are defined to allow for the sorting of an array/vector of CompiledShaders.
//			Ordering between compiled shader objects is defined as follows:
//		        (i)  If the shaders are of separate types, then they will be ordered as: 
//                      Compute < Fragment < Geometry < TessC < TessE < Vertex < UNASSIGNED < ShaderWithError
//              (ii) If the shaders are of the same type, they will be ordered based off their filepaths as c-strings
//                      (the comparsion is done using the c function 'strcmp')
//
//          The ordering between shaders of the same type and filepath is arbitrary, but well-defined    
//           
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

#include "GlobalConstants.h"

namespace ShaderInterface {

	//-------------------------------
	//Enum for the various shader types
	//-------------------------------
	enum class ShaderType {VERTEX, GEOMETRY, TESSELLATION_CONTROL,
						   TESSELATION_EVALUATION, FRAGMENT,
						   COMPUTE, UNASSIGNED};

	//-------------------------------
	// Struct to tie ShaderID and ShaderType together
	//-------------------------------
	typedef struct ShaderID {
		GLuint mID;       //Context-Assigned ID
		ShaderType mType; //Type of shader
		//Constructor
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
		//Note: The parameterless constructor is protected (It is declared below with the other protected class members)
		CompiledShader(const char * sourceFilepath, GLenum type);
		CompiledShader(const CompiledShader& that) = delete; //No copying from abstract base type
		CompiledShader(CompiledShader&& that) = delete; //No moving an abstract base type

		virtual ~CompiledShader();

		//-------------------------------
		// Operators
		//-------------------------------
		CompiledShader& operator=(const CompiledShader& that) = delete; //No copying from abstract base type
		CompiledShader& operator=(CompiledShader&& that) = delete; //No moving an abstract base type 

		//Compares ShaderTypes and Filepaths for equality. Does not check to see if
		//both shaders are primary or secondary.
		bool operator==(const CompiledShader&) const;

		//Compares ShaderTypes and Filepaths for inequality. Does not check to see if
		//both shaders are primary or secondary.
		bool operator!=(const CompiledShader&) const;

		//Comparison operators to allow for the sorting of shaders. Sorts shaders
		//by type
		bool operator<(const CompiledShader&) const;
		
		//Comparison operators to allow for the sorting of shaders. Sorts shaders
		//by type
		bool operator>(const CompiledShader&) const;

		//Returns true if the shader is ready to be attached (thus not decommissioned),
		//and false otherwise (if there was an error, a bad filepath or shader gets decommissioned)
		explicit operator bool() const; //(the 'explicit' keyword prevents this class from being allowed to conversion-construct a bool)

		//Returns true if for any reason the shader is not ready to be attached. 
		//Returns false if there is nothing preventing the shader from being attached.
		bool operator!() const;

		//explicit operator GLuint() const;  //This would allow for this object to be converted to a GLuint

		//-------------------------------
		//Public Interface Functions  
		//-------------------------------

		//Decommission marks a shader as no-longer needed for more programs and releases this objects resources.
		//After being decommissioned, a shader must be reinstated in order to be attached to new shader programs.
		void decommision();  //Alternative function names: abolish(), deactivate(), unload(), 

		//Reinstate recompiles the shader and allows it to be attached to ShaderPrograms again. Could change this
		//objects GL-Context-Assigned ID Number  (I am unsure if this will invalidate old shaders still bound with the old ID number of this shader?)
		virtual void reinstate() = 0; //Alternative function names: reestablish(), restore(), rehabilitate(), revive(), reload(), recompile(),

		//Marks this shader as a secondary shader, thus allowing it to be 
		//attached to ShaderPrograms that already have a shader of the same
		//type attached. By marking a shader as secondary, it is promised that
		//the shader does not contain a 'main' method. Failure to satisfy this
		//promise will result in errors in ShaderProgram linking. Once a shader
		//has been made secondary, it cannot be reverted back to a primary.
		void makeSecondary() { mMarkedAsSecondary = true; }

		//-------------------------------
		//Member field getters
		//-------------------------------

		//Returns the filepath of this shader's source file.
		std::string getFilepath() const { return mFilepath; }
		//Returns true if this shader has been decommissioned
		bool seeIfIsDecomissioned() const { return mIsDecomissioned; }
		//Returns true if this shader is ready to be attached to a ShaderProgram
		bool readyToBeAttached() const { return mReadyToBeAttached; }
		//Checks to see if this shader has been marked as a secondary shader. To be a 
		//secondary shader, the shader must not contain a 'main' function. A shader must
		//be marked secondary to be eligible to be attached as a secondary.
		bool markedAsSecondary() const { return mMarkedAsSecondary; }
		//Returns true if this shader was able to read and compile itself from the provided filepath
		bool validFilepath() const { return mValidFilepath; }
		//Checks with the GLContext to see if the shader is compiled
		bool isCompiled() const; 
		//Checks to see if an error has occurred with this shader (includes both client (Application) and Server (GPU) errors)
		bool error() const { return mError; }
		//Returns the type of this CompiledShader. Type is expected to always match the type of the derived shader object.
		ShaderType type() const { return mShaderID.mType; }
		//Returns the uniquely assigned name (as a GLuint) for this shader that was assigned 
		//by the context. (Really there is no reason for this function to be called (outside of debugging
		// and some possible future hack-job programming))
		GLuint ID() const { return mShaderID.mID; } 
		//Returns a copy of this object's shaderID struct 
		ShaderID shaderID() const { return ShaderID(mShaderID.mID, mShaderID.mType); }

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
		//Protected functions:  (these are internal functions that are made available for derived types)
		//----------------------------
		//Compiles the shader within OpenGL. Expects for there to be the loaded source text stored within a valid string pointed to by mSourceText 
		bool compile(GLenum type);

		//Creates a new shader within the GL Context and acquires a GLuint handle to the shader object. 
		//Note that 0u will never be used as a shader handle
		void aquireShaderID(GLenum type);

		//Loads the text of a file located at filepath. Useful for loading shaders
		bool loadSourceFile();

		//This function is to facilitate copying/moving amongst derived objects of this type. 
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
		bool mMarkedAsSecondary;

		//----------------------------
		//Private functions
		//----------------------------
		//Gives values to all of this classes member variables but doesn't necessarily put the object
		//in a valid state
		void initialize();
		
		//Checks to see if there is a "std::ios_base::failure" within the ifstream 
		bool checkIfValidFilepath(std::ifstream& inFileStream);

		//Checks with the GL context for compilation errors for the shader
		bool checkForCompilationErrors();

		//Used as part of the implementation for operator< and operator>
		static bool compareFilepaths(const char * fp1, const char * fp2) noexcept;
	};

}  //namespace ShaderInterface

#endif //COMPILED_SHADER_2_H_