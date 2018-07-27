//Implementation file for the Geometry Shader class.
//The Geometry Shader class inherits from the CompiledShader class\
//
//Created by Forrest Miller on July 26, 2018

#include "GeometryShader.h"

namespace ShaderInterface {

	GeometryShader::GeometryShader(const char * filepath) : CompiledShader(filepath) {
		mType = ShaderType::GEOMETRY;
		if (mHasLoadedSourceText) //if the base constructor was able to load the file
			compile();
	}

	GeometryShader::GeometryShader(const GeometryShader& that) : CompiledShader(that.mFilepath) {
		if (that.mValidFilepath) {
			GeometryShader::GeometryShader(that.mFilepath);
			mType = ShaderType::GEOMETRY;
		}
		else {
			mValidFilepath = false;
			mType = ShaderType::GEOMETRY;
		}

	}
	GeometryShader::GeometryShader(GeometryShader&& that) : CompiledShader(that.mFilepath) {
		if (that.mValid) {
			mShaderID = that.mShaderID;
			that.mShaderID = 0u;
			that.mValid = false;
		}
		else {
			mValid = false;
			mType = ShaderType::GEOMETRY;
		}
	}

	GeometryShader::~GeometryShader() {

	}

	//Decomissions this geom shader object
	void GeometryShader::decommision() {
		if (mWasDecomissioned) {
			return;
		}
		else {
			mSourceText = "\0";
			mHasBeenCompiled = false;
			mHasLoadedSourceText = false;
			mValid = false;
			glDeleteShader(mShaderID);
			mShaderID = 0u;
			mWasDecomissioned = true;
		}
	}
	void GeometryShader::reinstate() {
		if (!mWasDecomissioned) {
			fprintf(WRNLOG, "\nWarning! Unable to reinstate geometry shader \"%s\" because it was never decomissioned!\n", mFilepath);
			return;
		}
		else {
			GeometryShader::GeometryShader(mFilepath); //Reconstruct this object to reinstate it
			mWasDecomissioned = false;
		}
	}

	//Assigns a different ID to this shader if need be
	GeometryShader& GeometryShader::operator=(const GeometryShader& that) {
		if (this != &that) {
			CompiledShader::operator=(that); //Call the base class copy operator
			if (that.mError || !that.mValidFilepath) {
				fprintf(WRNLOG, "\nWarning! Copying invalid Geometry shader \"%s\"\n", mFilepath);
				return *this;
			}
			else if (that.mWasDecomissioned) {
				fprintf(WRNLOG, "\nWarning! Copying decomissioned Geometry shader \"%s\"\n", mFilepath);
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
	GeometryShader& GeometryShader::operator=(GeometryShader&& that) {
		if (this != &that) {
			if (!that.mValid) {
				GeometryShader::GeometryShader(that.mFilepath);
			}
			else {
				if (that.mWasDecomissioned) {
					GeometryShader::GeometryShader(that.mFilepath);
				}
				else {
					CompiledShader::operator=(that);
					that.mShaderID = 0u; //'that' loses ownership over the OpenGL shader
				}
			}
		}
		return *this;
	}

	void GeometryShader::aquireShaderID() {
		if (mShaderID != 0u) {
			fprintf(ERRLOG, "\nError aquiring shaderID. This shader already has ID %u\n", mShaderID);
			return;
		}
		mShaderID = glCreateShader(GL_GEOMETRY_SHADER);
	}

} //namespace ShaderInterface