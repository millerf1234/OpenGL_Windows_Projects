//See Header file for detail. This class works within the UniformLocationBucket-UniformLocationTracker system
//Created by Forrest Miller on July 23, 2018

#include "CachedUniformLocation.h"


namespace ShaderInterface {

	CachedUniformLocation::CachedUniformLocation() {
		mWasInitialized = false;
		mUniformName = "\0";
		mProgramID = 0u;
		mUniformType = UniformType::UNSPECIFIED;
		mValidLocation = false;
		mCount = 0u;
		mTranspose = false;
		mUniformLocation = -1;
	}
	CachedUniformLocation::CachedUniformLocation(const GLchar * name, GLuint programID, UniformType type, GLsizei count, GLboolean transpose) {
		mWasInitialized = true;
		mUniformName = name;
		mProgramID = programID;
		mUniformType = type;
		mCount = count;
		mTranspose = transpose;

		mUniformLocation = glGetUniformLocation(programID, name);
		if (mUniformLocation == -1) {
			mValidLocation = false;
			fprintf(WRNLOG, "\nWarning! Unable to find uniform location for %s!\n", name);
		}
		else {
			mValidLocation = true;
		}
	}


	CachedUniformLocation::~CachedUniformLocation() {
		//Nothing to do here thanks to smart pointers 
	}


	void CachedUniformLocation::updateUniform(void * data) {
		if (!mWasInitialized) {
			fprintf(ERRLOG, "\nERROR! updateUniform was called on an uninitialized cached uniform!"
				"\nPlease make sure all cached uniforms are initialized before being updated!\n");
			return;
		}
		if (!mValidLocation) {
			fprintf(WRNLOG, "\nWarning! updateUniform called on uniform %s, but this uniform's location in the shader is not valid!\n", mUniformName);
			return;
		}
		//else 
		switch (mUniformType) {
		case UniformType::INT1:
			glProgramUniform1i(mProgramID, mUniformLocation, *(static_cast<GLint *>(data)));
			break;
		case UniformType::UINT1:
			glProgramUniform1ui(mProgramID, mUniformLocation, *(static_cast<GLuint *>(data)));
			break;
		case UniformType::FLOAT1:
			glProgramUniform1f(mProgramID, mUniformLocation, *(static_cast<GLfloat *>(data)));
			break;
		case UniformType::INT1V:
			glProgramUniform1iv(mProgramID, mUniformLocation, mCount, static_cast<GLint *>(data));
			break;
		case UniformType::UINT1V:
			glProgramUniform1uiv(mProgramID, mUniformLocation, mCount, static_cast<GLuint *>(data));
			break;
		case UniformType::FLOAT1V:
			glProgramUniform1fv(mProgramID, mUniformLocation, mCount, static_cast<GLfloat *>(data));
			break;



		case UniformType::INT2:
			fprintf(WRNLOG, "\nWarning! updateUniform for INT2 was called for cached uniform with only 1 data parameter.\nThere is a better way to send data like this!\n");
			glProgramUniform2i(mProgramID, mUniformLocation, *(static_cast<GLint *>(data)), *(static_cast<GLint *>(data)));
			break;
		case UniformType::UINT2:
			fprintf(WRNLOG, "\nWarning! updateUniform for UINT2 was called for cached uniform with only 1 data parameter.\nThere is a better way to send data like this!\n");
			glProgramUniform2ui(mProgramID, mUniformLocation, *(static_cast<GLint *>(data)), *(static_cast<GLint *>(data)));
			break;
		case UniformType::FLOAT2:
			fprintf(WRNLOG, "\nWarning! updateUniform for FLOAT2 was called for cached uniform with only 1 data parameter.\nThere is a better way to send data like this!\n");
			glProgramUniform2f(mProgramID, mUniformLocation, *(static_cast<GLfloat *>(data)), *(static_cast<GLfloat *>(data)));
			break;
		case UniformType::INT2V:
			glProgramUniform2iv(mProgramID, mUniformLocation, mCount, static_cast<GLint *>(data));
			break;
		case UniformType::UINT2V:
			glProgramUniform2uiv(mProgramID, mUniformLocation, mCount, static_cast<GLuint *>(data));
			break;
		case UniformType::FLOAT2V:
			glProgramUniform2fv(mProgramID, mUniformLocation, mCount, static_cast<GLfloat *>(data));
			break;



		case UniformType::INT3:
			fprintf(WRNLOG, "\nWarning! updateUniform for INT3 was called for cached uniform with only 1 data parameter.\nThere is a better way to send data like this!\n");
			glProgramUniform3i(mProgramID, mUniformLocation, *(static_cast<GLint *>(data)), *(static_cast<GLint *>(data)), *(static_cast<GLint *>(data)));
			break;
		case UniformType::UINT3:
			fprintf(WRNLOG, "\nWarning! updateUniform for UINT3 was called for cached uniform with only 1 data parameter.\nThere is a better way to send data like this!\n");
			glProgramUniform3ui(mProgramID, mUniformLocation, *(static_cast<GLint *>(data)), *(static_cast<GLint *>(data)), *(static_cast<GLint *>(data)));
			break;

		case UniformType::FLOAT3:
			fprintf(WRNLOG, "\nWarning! updateUniform for FLOAT3 was called for cached uniform with only 1 data parameter.\nThere is a better way to send data like this!\n");
			glProgramUniform3f(mProgramID, mUniformLocation, *(static_cast<GLfloat *>(data)), *(static_cast<GLfloat *>(data)), *(static_cast<GLfloat *>(data)));
			break;

		case UniformType::INT3V:
			glProgramUniform3iv(mProgramID, mUniformLocation, mCount, static_cast<GLint *>(data));
			break;
		case UniformType::UINT3V:
			glProgramUniform3uiv(mProgramID, mUniformLocation, mCount, static_cast<GLuint *>(data));
			break;
		case UniformType::FLOAT3V:
			glProgramUniform3fv(mProgramID, mUniformLocation, mCount, static_cast<GLfloat *>(data));
			break;



		case UniformType::INT4:
			fprintf(WRNLOG, "\nWarning! updateUniform for INT4 was called for cached uniform with only 1 data parameter.\nThere is a better way to send data like this!\n");
			glProgramUniform4i(mProgramID, mUniformLocation, *(static_cast<GLint *>(data)), *(static_cast<GLint *>(data)), *(static_cast<GLint *>(data)), *(static_cast<GLint *>(data)));
			break;
		case UniformType::UINT4:
			fprintf(WRNLOG, "\nWarning! updateUniform for UINT4 was called for cached uniform with only 1 data parameter.\nThere is a better way to send data like this!\n");
			glProgramUniform4ui(mProgramID, mUniformLocation, *(static_cast<GLint *>(data)), *(static_cast<GLint *>(data)), *(static_cast<GLint *>(data)), *(static_cast<GLint *>(data)));
			break;
		case UniformType::FLOAT4:
			fprintf(WRNLOG, "\nWarning! updateUniform for FLOAT4 was called for cached uniform with only 1 data parameter.\nThere is a better way to send data like this!\n");
			glProgramUniform4f(mProgramID, mUniformLocation, *(static_cast<GLfloat *>(data)), *(static_cast<GLfloat *>(data)), *(static_cast<GLfloat *>(data)), *(static_cast<GLfloat *>(data)));
			break;
		case UniformType::INT4V:
			glProgramUniform4iv(mProgramID, mUniformLocation, mCount, static_cast<GLint *>(data));
			break;
		case UniformType::UINT4V:
			glProgramUniform4uiv(mProgramID, mUniformLocation, mCount, static_cast<GLuint *>(data));
			break;
		case UniformType::FLOAT4V:
			glProgramUniform4fv(mProgramID, mUniformLocation, mCount, static_cast<GLfloat *>(data));
			break;


		case UniformType::MAT2:
			glProgramUniformMatrix2fv(mProgramID, mUniformLocation, mCount, mTranspose, static_cast<GLfloat *>(data));
			break;
		case UniformType::MAT2X3:
			glProgramUniformMatrix2x3fv(mProgramID, mUniformLocation, mCount, mTranspose, static_cast<GLfloat *>(data));
			break;

		case UniformType::MAT2X4:
			glProgramUniformMatrix2x4fv(mProgramID, mUniformLocation, mCount, mTranspose, static_cast<GLfloat *>(data));
			break;

		case UniformType::MAT3X2:
			glProgramUniformMatrix3x2fv(mProgramID, mUniformLocation, mCount, mTranspose, static_cast<GLfloat *>(data));
			break;

		case UniformType::MAT3:
			glProgramUniformMatrix3fv(mProgramID, mUniformLocation, mCount, mTranspose, static_cast<GLfloat *>(data));
			break;

		case UniformType::MAT3X4:
			glProgramUniformMatrix3x4fv(mProgramID, mUniformLocation, mCount, mTranspose, static_cast<GLfloat *>(data));
			break;

		case UniformType::MAT4X2:
			glProgramUniformMatrix4x2fv(mProgramID, mUniformLocation, mCount, mTranspose, static_cast<GLfloat *>(data));
			break;
		case UniformType::MAT4X3:
			glProgramUniformMatrix4x3fv(mProgramID, mUniformLocation, mCount, mTranspose, static_cast<GLfloat *>(data));
			break;
		case UniformType::MAT4:
			glProgramUniformMatrix4fv(mProgramID, mUniformLocation, mCount, mTranspose, static_cast<GLfloat *>(data));
			break;
		case UniformType::UNSPECIFIED:
			fprintf(WRNLOG, "\nWarning! Update was called for a cached uniform location of type UNSPECIFIED! This means no uniform update will occur.\n");
			fprintf(WRNLOG, "This message is regarding the uniform variable %s\n", mUniformName);
			break;
		}
	}


	void CachedUniformLocation::updateUniform(void * xPtr, void * yPtr) { //For uniform2i, uniform2ui, uniform2f only
		if (!mWasInitialized) {
			fprintf(ERRLOG, "\nERROR! updateUniform was called on an uninitialized cached uniform!"
				"\nPlease make sure all cached uniforms are initialized before being updated!\n");
			return;
		}
		if (!mValidLocation) {
			fprintf(WRNLOG, "\nWarning! updateUniform called on uniform %s, but this uniform's location in the shader is not valid!\n", mUniformName);
			return;
		}

		//else  
		switch (mUniformType) {
		case UniformType::INT2:
			return;
		case UniformType::UINT2:
			return;
		case UniformType::FLOAT2:
			return;
		case UniformType::UNSPECIFIED: //Print out special UNSPECIFIED message to WRN/ERRLOG?
			[[fallthrough]];
		default:
			fprintf(WRNLOG, "\nWarning! update Uniform was called for an update that takes 2 variables but was set to non-complient type!\n");
			return;
		}
	}
	void CachedUniformLocation::updateUniform(void * xPtr, void * yPtr, void * zPtr) { //For uniform3i, uniform3ui, uniform3f only
		if (!mWasInitialized) {
			fprintf(ERRLOG, "\nERROR! updateUniform was called on an uninitialized cached uniform!"
				"\nPlease make sure all cached uniforms are initialized before being updated!\n");
			return;
		}
		if (!mValidLocation) {
			fprintf(WRNLOG, "\nWarning! updateUniform called on uniform %s, but this uniform's location in the shader is not valid!\n", mUniformName);
			return;
		}
		switch (mUniformType) {


		default:

			return;
		}
	}
	void CachedUniformLocation::updateUniform(void * xPtr, void * yPtr, void * zPtr, void * wPtr) { ////For uniform4i, uniform4ui, uniform4f only
		if (!mWasInitialized) {
			fprintf(ERRLOG, "\nERROR! updateUniform was called on an uninitialized cached uniform!"
				"\nPlease make sure all cached uniforms are initialized before being updated!\n");
			return;
		}
		if (!mValidLocation) {
			fprintf(WRNLOG, "\nWarning! updateUniform called on uniform %s, but this uniform's location in the shader is not valid!\n", mUniformName);
			return;
		}

		switch (mUniformType) {


		default:

			return;
		}
	}



} //namespace ShaderInterface 