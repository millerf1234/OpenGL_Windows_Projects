//Class: Tessellation Control Shader
//Namespace: ShaderInterface
//Programmer: Forrest Miller
//Date(s): 7/24/2018 - 7/31/2018  
//
//This is the implementation file for Tessellation Control Shader. This type derives from CompiledShader.
//See the header file for more details. 
//
// Implementation notes: Copying is not allowed but moving is allowed. Luckily most of 
//						 the implementation details is shared amongst shaders and is located 
//						 in the CompiledShader implementation file. 

#include "TessellationControlShader.h"

namespace ShaderInterface {

	TessellationControlShader::TessellationControlShader(const char * filepath) : CompiledShader(filepath, GL_TESS_CONTROL_SHADER) {
		if (mShaderID.mID != 0u)
			mShaderID.mType = ShaderType::TESSELLATION_CONTROL;

#if defined PRINT_SHADER_COMPILE_MESSAGES 
		if (!mError)
			fprintf(MSGLOG, "Created Tessellation Control shader from source \"%s\"\n", filepath);
#endif //PRINT_SHADER_COMPILE_MESSAGES 
	}

	TessellationControlShader::TessellationControlShader(TessellationControlShader&& other) : CompiledShader() {
		copyMemberVariables(other);
		other.invalidateCompiledShaderAfterCopying();
	}

	TessellationControlShader::~TessellationControlShader() {

	}

	void TessellationControlShader::reinstate() {
		if (!mIsDecomissioned) {
			fprintf(WRNLOG, "\nWarning! Reinstate() was called on shader \"%s\" even though this shader was never decommissioned!\n", mFilepath);
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
			if (compile(GL_TESS_CONTROL_SHADER)) {
				mIsDecomissioned = false;
			}
		}
	}

	TessellationControlShader& TessellationControlShader::operator=(TessellationControlShader&& that) {
		if (this != &that) {
			copyMemberVariables(that);
			that.invalidateCompiledShaderAfterCopying();
		}
		return *this;
	}

	//void TessellationControlShader::aquireShaderID() {
	//	if (mShaderID.mID != 0u) {
	//		fprintf(ERRLOG, "\nError acquiring shaderID. This shader already has ID %u\n", mShaderID.mID);
	//		return;
	//	}
	//	mShaderID = glCreateShader(GL_TESS_CONTROL_SHADER);
	//}

	void TessellationControlShader::makeSureShaderSourceTextIsLoaded() {
		if (mSourceText == nullptr) {
			loadSourceFile();
		}
	}


} //namespace ShaderInterface


