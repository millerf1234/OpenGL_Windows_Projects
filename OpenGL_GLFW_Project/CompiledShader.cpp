#include "CompiledShader.h"

namespace ShaderInterface {

	/*CompiledShader::CompiledShader() {
		initialize();
	}*/

	CompiledShader::CompiledShader(const char * filepath) {
		initialize(filepath);
	}

	CompiledShader::~CompiledShader() {
		//New implementation
		if (!mWasDecomissioned) { //Check to make sure there is something to delete
			if (mHasBeenCompiled) {

			}
			
		}
		//Olde implementation
		////see: https://www.khronos.org/opengl/wiki/GLAPI/glGetShader   and https://www.khronos.org/opengl/wiki/GLAPI/glIsShader
		////Check to see if mID corresponds to a valid shader
		//GLboolean stillValid = glIsShader(mShaderID);
		//if (stillValid) { //If the shader is still valid
		//	GLint shaderFlaggedForDeletion; //Check to see if it has been flagged for deletion already
		//	glGetShaderiv(mShaderID, GL_DELETE_STATUS, &shaderFlaggedForDeletion);
		//		if (shaderFlaggedForDeletion != GL_TRUE) {
		//			glDeleteShader(mShaderID);
		//		}
		//}
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
		if ( (mID == that.mID) && (mID != 0u) && (strcmp(mFilepath, that.mFilepath) == 0) )
			return false;
		return true;
	}

	//void CompiledShader::deleteShader() { //Delete the shader by invalidating it
	//	mValid = false;
	//	//Check to see if mID corresponds to a valid shader
	//	if (mID != 0u) {
	//		GLboolean stillValid = glIsShader(mID);
	//		if (stillValid) { //If the shader is still valid
	//			GLint shaderFlaggedForDeletion; //Check to see if it has been flagged for deletion already
	//			glGetShaderiv(mID, GL_DELETE_STATUS, &shaderFlaggedForDeletion);
	//			if (shaderFlaggedForDeletion != GL_TRUE) {
	//				glDeleteShader(mID);
	//				stillValid = GL_FALSE;
	//			}
	//		}
	//	}
	//	mID = 0u;
	//}

	void CompiledShader::initialize(const char * filepath) {
		fprintf(MSGLOG, "\nInitializing shader for shader source file %s\n", filepath);
		//for (size_t i = 0; i < SHADER_COMPILATION_INFO_LOG_BUFFER_SIZE; i++) {
		//	mCompilationInfoLog[i] = '\0';
		//}
		mCompilationInfoLog[0] = mCompilationInfoLog[1] = '\0'; 
		initializeBooleans();
		mType = ShaderType::UNSPECIFIED;
		mShaderID = 0u; //0u will never be assigned to a valid shader
		mFilepath = filepath;
		
		

		//mValid = false;
		//mValidFilepath = false;
		//mError = false;
		//mType = ShaderType::UNSPECIFIED;

		//mID = 0u; //I am not sure if 0u is a valid OpenGL shader program handle or not. Oh well...
		//mFilepath = "\0";
		//infoLog[0] = '\0';
	}

	void CompiledShader::initializeBooleans() {
		
	}

	void CompiledShader::loadSourceFile(const char * filename, std::string& textBuffer) {
		std::ifstream shaderInputStream{ filename };
		if (this->validateFilepath(shaderInputStream)) {

		}
		textBuffer = { std::istreambuf_iterator<char>(shaderInputStream),  std::istreambuf_iterator<char>() };
	
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
		}
		return true;
	}

} //namespace ShaderInterface 



/*

//Enum for the various shader types
enum class ShaderType {VERTEX, GEOMETRY, TESSELATION_CONTROL, TESSELATION_EVALUATION, FRAGMENT, INVALID};


class CompiledShader {
public:
CompiledShader();
~CompiledShader();

const char* getFilepath() const { return mFilepath; }

bool valid() const { return mValid; }
bool validFilepath() const { return mValidFilepath; }
bool error() const { return mError; }
ShaderType type() const { return mType; }

bool operator==(const CompiledShader&); //For comparing shaders...
bool operator!=(const CompiledShader&);

//Copying not allowed but moving is okay
CompiledShader(const CompiledShader &) = delete;
CompiledShader& operator=(const CompiledShader &) = delete;
//CompiledShader(CompiledShader && that); //Not sure about this one... It can't be virtual since it's a constructor
virtual CompiledShader& operator=(CompiledShader &&) = 0;


protected:

ShaderType mType;

//Loads the text of a file located at filepath. Useful for loading shaders
std::string loadSourceFile(char * filepath) const;

private:
const char * mFilepath;
GLchar infoLog[512]; //Buffer for storing shader-compilation error messages
GLuint mID;

//Variables representing object's state:
bool mValid; //True on successful shader compilation
bool mValidFilepath; //True on successfuly opening of file handle
bool mError; //True on encountering an error

//initialize should only be called from a constructor
void initialize();

};
} //namespace ShaderInterface
*/