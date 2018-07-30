//Implementation file for the Fragment Shader class.
//The Fragment Shader class inherits from the CompiledShader class\
//
//Created by Forrest Miller on July 26, 2018

#include "FragmentShader.h"

namespace ShaderInterface {

	FragmentShader::FragmentShader(const char * filepath) : CompiledShader(filepath) {
		mType = ShaderType::FRAGMENT;
		if (mHasLoadedSourceText)
			compile();
	}

	FragmentShader::FragmentShader(const FragmentShader& that) : CompiledShader(that.mFilepath) {
		if (that.mValidFilepath) {
			FragmentShader::FragmentShader(that.mFilepath);
			mType = ShaderType::FRAGMENT;
		}
		else {
			mValidFilepath = false;
			mType = ShaderType::FRAGMENT;
		}
		
	}
	FragmentShader::FragmentShader(FragmentShader&& that) : CompiledShader(that.mFilepath) {
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
	
	FragmentShader::~FragmentShader() {

	}

	void FragmentShader::reinstate() {
		if (!mWasDecomissioned) {
			return;
		}
		else {
			FragmentShader::FragmentShader(mFilepath);
			mWasDecomissioned = false;
		}
	}

	//Assigns a different ID to this shader if need be
	FragmentShader& FragmentShader::operator=(const FragmentShader& that) {
		if (this != &that) {
			CompiledShader::operator=(that); //Call the base class copy operator
			if (that.mError || !that.mValidFilepath) {
				fprintf(WRNLOG, "\nWarning! Copying invalid fragment shader \"%s\"\n", mFilepath);
				return *this;
			}
			else if (that.mWasDecomissioned) {
				fprintf(WRNLOG, "\nWarning! Copying decomissioned fragment shader \"%s\"\n", mFilepath);
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
	FragmentShader& FragmentShader::operator=(FragmentShader&& that) {
		if (this != &that) {
			if (!that.mValid) {
				FragmentShader::FragmentShader(that.mFilepath);
			}
			else {
				if (that.mWasDecomissioned) {
					FragmentShader::FragmentShader(that.mFilepath);
				}
				else {
					CompiledShader::operator=(that);
					that.mShaderID = 0u; //That loses ownership over the OpenGL shader
				}
			}

		}
		return *this;
	}

	void FragmentShader::aquireShaderID() {
		if (mShaderID != 0u) {
			fprintf(ERRLOG, "\nError aquiring shaderID. This shader already has ID %u\n", mShaderID);
			return;
		}
		mShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	}

} //namespace ShaderInterface