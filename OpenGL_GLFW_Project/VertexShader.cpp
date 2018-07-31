//Implementation file for the Vertex Shader class.
//The Vertex Shader class inherits from the CompiledShader class
//
//Created by Forrest Miller on July 26, 2018
//Rewritten by Forrest Miller on July 30, 2018

#include "VertexShader.h"

namespace ShaderInterface {

	VertexShader::VertexShader(const char * filepath) : CompiledShader(filepath) {
		
	}
	
	VertexShader::VertexShader(VertexShader&& other) : CompiledShader() {
		this->mFilepath = other.mFilepath;
		this->mShaderID = other.shaderID;
		other.shaderID = ShaderID(0u, ShaderType::UNSPECIFIED);
		if (other.mShaderID.mType != ShaderType::VERTEX) {
			this->mError = true;
		}
		else {
			this->mError = other.mError;
		}
		this->mIsDecomissioned = other.mIsDecomissioned;
		if (this->mShaderID.mID != 0u)
			this->mReadyToBeAttached = other.mReadyToBeAttached;
	}

	VertexShader::~VertexShader() {

	}

	void VertexShader::reinstate() {
		if (!mIsDecomissioned) {
			fprintf(WRNLOG, "\nWarning! Reinstate() was called on shader \"%s\" even though this shader was never decomissioned!\n", mFilepath);
			return;
		}
		else if (!mValidFilepath) {
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

	VertexShader& VertexShader::operator=(VertexShader&& that) {
		if (this != &that) {
			this->mValidFilepath = that.mValidFilepath;
			this->mFilepath = that.mFilepath;
			this->mIsDecomissioned = that.mIsDecomissioned;

			if (that.mSourceText == nullptr) {
				this->mSourceText = nullptr;
				if (that.mValidFilepath) {
					this->makeSureShaderSourceTextIsLoaded(); //Reload the text
				}
			}
			if (that.mShaderID.mType != ShaderType::VERTEX) {
				this->mError = true;
				this->mReadyToBeAttached = false;
				this->mValidFilepath = that.mValidFilepath;
				this->mFilepath = that.mFilepath;
				this->mIsDecomissioned = that.mIsDecomissioned;
			}
			else {
				this->mError = that.mError;
				this->mReadyToBeAttached = that.mReadyToBeAttached;
				this->mShaderID.mID = that.mShaderID.mID;
				this->mShaderID.mType = that.mShaderID.mType;
				that.mShaderID.mID = 0u; //Remove ownership from 'that' VertexShader
			}
		}
	}

	void VertexShader::aquireShaderID() {
		if (mShaderID.mID != 0u) {
			fprintf(ERRLOG, "\nError aquiring shaderID. This shader already has ID %u\n", mShaderID);
			return;
		}
		mShaderID = glCreateShader(GL_VERTEX_SHADER);
	}

	bool VertexShader::makeSureShaderSourceTextIsLoaded() {
		if (mSourceText == nullptr) {
			loadSourceFile();
		}
	}

	//OLDE
	//VertexShader::VertexShader(const char * filepath) : CompiledShader(filepath) {
	//	mType = ShaderType::VERTEX;
	//	if (mHasLoadedSourceText)
	//		compile();
	//}

	//VertexShader::VertexShader(const VertexShader& that) : CompiledShader(that.mFilepath) {
	//	if (that.mValidFilepath) {
	//		VertexShader::VertexShader(that.mFilepath);
	//		mType = ShaderType::VERTEX;
	//	}
	//	else {
	//		mValidFilepath = false;
	//		mType = ShaderType::VERTEX;
	//	}

	//}
	//VertexShader::VertexShader(VertexShader&& that) : CompiledShader(that.mFilepath) {
	//	if (that.mValid) {
	//		mShaderID = that.mShaderID;
	//		that.mShaderID = 0u;
	//		that.mValid = false;
	//	}
	//	else {
	//		mValid = false;
	//		mType = ShaderType::VERTEX;
	//	}
	//}

	//VertexShader::~VertexShader() {

	//}

	//void VertexShader::reinstate() {
	//	if (!mWasDecomissioned) {
	//		return;
	//	}
	//	else {
	//		VertexShader::VertexShader(mFilepath);
	//		mWasDecomissioned = false;
	//	}
	//}

	////Assigns a different ID to this shader if need be
	//VertexShader& VertexShader::operator=(const VertexShader& that) {
	//	if (this != &that) {
	//		CompiledShader::operator=(that); //Call the base class copy operator
	//		if (that.mError || !that.mValidFilepath) {
	//			fprintf(WRNLOG, "\nWarning! Copying invalid VERTEX shader \"%s\"\n", mFilepath);
	//			return *this;
	//		}
	//		else if (that.mWasDecomissioned) {
	//			fprintf(WRNLOG, "\nWarning! Copying decomissioned Vertex shader \"%s\"\n", mFilepath);
	//			return *this;
	//		}
	//		else if (that.mShaderID != 0u) {
	//			if (mHasLoadedSourceText) {
	//				mValid = mHasBeenCompiled = compile();
	//			}
	//			else { //else something is screwed up so we gotta reload the shader from the file
	//				loadSourceFile(mFilepath, mSourceText);
	//				if (mHasLoadedSourceText) {
	//					mValid = mHasBeenCompiled = compile();
	//				}
	//			}
	//		}
	//	}
	//	return *this;
	//}
	//VertexShader& VertexShader::operator=(VertexShader&& that) {
	//	if (this != &that) {
	//		if (!that.mValid) {
	//			VertexShader::VertexShader(that.mFilepath);
	//		}
	//		else {
	//			if (that.mWasDecomissioned) {
	//				VertexShader::VertexShader(that.mFilepath);
	//			}
	//			else {
	//				CompiledShader::operator=(that);
	//				that.mShaderID = 0u; //That loses ownership over the OpenGL shader
	//			}
	//		}
	//	}
	//	return *this;
	//}

	//void VertexShader::aquireShaderID() {
	//	if (mShaderID != 0u) {
	//		fprintf(ERRLOG, "\nError aquiring shaderID. This shader already has ID %u\n", mShaderID);
	//		return;
	//	}
	//	mShaderID = glCreateShader(GL_VERTEX_SHADER);
	//}

} //namespace ShaderInterface