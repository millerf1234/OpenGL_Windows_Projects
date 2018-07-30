//Implementation file for the Tesselation Control Shader class.
//The Tesselation Control Shader class inherits from the CompiledShader class
//
//Created by Forrest Miller on July 26, 2018

#include "TesselationControlShader.h"

namespace ShaderInterface {

	TesselationControlShader::TesselationControlShader(const char * filepath) : CompiledShader(filepath) {
		mType = ShaderType::TESSELATION_CONTROL;
		if (mHasLoadedSourceText) //if the base constructor was able to load the file
			compile();
	}

	TesselationControlShader::TesselationControlShader(const TesselationControlShader& that) : CompiledShader(that.mFilepath) {
		if (that.mValidFilepath) {
			TesselationControlShader::TesselationControlShader(that.mFilepath);
			mType = ShaderType::TESSELATION_CONTROL;
		}
		else {
			mValidFilepath = false;
			mType = ShaderType::TESSELATION_CONTROL;
		}

	}
	
	TesselationControlShader::TesselationControlShader(TesselationControlShader&& that) : CompiledShader(that.mFilepath) {
		if (that.mValid) {
			mShaderID = that.mShaderID;
			that.mShaderID = 0u;
			that.mValid = false;
		}
		else {
			mValid = false;
			mType = ShaderType::TESSELATION_CONTROL;
		}
	}

	TesselationControlShader::~TesselationControlShader() {

	}

	void TesselationControlShader::reinstate() {
		if (!mWasDecomissioned) {
			fprintf(WRNLOG, "\nWarning! Unable to reinstate Tess Cntrl shader \"%s\" because it was never decomissioned!\n", mFilepath);
			return;
		}
		else {
			TesselationControlShader::TesselationControlShader(mFilepath); //Reconstruct this object to reinstate it
			mWasDecomissioned = false;
		}
	}

	//Assigns a different ID to this shader if need be
	TesselationControlShader& TesselationControlShader::operator=(const TesselationControlShader& that) {
		if (this != &that) {
			CompiledShader::operator=(that); //Call the base class copy operator
			if (that.mError || !that.mValidFilepath) {
				fprintf(WRNLOG, "\nWarning! Copying invalid TessCntrl shader \"%s\"\n", mFilepath);
				return *this;
			}
			else if (that.mWasDecomissioned) {
				fprintf(WRNLOG, "\nWarning! Copying decomissioned TessCntrl shader \"%s\"\n", mFilepath);
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
	TesselationControlShader& TesselationControlShader::operator=(TesselationControlShader&& that) {
		if (this != &that) {
			if (!that.mValid) {
				TesselationControlShader::TesselationControlShader(that.mFilepath);
			}
			else {
				if (that.mWasDecomissioned) {
					TesselationControlShader::TesselationControlShader(that.mFilepath);
				}
				else {
					CompiledShader::operator=(that);
					that.mShaderID = 0u; //'that' loses ownership over the OpenGL shader
				}
			}
		}
		return *this;
	}

	void TesselationControlShader::aquireShaderID() {
		if (mShaderID != 0u) {
			fprintf(ERRLOG, "\nError aquiring shaderID. This shader already has ID %u\n", mShaderID);
			return;
		}
		mShaderID = glCreateShader(GL_TESS_CONTROL_SHADER);
	}

} //namespace ShaderInterface