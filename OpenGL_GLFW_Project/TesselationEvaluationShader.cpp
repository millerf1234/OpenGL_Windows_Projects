//Implementation file for the Tesselation Evaluation Shader class.
//The Tesselation Evaluation Shader class inherits from the CompiledShader class
//
//Created by Forrest Miller on July 26, 2018

#include "TesselationEvaluationShader.h"

namespace ShaderInterface {

	TesselationEvaluationShader::TesselationEvaluationShader(const char * filepath) : CompiledShader(filepath) {
		mType = ShaderType::TESSELATION_EVALUATION;
		if (mHasLoadedSourceText) //if the base constructor was able to load the file
			compile();
	}

	TesselationEvaluationShader::TesselationEvaluationShader(const TesselationEvaluationShader& that) : CompiledShader(that.mFilepath) {
		if (that.mValidFilepath) {
			TesselationEvaluationShader::TesselationEvaluationShader(that.mFilepath);
			mType = ShaderType::TESSELATION_EVALUATION;
		}
		else {
			mValidFilepath = false;
			mType = ShaderType::TESSELATION_EVALUATION;
		}

	}
	TesselationEvaluationShader::TesselationEvaluationShader(TesselationEvaluationShader&& that) : CompiledShader(that.mFilepath) {
		if (that.mValid) {
			mShaderID = that.mShaderID;
			that.mShaderID = 0u;
			that.mValid = false;
		}
		else {
			mValid = false;
			mType = ShaderType::TESSELATION_EVALUATION;
		}
	}

	TesselationEvaluationShader::~TesselationEvaluationShader() {

	}

	void TesselationEvaluationShader::reinstate() {
		if (!mWasDecomissioned) {
			fprintf(WRNLOG, "\nWarning! Unable to reinstate Tess Eval shader \"%s\" because it was never decomissioned!\n", mFilepath);
			return;
		}
		else {
			TesselationEvaluationShader::TesselationEvaluationShader(mFilepath); //Reconstruct this object to reinstate it
			mWasDecomissioned = false;
		}
	}

	//Assigns a different ID to this shader if need be
	TesselationEvaluationShader& TesselationEvaluationShader::operator=(const TesselationEvaluationShader& that) {
		if (this != &that) {
			CompiledShader::operator=(that); //Call the base class copy operator
			if (that.mError || !that.mValidFilepath) {
				fprintf(WRNLOG, "\nWarning! Copying invalid TessEval shader \"%s\"\n", mFilepath);
				return *this;
			}
			else if (that.mWasDecomissioned) {
				fprintf(WRNLOG, "\nWarning! Copying decomissioned TessEval shader \"%s\"\n", mFilepath);
				return *this;
			}
			else if (that.mShaderID != 0u) {
				if (mHasLoadedSourceText) {
					mValid = mHasBeenCompiled = compile();
				}
				else { //else something is screwed up so we gotta reload the shader from the file
					loadSourceFile(mFilepath, mSourceText);
					if (mHasLoadedSourceText) {
						mValid = mHasBeenCompiled = compile();
					}
				}
			}
		}
		return *this;
	}
	TesselationEvaluationShader& TesselationEvaluationShader::operator=(TesselationEvaluationShader&& that) {
		if (this != &that) {
			if (!that.mValid) {
				TesselationEvaluationShader::TesselationEvaluationShader(that.mFilepath);
			}
			else {
				if (that.mWasDecomissioned) {
					TesselationEvaluationShader::TesselationEvaluationShader(that.mFilepath);
				}
				else {
					CompiledShader::operator=(that);
					that.mShaderID = 0u; //'that' loses ownership over the OpenGL shader
				}
			}
		}
		return *this;
	}

	void TesselationEvaluationShader::aquireShaderID() {
		if (mShaderID != 0u) {
			fprintf(ERRLOG, "\nError aquiring shaderID. This shader already has ID %u\n", mShaderID);
			return;
		}
		mShaderID = glCreateShader(GL_TESS_EVALUATION_SHADER); 
	}

} //namespace ShaderInterface