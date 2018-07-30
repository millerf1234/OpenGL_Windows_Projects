//Implementation file for the Compute Shader class.
//The Compute Shader class inherits from the CompiledShader class
//
//Created by Forrest Miller on July 26, 2018

#include "ComputeShader.h"

namespace ShaderInterface {

	ComputeShader::ComputeShader(const char * filepath) : CompiledShader(filepath) {
		mType = ShaderType::FRAGMENT;
		if (mHasLoadedSourceText)
			compile();
	}

	ComputeShader::ComputeShader(const ComputeShader& that) : CompiledShader(that.mFilepath) {
		if (that.mValidFilepath) {
			ComputeShader::ComputeShader(that.mFilepath);
			mType = ShaderType::COMPUTE;
		}
		else {
			mValidFilepath = false;
			mType = ShaderType::COMPUTE;
		}

	}
	ComputeShader::ComputeShader(ComputeShader&& that) : CompiledShader(that.mFilepath) {
		if (that.mValid) {
			mShaderID = that.mShaderID;
			that.mShaderID = 0u;
			that.mValid = false;
		}
		else {
			mValid = false;
			mType = ShaderType::FRAGMENT;
		}
	}

	ComputeShader::~ComputeShader() {

	}

	void ComputeShader::reinstate() {
		if (!mWasDecomissioned) {
			return;
		}
		else {
			ComputeShader::ComputeShader(mFilepath);
			mWasDecomissioned = false;
		}
	}

	//Assigns a different ID to this shader if need be
	ComputeShader& ComputeShader::operator=(const ComputeShader& that) {
		if (this != &that) {
			CompiledShader::operator=(that); //Call the base class copy operator
			if (that.mError || !that.mValidFilepath) {
				fprintf(WRNLOG, "\nWarning! Copying invalid Compute shader \"%s\"\n", mFilepath);
				return *this;
			}
			else if (that.mWasDecomissioned) {
				fprintf(WRNLOG, "\nWarning! Copying decomissioned Compute shader \"%s\"\n", mFilepath);
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
	ComputeShader& ComputeShader::operator=(ComputeShader&& that) {
		if (this != &that) {
			if (!that.mValid) {
				ComputeShader::ComputeShader(that.mFilepath);
			}
			else {
				if (that.mWasDecomissioned) {
					ComputeShader::ComputeShader(that.mFilepath);
				}
				else {
					CompiledShader::operator=(that);
					that.mShaderID = 0u; //That loses ownership over the OpenGL shader
				}
			}
		}
		return *this;
	}

	void ComputeShader::aquireShaderID() {
		if (mShaderID != 0u) {
			fprintf(ERRLOG, "\nError aquiring shaderID. This shader already has ID %u\n", mShaderID);
			return;
		}
		mShaderID = glCreateShader(GL_COMPUTE_SHADER);
	}

} //namespace ShaderInterface