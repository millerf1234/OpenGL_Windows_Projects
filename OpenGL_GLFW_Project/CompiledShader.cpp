//Class: CompiledShader
//Namespace: ShaderInterface
//Programmer: Forrest Miller
//Date(s): 7/24/2018 - 7/31/2018  
//
// 9/14/2018  --  Added the option to convert to a secondary shader 
//
//This is the implementation file for CompiledShader.
//See the header file for more details.
//
// Implementation notes:  Not much to note here. 


#include "CompiledShader.h"

namespace ShaderInterface {

	void CompiledShader::initialize() {
		mShaderID = ShaderID(0u, ShaderType::UNASSIGNED);
		mIsDecomissioned = false;
		mFilepath = "\0";
		mSourceText = nullptr;
		mError = false;
		mReadyToBeAttached = false;
		mValidFilepath = false;
		mMarkedAsSecondary = false;
	}

	CompiledShader::CompiledShader(const char * sourceFilepath, GLenum type) {
		initialize();
		mShaderID.mType = ShaderType::UNASSIGNED;
		mFilepath = sourceFilepath;
		if (sourceFilepath == nullptr) {
			fprintf(ERRLOG, "\nERROR! Attempting to construct shader from a NULL filepath!\n");
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

		if (compile(type)) {
			mReadyToBeAttached = true;
		}
		else {
			mError = true;
			mReadyToBeAttached = false;
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
					glDeleteShader(mShaderID.mID); //This marks the shader for deletion by the GL Context
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



	bool CompiledShader::operator<(const CompiledShader& other) const {
		if (this->mError || !(this->mValidFilepath)) {
			if (other.mError || (other.mValidFilepath))  //if both are invalid
				return compareFilepaths(this->mFilepath, other.mFilepath);
			else //If this is invalid (and other is valid)
				return false;
		}
		else if (other.mError || !(other.mValidFilepath)) //If other is invalid
			return true; 
		else if (this->mShaderID.mType == other.mShaderID.mType) //If the shaders have the same type
				return compareFilepaths(this->mFilepath, other.mFilepath);
		else { //If both shaders are valid but have different types	
			switch (this->mShaderID.mType) {
				// The ordering is first done by derived type in the order :
				//Compute < Fragment < Geometry < TessC < TessE < Vertex < UNASSIGNED < ShaderWithError
			case ShaderType::COMPUTE: 
				return true;													// COMPUTE < {All Other Shaders}
			case ShaderType::FRAGMENT:                                         
				return (other.mShaderID.mType != ShaderType::COMPUTE);         // {COMPUTE} < FRAGMENT < {All Other Shaders}
			case ShaderType::GEOMETRY:
				return ((other.mShaderID.mType != ShaderType::COMPUTE) ||      // {COMPUTE, FRAGMENT} < GEOMETRY < {All Other Shaders}
					(other.mShaderID.mType != ShaderType::FRAGMENT));																	//etc...
			case ShaderType::TESSELATION_CONTROL:
				return ((other.mShaderID.mType != ShaderType::COMPUTE) ||
					(other.mShaderID.mType != ShaderType::FRAGMENT) ||
					(other.mShaderID.mType != ShaderType::GEOMETRY));
			case ShaderType::TESSELATION_EVALUATION:
				return ((other.mShaderID.mType == ShaderType::VERTEX) ||
					(other.mShaderID.mType == ShaderType::UNASSIGNED));
			case ShaderType::VERTEX:
				return (other.mShaderID.mType == ShaderType::UNASSIGNED);
			case ShaderType::UNASSIGNED:
				[[fallthrough]]; //C++17 required
			default:
				return false;
			}
		}
	}



	bool CompiledShader::operator>(const CompiledShader& other) const {
		return !( *this < other);
	}


	//(Note to future self: I notice that this operator does not have a return type. I should find
	//						out if this is just a quirk in the syntax (akin to the dummy 'int' used
	//						to destinguish between pre-increment and post-increment) or if there is 
	//						actually a temporary bool variable being constructed.
	CompiledShader::operator bool() const {
		return mReadyToBeAttached;
	}

	bool CompiledShader::operator!() const {
		return mReadyToBeAttached;
	}

	void CompiledShader::decommision() {
		if (mIsDecomissioned || mError || !mValidFilepath)
			return;
		mIsDecomissioned = true;
		mReadyToBeAttached = false;
		mSourceText.release(); //Frees the string containing this objects source text

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



	//Protected default constructor that is to be called by derived types only. This constructor does not create a valid object, 
	//instead it creates an object that is intended to serve as a dummy for another objects member variables to be moved over to.
	CompiledShader::CompiledShader() {
		mError = true;
		mShaderID = ShaderID(0u, ShaderType::UNASSIGNED);
		mReadyToBeAttached = false;
		mValidFilepath = false;
		mIsDecomissioned = false;
		mSourceText = nullptr;
		mFilepath = nullptr;
	}



	bool CompiledShader::compile(GLenum type) {
		//See if for some reason this object represents a valid shader...
		if (mShaderID.mID != 0u) {
			GLboolean isShaderProgram = glIsShader(mShaderID.mID);
			if (isShaderProgram) {
				fprintf(WRNLOG, "\nWarning! Deleting shader source object %u!\n", mShaderID.mID);
				glDeleteShader(mShaderID.mID);
			}
			mShaderID.mID = 0u;
			mShaderID.mType = ShaderType::UNASSIGNED;
		}

		aquireShaderID(type);
		//aquireShaderID(); //This function is overridden by derived classes and will choose to create the correct type of shader within OpenGL

		const GLchar * rawShaderSource = mSourceText->c_str();
		glShaderSource(mShaderID.mID, 1, &rawShaderSource, NULL);
		glCompileShader(mShaderID.mID);

		return checkForCompilationErrors();
	}


	void CompiledShader::aquireShaderID(GLenum type) {
		if (mShaderID.mID != 0u) {
			fprintf(WRNLOG, "\nWarning! Unable to aquire a new shaderID for shader:\n\t\"%s\"\nbecause this shader already has ID %u!", mFilepath, mShaderID.mID);
			mReadyToBeAttached = false;
			return;
		}
		mShaderID = glCreateShader(type);
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


	void CompiledShader::copyMemberVariables(CompiledShader& source) {
		this->mShaderID.mID = source.mShaderID.mID;
		this->mShaderID.mType = source.mShaderID.mType;
		this->mIsDecomissioned = source.mIsDecomissioned;
		this->mFilepath = source.mFilepath;
		this->mSourceText = std::move(source.mSourceText);
		this->mError = source.mError;
		this->mReadyToBeAttached = source.mReadyToBeAttached;
		this->mValidFilepath = source.mValidFilepath;
	}



	void CompiledShader::invalidateCompiledShaderAfterCopying() {
		mShaderID = ShaderID(0u, ShaderType::UNASSIGNED);
		mIsDecomissioned = false;
		mFilepath = nullptr;
		mSourceText = nullptr;
		mError = true;
		
		mReadyToBeAttached = false;
		mValidFilepath = false;
	}



	bool CompiledShader::checkIfValidFilepath(std::ifstream& inFileStream) {
		try {
			inFileStream.exceptions(inFileStream.failbit);
		}
		catch (const std::ios_base::failure& e [[maybe_unused]]) { //[[maybe_unused]] requires C++17
			fprintf(WRNLOG, "\nWARNING! Invalid or Unreadable filepath encountered with \"%s\"\n", mFilepath);
			mValidFilepath = false;
			mError = true;
			mReadyToBeAttached = false;
			mShaderID.mID = 0u;
			return false;
		}
		mValidFilepath = true;
		return true;
	}



	bool CompiledShader::checkForCompilationErrors() {
		GLint success;
		glGetShaderiv(mShaderID.mID, GL_COMPILE_STATUS, &success);
		if (!success) {
			mError = true;
			mReadyToBeAttached = false;
			GLchar compilationInfoLog[768];
			glGetShaderInfoLog(mShaderID.mID, 768, NULL, compilationInfoLog);
			fprintf(WRNLOG, "\nSHADER COMPILATION FAILURE WARNING!\n");
			fprintf(WRNLOG, "\nThe Fragment Shader \"%s\" failed to compile...\n\t%s\n", mFilepath, compilationInfoLog);
			return false;
		}
		mReadyToBeAttached = true; //Important to keep this here in case derived types rely on it
		return true;
	}



	bool CompiledShader::compareFilepaths(const char * fp1, const char * fp2) {
		if (fp1 == nullptr)
			return false;
		else if (fp2 == nullptr)
			return true;
		else {
			int result = strcmp(fp1, fp2); //Behavior is undefined if strcmp is passed NULL parameter
			if (result < 0)
				return true;
			else
				return false;
		}
	}

} //namespace ShaderInterface