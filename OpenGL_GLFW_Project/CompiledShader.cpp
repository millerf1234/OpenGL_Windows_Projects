//Redone version of CompiledShader    (i.e. Version_old (written over 7/24/2018-7/27/2018) )
//
// See Header file for more details 
//
//Code by Forrest Miller           Version2 (7/29/2018 - 7/30/2018)



#include "CompiledShader.h"

namespace ShaderInterface {

	/*CompiledShader::CompiledShader() {
		initialize();
	}*/

	CompiledShader::CompiledShader(const char * sourceFilepath) {
		initialize();
		mFilepath = sourceFilepath;
		if (sourceFilepath == nullptr) {
			mError = true;
			mValidFilepath = false;
			return;
		}
		if (!loadSourceFile()) {
			fprintf(ERRLOG, "\nError reading ShaderSource \"%s\"\nNo file found at this location!\n", mFilepath);
			mError = true;
			mValidFilepath = false;
			return;
		}

		if (compile()) {
			mReadyToBeAttached = true;
		}
	}

	CompiledShader::~CompiledShader() {
		if (mShaderID.mID != 0u) {
			GLboolean shaderStillValid = glIsShader(mShaderID.mID);
			if (shaderStillValid) {
				//Check to see if a delete call is already pending with OpenGL
				GLint shaderMarkedForDeletion;
				glGetShaderiv(mShaderID.mID, GL_DELETE_STATUS, &shaderMarkedForDeletion);
				if (!shaderMarkedForDeletion) {
					//glDeleteShader(mShaderID);
				}
			}
			mShaderID = 0u;
		}
	}


	bool CompiledShader::operator==(const CompiledShader& that) const {
		if (this == &that)
			return true;  //If comparing with self, then return true
		if (this->mShaderID.mType == that.mShaderID.mType) {     //   If the types match
			if ((strcmp(mFilepath, that.mFilepath) == 0)) {  //  And the filepaths match
				return true;
			}
		}
		return false;
	}

	bool CompiledShader::operator!=(const CompiledShader& that) const {
		if (this == &that)
			return false;
		if (this->mShaderID.mType == that.mShaderID.mType) {     //   If the types match
			if ((strcmp(mFilepath, that.mFilepath) == 0)) {  //  And the filepaths match
				return false;
			}
		}
		return true;
	}

	void CompiledShader::decommision() {
		if (mIsDecomissioned || mError)
			return;
		mIsDecomissioned = true;
		mReadyToBeAttached = false;
		mSourceText = nullptr;


		GLboolean shaderStillValid = glIsShader(mShaderID.mID);
		if (shaderStillValid) {
			glDeleteShader(mShaderID.mID);
		}

		mShaderID.mID = 0u;
	}

	bool CompiledShader::isCompiled() const {
		if (mError) { return false; }
		GLboolean isShaderProgram = glIsShader(mShaderID.mID);
		if (isShaderProgram) {
			GLint compiled = 0;
			glGetShaderiv(mShaderID.mID, GL_COMPILE_STATUS, &compiled);
			if (compiled != GL_FALSE)
				return true;
		}
		return false;
	}
	
	bool CompiledShader::compile() {
		//See if for some reason this object represents a valid shader...
		if (mShaderID.mID != 0u) {
			GLboolean isShaderProgram = glIsShader(mShaderID.mID);
			if (isShaderProgram) {
				fprintf(WRNLOG, "\nWarning! Deleting shader source object %u!\n", mShaderID.mID);
				glDeleteShader(mShaderID.mID);
			}
			mShaderID.mID = 0u;
			mShaderID.mType = ShaderType::UNSPECIFIED;
		}

		aquireShaderID(); //This function is overridden by derived classes and will choose to create the correct type of shader within OpenGL

		const GLchar * rawShaderSource = mSourceText->c_str();
		glShaderSource(mShaderID.mID, 1, &rawShaderSource, NULL);
		glCompileShader(mShaderID.mID);

		return checkForCompilationErrors();
	}


	bool CompiledShader::loadSourceFile() {
		std::ifstream shaderInputStream{ mFilepath };
		if (!checkIfValidFilepath(shaderInputStream)) {
			return false;
		}
		mSourceText = std::make_unique<std::string>();
		*mSourceText = { std::istreambuf_iterator<char>(shaderInputStream),  std::istreambuf_iterator<char>() };
		return true;
	}

	void CompiledShader::initialize() {
		mShaderID = ShaderID(0u, ShaderType::UNSPECIFIED);
		mIsDecomissioned = false;
		mFilepath = "\0";
		mSourceText = nullptr;
		mError = false;
		mReadyToBeAttached = false;
		mHasFilepath = false;
		mValidFilepath = false;
	}

	bool CompiledShader::checkIfValidFilepath(std::ifstream& inFileStream) {
		try {
			inFileStream.exceptions(inFileStream.failbit);
		}
		catch (const std::ios_base::failure& e) {
			fprintf(WRNLOG, "\nWARNING! Invalid or Unreadable filepath encountered with \"%s\"\n", mFilepath);
			mValidFilepath = false;
			mError = true;
			return false;
		}
		mValidFilepath = true;
		return true;
	}


	bool CompiledShader::checkForCompilationErrors() {
		GLint success;
		glGetShaderiv(mShaderID.mID, GL_COMPILE_STATUS, &success);
		if (!success) {
			GLchar compilationInfoLog[768];
			glGetShaderInfoLog(mShaderID.mID, 768, NULL, compilationInfoLog);
			fprintf(WRNLOG, "\nSHADER COMPILATION FAILURE WARNING!\n");
			fprintf(WRNLOG, "\nThe Fragment Shader \"%s\" failed to compile...\n\t%s\n", mFilepath, compilationInfoLog);
			return false;
		}
		return true;
	}

} //namespace ShaderInterface