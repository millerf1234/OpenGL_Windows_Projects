#include "CompiledShader.h"

namespace ShaderInterface {

	CompiledShader::CompiledShader(const char * filepath) {
		initialize(filepath);
	}

	CompiledShader::~CompiledShader() {
		//see: https://www.khronos.org/opengl/wiki/GLAPI/glGetShader   
		// and https://www.khronos.org/opengl/wiki/GLAPI/glIsShader

		//All the base constructor will do is free the programID if necessary
		if (mShaderID != 0u) {
			GLboolean shaderStillValid = glIsShader(mShaderID);
			if (shaderStillValid) {
				//Check to see if a delete call is already pending with OpenGL
				GLint shaderMarkedForDeletion;
				glGetShaderiv(mShaderID, GL_DELETE_STATUS, &shaderMarkedForDeletion);
				if (!shaderMarkedForDeletion) {
					//glDeleteShader(mShaderID);
				}
			}
			mShaderID = 0u;
		}
	}

	//Decomissions this shader object (i.e. free's it's resources and prevent's it from being attached to new programs
	void CompiledShader::decommision() {
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

	bool CompiledShader::operator==(const CompiledShader& that) const {
		if (this == &that)  
			return true;  //If comparing with self, then return true
		else if (mType == that.mType) {
			if (strcmp(mFilepath, that.mFilepath) == 0) { //Same filepath
				return true;
			}
		}
		return false;
	}

	bool CompiledShader::operator!=(const CompiledShader& that) const {
		if (this == &that) 
			return false;
		if (this->mType == that.mType) {                     //   If the types match
			if ((strcmp(mFilepath, that.mFilepath) == 0)) {  //  And the filepaths match
				return false;
			}
		}
		return true;
	}

	bool CompiledShader::compile() {
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

		//Check to see if this ShaderID is valid
		GLboolean shaderStillValid = glIsShader(mShaderID);
		if (shaderStillValid) {
			fprintf(WRNLOG, "\nWARNING! Unable to compile shader \"%s\" because ShaderID already represents a program!\n", mFilepath);
			return true;
		}
		aquireShaderID(); //This function's implementation will choose the appropriate glFunctionCall to get the appropriate type of shader handle
		
		const GLchar* rawShaderSource = mSourceText.c_str();
		glShaderSource(mShaderID, 1, &rawShaderSource, NULL);
		glCompileShader(mShaderID);

		checkForCompilationErrors();
		return mValid;
	}


	void CompiledShader::initialize(const char * filepath) {
		fprintf(MSGLOG, "\nInitializing shader for shader source file %s\n", filepath);
		//mCompilationInfoLog[0] = '\0';
		//mCompilationInfoLog[1] = '\0';
		//mCompilationInfoLog[SHADER_COMPILATION_INFO_LOG_BUFFER_SIZE - INDEX_SHIFT] = '\0';
		initializeBooleans();
		mType = ShaderType::UNSPECIFIED;
		mShaderID = 0u; //0u will never be assigned to a valid shader
		mFilepath = filepath;
		//Try to open the file
		loadSourceFile(filepath, mSourceText);
		if (mHasLoadedSourceText) {
			fprintf(MSGLOG, "Shader filepath successfully read from. Preparing to compile shader...\n");
		}
		else {
			fprintf(ERRLOG, "\nERROR! Unable to read from shader file \"%s\"\nPlease ensure that a valid filepath is used.\n", filepath);
			mError = true;
			mValid = false;
			return;
		}
	}

	void CompiledShader::initializeBooleans() {
		mError = false;
		mValid = false;
		mValidFilepath = false;
		mHasLoadedSourceText = false;
		mHasBeenCompiled = false;
		mWasDecomissioned = false;
	}

	void CompiledShader::loadSourceFile(const char * filename, std::string& textBuffer) {
		std::ifstream shaderInputStream{ filename };
		if (this->validateFilepath(shaderInputStream)) {
			textBuffer = { std::istreambuf_iterator<char>(shaderInputStream),  std::istreambuf_iterator<char>() };
			if (textBuffer.length() > 20ul) //I am using 20 as a cutoff for shortest allowed shader length in characters
				mHasLoadedSourceText = true;
		}
		else { //if unable to validate the filepath
			mValidFilepath = false;
			mHasLoadedSourceText = false;
			mError = true;
			mValid = false;
			textBuffer = "\0";
			//textBuffer = "#version 450 core\nvoid main() {\n\t;\n}\n\0"; //Throw in a bogus empty shader 
		}
	}
	
	//This function is to be called only right after glCompileShader is called.
	bool CompiledShader::checkForCompilationErrors() {
		GLint success;
		glGetShaderiv(mShaderID, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(mShaderID, SHADER_COMPILATION_INFO_LOG_BUFFER_SIZE, NULL, mCompilationInfoLog);
			fprintf(WRNLOG, "\nSHADER COMPILATION FAILURE WARNING!\n");
			fprintf(WRNLOG, "\nThe Fragment Shader \"%s\" failed to compile...\n\t%s\n", mFilepath, mCompilationInfoLog);
			mHasBeenCompiled = false;
			mValid = false;
			return false;
		}
		mHasBeenCompiled = true;
		mValid = true;
		return true;
	}


	//This function is based off the example listed at: https://en.cppreference.com/w/cpp/io/ios_base/failure
	bool CompiledShader::validateFilepath(std::ifstream& inFileStream) {
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


} //namespace ShaderInterface 
