#include "CompiledShader.h"

namespace ShaderInterface {

	CompiledShader::CompiledShader() {
		initialize();
	}


	CompiledShader::~CompiledShader() {
		//see: https://www.khronos.org/opengl/wiki/GLAPI/glGetShader   and https://www.khronos.org/opengl/wiki/GLAPI/glIsShader
		//Check to see if mID corresponds to a valid shader
		GLboolean stillValid = glIsShader(mID);
		if (stillValid) { //If the shader is still valid
			GLint shaderFlaggedForDeletion; //Check to see if it has been flagged for deletion already
			glGetShaderiv(mID, GL_DELETE_STATUS, &shaderFlaggedForDeletion);
				if (shaderFlaggedForDeletion != GL_TRUE) {
					glDeleteShader(mID);
				}
		}
	}

	bool CompiledShader::operator==(const CompiledShader& that) {
		if (this == &that)  
			return true;  //If comparing with self, then return true
		else if (mType == that.mType) {
			if (strcmp(mFilepath, that.mFilepath) == 0) { //Same filepath
				return true;
			}
		}
		return false;
	}

	bool CompiledShader::operator!=(const CompiledShader& that) {
		if (this == &that) 
			return false;
		if ( (mID == that.mID) && (mID != 0u) && (strcmp(mFilepath, that.mFilepath) == 0) )
			return false;
		return true;
	}

	void CompiledShader::deleteShader() {
		mValid = false;
		//Check to see if mID corresponds to a valid shader
		if (mID != 0u);
		GLboolean stillValid = glIsShader(mID);
		if (stillValid) { //If the shader is still valid
			GLint shaderFlaggedForDeletion; //Check to see if it has been flagged for deletion already
			glGetShaderiv(mID, GL_DELETE_STATUS, &shaderFlaggedForDeletion);
			if (shaderFlaggedForDeletion != GL_TRUE) {
				glDeleteShader(mID);
			}
		}
		mID = 0u;
	}


	std::string CompiledShader::loadSourceFile(char * filename) const {
		std::ifstream shaderInputStream{ filename };
		return { std::istreambuf_iterator<char>(shaderInputStream),  std::istreambuf_iterator<char>() };
	}


	void CompiledShader::initialize() {
		mValid = false;
		mValidFilepath = false;
		mError = false;
		mType = ShaderType::UNSPECIFIED;

		mID = 0u; //I am not sure if 0u is a valid OpenGL shader program handle or not. Oh well...
		mFilepath = "\0";
		infoLog[0] = '\0';
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