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

	}
	FragmentShader::FragmentShader(FragmentShader&& that) : CompiledShader(that.mFilepath) {

	}
	
	FragmentShader::~FragmentShader() {

	}

	//Decomissions this frag shader object
	void FragmentShader::decommision() {

	}
	void FragmentShader::reinstate() {

	}

	//Assigns a different ID to this shader if need be
	FragmentShader& FragmentShader::operator=(const FragmentShader& that) {
		if (this != &that) {
			CompiledShader::operator=(that); //Call the base class copy operator
			if (that.mError || !that.mValidFilepath) {
				fprintf(WRNLOG, "\nWarning! Copying invalid fragment shader \"%s\"\n", mFilepath);
				return;
			}
			else if (that.mWasDecomissioned) {
				fprintf(WRNLOG, "\nWarning! Copying decomissioned fragment shader \"%s\"\n", mFilepath);
				return;
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

	bool FragmentShader::compile() {
		if (isCompiled()) {
			if (mShaderID == 0) {
				mHasBeenCompiled = false;
			}
			fprintf(ERRLOG, "\nERROR COMPILING SHADER \"%s\"!\nThis shader is already compiled!\n", mFilepath);
			return true;
		}
		if (!mHasLoadedSourceText || !mValidFilepath) 
			return false;
		if (mShaderID != 0u) { //Check to make sure there isn't a shader leak
			GLboolean shaderStillValid = glIsShader(mShaderID);
			if (shaderStillValid) {
				//Check to see if a delete call is already pending with OpenGL
				GLint shaderMarkedForDeletion;
				glGetShaderiv(mShaderID, GL_DELETE_STATUS, &shaderMarkedForDeletion);
				if (!shaderMarkedForDeletion) {
					glDeleteShader(mShaderID);
				}
			}
			mShaderID = 0u;
		}
	
		aquireShaderID();
		//Check to see if this ShaderID is valid
		GLboolean shaderStillValid = glIsShader(mShaderID);
		if (shaderStillValid) {
			fprintf(WRNLOG, "\nWARNING! Unable to compile shader \"%s\" because ShaderID already represents a program!\n", mFilepath);
			return true;
		}
		const GLchar* rawShaderSource = mSourceText.c_str();
		glShaderSource(mShaderID, 1, &rawShaderSource, NULL);
		glCompileShader(mShaderID);

		checkForCompilationErrors();
		return mValid;
	}

	void FragmentShader::aquireShaderID() {
		if (mShaderID != 0u) {
			fprintf(ERRLOG, "\nError aquiring shaderID. This shader already has ID %u\n", mShaderID);
			return;
		}
		mShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	}

} //namespace ShaderInterface