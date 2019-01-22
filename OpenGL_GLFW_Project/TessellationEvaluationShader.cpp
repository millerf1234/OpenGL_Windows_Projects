//Class: Tesselation Evaluation Shader
//Namespace: ShaderInterface
//Programmer: Forrest Miller
//Date(s): 7/24/2018 - 7/31/2018  
//
//This is the implementation file for Tesselation Evaluation Shader. This type derives from CompiledShader.
//See the header file for more details. 
//
// ImplementationNotes:   - Copying is not allowed but moving is allowed. Luckily most of 
//						    the implementation details is shared amongst shaders and is located 
//						    in the CompiledShader implementation file. 


#include "TessellationEvaluationShader.h"

namespace ShaderInterface {

	TessellationEvaluationShader::TessellationEvaluationShader(const char * filepath) : CompiledShader(filepath, GL_TESS_EVALUATION_SHADER) {
		if (mShaderID.mID != 0u)
			mShaderID.mType = ShaderType::TESSELATION_EVALUATION;

#if defined PRINT_SHADER_COMPILE_MESSAGES 
		if (!mError)
			fprintf(MSGLOG, "Created Tesselation Evaluation shader from source \"%s\"\n", filepath);
#endif //PRINT_SHADER_COMPILE_MESSAGES 
	}

	TessellationEvaluationShader::TessellationEvaluationShader(TessellationEvaluationShader&& other) : CompiledShader() {
		copyMemberVariables(other);
		other.invalidateCompiledShaderAfterCopying();
	}

	//TessellationEvaluationShader::~TessellationEvaluationShader() {
    //
	//}

	void TessellationEvaluationShader::reinstate() {
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
			if (compile(GL_TESS_EVALUATION_SHADER)) {
				mIsDecomissioned = false;
			}
		}
	}

	TessellationEvaluationShader& TessellationEvaluationShader::operator=(TessellationEvaluationShader&& that) {
		if (this != &that) {
			copyMemberVariables(that);
			that.invalidateCompiledShaderAfterCopying();
		}
		return *this;
	}

	//void TessellationEvaluationShader::aquireShaderID() {
	//	if (mShaderID.mID != 0u) {
	//		fprintf(ERRLOG, "\nError aquiring shaderID. This shader already has ID %u\n", mShaderID.mID);
	//		return;
	//	}
	//	mShaderID = glCreateShader(GL_TESS_EVALUATION_SHADER);
	//}

	void TessellationEvaluationShader::makeSureShaderSourceTextIsLoaded() {
		if (mSourceText == nullptr) {
			loadSourceFile();
		}
	}


} //namespace ShaderInterface


