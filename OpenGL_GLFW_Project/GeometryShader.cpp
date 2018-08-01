//Class: GeometryShader
//Namespace: ShaderInterface
//Programmer: Forrest Miller
//Date(s): 7/24/2018 - 7/31/2018  
//
//This is the implementation file for Geometry Shader. This type derives from CompiledShader.
//See the header file for more details. 
//This type is intended to wrap a single Geometry shader source code object
//
// Implementation notes: Copying is not allowed but moving is allowed. Luckily most of 
//						 the implementation details is shared amongst shaders and is located 
//						 in the CompiledShader implementation file. 

#include "GeometryShader.h"

namespace ShaderInterface {

	GeometryShader::GeometryShader(const char * filepath) : CompiledShader(filepath) {
#if defined PRINT_SHADER_COMPILE_MESSAGES 
		if (!mError)
			fprintf(MSGLOG, "Created geometry shader from source \"%s\"\n", filepath);
#endif //PRINT_SHADER_COMPILE_MESSAGES 
	}

	GeometryShader::GeometryShader(GeometryShader&& other) : CompiledShader() {
		copyMemberVariables(other);
		other.invalidateCompiledShaderAfterCopying();
	}

	GeometryShader::~GeometryShader() {

	}

	void GeometryShader::reinstate() {
		if (!mIsDecomissioned) {
			fprintf(WRNLOG, "\nWarning! Reinstate() was called on shader \"%s\" even though this shader was never decomissioned!\n", mFilepath);
			return;
		}
		else if (!validFilepath()) {
			fprintf(ERRLOG, "\nERROR! Unable to reinstate() shader \"%s\" because filepath is invalid!\n", mFilepath);
			mError = true;
			return;
		}
		else if (mError) {
			fprintf(ERRLOG, "\nERROR Reinstating shader \"%s\"!\nReason: This shader has already encountered an error!\n", mFilepath);
			return;
		}
		else { //Actually go ahead and reinstate it
			makeSureShaderSourceTextIsLoaded();
			if (compile()) {
				mIsDecomissioned = false;
			}
		}
	}

	GeometryShader& GeometryShader::operator=(GeometryShader&& that) {
		if (this != &that) {
			copyMemberVariables(that);
			that.invalidateCompiledShaderAfterCopying();
		}
		return *this;
	}

	void GeometryShader::aquireShaderID() {
		if (mShaderID.mID != 0u) {
			fprintf(ERRLOG, "\nError aquiring shaderID. This shader already has ID %u\n", mShaderID.mID);
			return;
		}
		mShaderID = glCreateShader(GL_GEOMETRY_SHADER);
	}

	void GeometryShader::makeSureShaderSourceTextIsLoaded() {
		if (mSourceText == nullptr) {
			loadSourceFile();
		}
	}


} //namespace ShaderInterface