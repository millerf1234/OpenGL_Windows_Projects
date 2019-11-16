//
// FILE:                                               UniformLocationTracker.h
//                                                                             
// Description:                    Implementation of the UniformLocationTracker
//                                class. This class is designed with the intent
//                               that it can be used by an OpenGL ShaderProgram
//                                 to simplify and streamline the interface for
//                               assigning and modifying its uniform variables.
//                                                                             
//                                                                             
//                                                                             
// PROGRAMMER:                                               Forrest Miller    
//                                                                             
// DATES:                                                                      
//                                                  July 20, 2018 -> Work Began
//                              July 24, 2018 -> Full Functionality Implemented
//                                                                             
//         NO NOTEWORTHY CHANGES MADE UNTIL ONE YEAR LATER ON...               
//                                                         July 24, 2019 Update 
//                      Over the past year of using this class, I have found it 
//                   to perform flawlessly for performing its purpose. However,
//                        this code was written quite a long time ago now and I
//                 feel it is time to update this code. See the header file for
//                                                         the full change-log.




#include "UniformLocationTracker.h"
#include <stdexcept>
#include <cassert>

//                      IF USING OPENGL 4.1 OR NEWER
//
//  If we are fortunate enough to be able to restrict our Application to require
//support for OpenGL 4.1 or newer, then we are able to completely avoid the 
//issue described below by using the new-as-of-OpenGL-4.1 function 
//'glProgramUniform()'. Usage of this function completely solves the issue 
//that is discussed below in depth by allowing the ShaderProgram which the 
//uniform to be updated belongs to as part of the update function call.
// 
//
//
//      IF USING A VERSION OF OPENGL OLDER THAN 4.1, PLEASE READ THE FOLLOWING 
//
//  Traditionally OpenGL would only allow for uniforms to be updated for which-
//ever shader program was currently bound to the context using 
//'glUseProgram()'. Seeing as how this class's purpose is to allow a
//ShaderProgram an easy interface for updating its uniforms, being restricted 
//to allowing updates to the currently bound ShaderProgram presents a 
//dilemma to how this class should be implemented. If any of the functions in 
//this class are called while the ShaderProgram owning the class instance is
//not the actively bound shader program, bad things happen. At best the context
//will see that the type of the glUniform update function fails to match the
//type of the uniform at that same location in the actively bound ShaderProgram,
//in which case the context emits a warning. At worst the types will match and 
//then there will be bogus values assigned to shaders without explanation.
//
//  The safest way to prevent this from happening would be to stick an 
//additional GL function call for binding the ShaderProgram owning this
//object right before each call to 'glUniformXX()', which would completely
//eliminate any possibility of the wrong Uniforms being updated unintentionally. 
//However, this approach has several major downsides. The first downside would
//be a possibly noticeable hit in performance, especially if the number of
//Uniforms that need updating requires repeated UPDATE_UNIFORM calls. This is
//clearly far from ideal since in most scenarios we typically should expect all
//of a ShaderProram's uniforms to be updated at the same time. If taking this 
//performance hit seems like a worthwhile price to pay for the increased safety 
//then consider this additional downside: simply updating a program's uniforms 
//will cause possibly unexpected changes in the Context's state by changing 
//the actively bound program. I suppose this could be countered by then always
//making sure to unbind the ShaderProgram from the context after the uniform 
//update has completed, but then how would it be known which ShaderProgram to 
//rebind back in its place? And what if this ShaderProgram was bound already
//when the update function gets called? To truly get the safest and most robust 
//implementation, it would be necessary to perform the following steps:
//  i)  Query the context as to which ShaderProgram is active and remember it,
// ii)  Bind the ShaderProgram owning the instance of this class
//iii)  Perform the update to the uniform
// iv)  Rebind the ShaderProgram that was bound upon entering this function.
//As you can probably see, this is quite a bit of avoidable overhead just 
//for simply performing uniform updates.
//
//  Rather than go with the safest approach, I decided to go with a 
//compromise. Each call to update a uniform will be preceded with an
//'assert' statement to verify that the invoking ShaderProgram matches the
//program the context reports as active. This will thus incur a performance
//hit only during debugging.



//The actual GL function call to update a uniform is hidden behind a macro
//so as to allow for easy backwards compatibility with older 
//OpenGL versions 
// Possible addition to possibly extend support of 'glProgramUniform()' to 
#if !defined GL_VERSION_4_1
#pragma message("\nOpenGL Version Meets or Surpasses 4.1\n"                   \
"Updates to ShaderProgram Uniforms will not require\n"                        \
"their ShaderProgram to be bound to the context\n")                             
#define UPDATE_UNIFORM1i(...) glProgramUniform1i(mProgramID, __VA_ARGS__)
#define UPDATE_UNIFORM1ui(...) glProgramUniform1ui(mProgramID, __VA_ARGS__)
#define UPDATE_UNIFORM1f(...) glProgramUniform1f(mProgramID, __VA_ARGS__)
#define UPDATE_UNIFORM1iv(...) glProgramUniform1iv(mProgramID, __VA_ARGS__)
#define UPDATE_UNIFORM1uiv(...) glProgramUniform1uiv(mProgramID, __VA_ARGS__)
#define UPDATE_UNIFORM1fv(...) glProgramUniform1fv(mProgramID, __VA_ARGS__)

#define UPDATE_UNIFORM2i(...) glProgramUniform2i(mProgramID, __VA_ARGS__)
#define UPDATE_UNIFORM2ui(...) glProgramUniform2ui(mProgramID, __VA_ARGS__)
#define UPDATE_UNIFORM2f(...) glProgramUniform2f(mProgramID, __VA_ARGS__)
#define UPDATE_UNIFORM2iv(...) glProgramUniform2iv(mProgramID, __VA_ARGS__)
#define UPDATE_UNIFORM2uiv(...) glProgramUniform2uiv(mProgramID, __VA_ARGS__)
#define UPDATE_UNIFORM2fv(...) glProgramUniform2fv(mProgramID, __VA_ARGS__)

#define UPDATE_UNIFORM3i(...) glProgramUniform3i(mProgramID, __VA_ARGS__)
#define UPDATE_UNIFORM3ui(...) glProgramUniform3ui(mProgramID, __VA_ARGS__)
#define UPDATE_UNIFORM3f(...) glProgramUniform3f(mProgramID, __VA_ARGS__)
#define UPDATE_UNIFORM3iv(...) glProgramUniform3iv(mProgramID, __VA_ARGS__)
#define UPDATE_UNIFORM3uiv(...) glProgramUniform3uiv(mProgramID, __VA_ARGS__)
#define UPDATE_UNIFORM3fv(...) glProgramUniform3fv(mProgramID, __VA_ARGS__)

#define UPDATE_UNIFORM4i(...) glProgramUniform4i(mProgramID, __VA_ARGS__)
#define UPDATE_UNIFORM4ui(...) glProgramUniform4ui(mProgramID, __VA_ARGS__)
#define UPDATE_UNIFORM4f(...) glProgramUniform4f(mProgramID, __VA_ARGS__)
#define UPDATE_UNIFORM4iv(...) glProgramUniform4iv(mProgramID, __VA_ARGS__)
#define UPDATE_UNIFORM4uiv(...) glProgramUniform4uiv(mProgramID, __VA_ARGS__)
#define UPDATE_UNIFORM4fv(...) glProgramUniform4fv(mProgramID, __VA_ARGS__)

#define UPDATE_UNIFORM_MAT2(...) glProgramUniformMatrix2fv(mProgramID, __VA_ARGS__)
#define UPDATE_UNIFORM_MAT2x3(...) glProgramUniformMatrix2x3fv(mProgramID, __VA_ARGS__)
#define UPDATE_UNIFORM_MAT2x4(...) glProgramUniformMatrix2x4fv(mProgramID, __VA_ARGS__)

#define UPDATE_UNIFORM_MAT3x2(...) glProgramUniformMatrix3x2fv(mProgramID, __VA_ARGS__)
#define UPDATE_UNIFORM_MAT3(...) glProgramUniformMatrix3fv(mProgramID, __VA_ARGS__)
#define UPDATE_UNIFORM_MAT3x4(...) glProgramUniformMatrix3x4fv(mProgramID, __VA_ARGS__)

#define UPDATE_UNIFORM_MAT4x2(...) glProgramUniformMatrix4x2fv(mProgramID, __VA_ARGS__)
#define UPDATE_UNIFORM_MAT4x3(...) glProgramUniformMatrix4x3fv(mProgramID, __VA_ARGS__)
#define UPDATE_UNIFORM_MAT4(...) glProgramUniformMatrix4fv(mProgramID, __VA_ARGS__)

#else 

#pragma message("Failed to Detect Support For OpenGL 4.1 Function 'glProgramUniform()'!\n"  \
"Please ensure that each ShaderProgram object gets bound to the context\n"                  \
"[using 'glUseProgram()'] before attempting to update any of its uniforms!\n"                 )

#define ASSERT_PROGRAM_ACTIVE GLint activeProgram = 0;                                      \
                              glGetIntegerv(GL_CURRENT_PROGRAM, &activeProgram);            \
                              assert(mProgramID == static_cast<GLuint>(activeProgram));

#define UPDATE_UNIFORM1i(...)   ASSERT_PROGRAM_ACTIVE; glUniform1i(__VA_ARGS__)
#define UPDATE_UNIFORM1ui(...)  ASSERT_PROGRAM_ACTIVE; glUniform1ui(__VA_ARGS__)
#define UPDATE_UNIFORM1f(...)   ASSERT_PROGRAM_ACTIVE; glUniform1f(__VA_ARGS__)
#define UPDATE_UNIFORM1iv(...)  ASSERT_PROGRAM_ACTIVE; glUniform1iv(__VA_ARGS__)
#define UPDATE_UNIFORM1uiv(...) ASSERT_PROGRAM_ACTIVE; glUniform1uiv(__VA_ARGS__)
#define UPDATE_UNIFORM1fv(...)  ASSERT_PROGRAM_ACTIVE; glUniform1fv(__VA_ARGS__)

#define UPDATE_UNIFORM2i(...)   ASSERT_PROGRAM_ACTIVE; glUniform2i(__VA_ARGS__)
#define UPDATE_UNIFORM2ui(...)  ASSERT_PROGRAM_ACTIVE; glUniform2ui(__VA_ARGS__)
#define UPDATE_UNIFORM2f(...)   ASSERT_PROGRAM_ACTIVE; glUniform2f(__VA_ARGS__)
#define UPDATE_UNIFORM2iv(...)  ASSERT_PROGRAM_ACTIVE; glUniform2iv(__VA_ARGS__)
#define UPDATE_UNIFORM2uiv(...) ASSERT_PROGRAM_ACTIVE; glUniform2uiv(__VA_ARGS__)
#define UPDATE_UNIFORM2fv(...)  ASSERT_PROGRAM_ACTIVE; glUniform2fv(__VA_ARGS__)

#define UPDATE_UNIFORM3i(...)   ASSERT_PROGRAM_ACTIVE; glUniform3i(__VA_ARGS__)
#define UPDATE_UNIFORM3ui(...)  ASSERT_PROGRAM_ACTIVE; glUniform3ui(__VA_ARGS__)
#define UPDATE_UNIFORM3f(...)   ASSERT_PROGRAM_ACTIVE; glUniform3f(__VA_ARGS__)
#define UPDATE_UNIFORM3iv(...)  ASSERT_PROGRAM_ACTIVE; glUniform3iv(__VA_ARGS__)
#define UPDATE_UNIFORM3uiv(...) ASSERT_PROGRAM_ACTIVE; glUniform3uiv(__VA_ARGS__)
#define UPDATE_UNIFORM3fv(...)  ASSERT_PROGRAM_ACTIVE; glUniform3fv(__VA_ARGS__)

#define UPDATE_UNIFORM4i(...)   ASSERT_PROGRAM_ACTIVE; glUniform4i(__VA_ARGS__)
#define UPDATE_UNIFORM4ui(...)  ASSERT_PROGRAM_ACTIVE; glUniform4ui(__VA_ARGS__)
#define UPDATE_UNIFORM4f(...)   ASSERT_PROGRAM_ACTIVE; glUniform4f(__VA_ARGS__)
#define UPDATE_UNIFORM4iv(...)  ASSERT_PROGRAM_ACTIVE; glUniform4iv(__VA_ARGS__)
#define UPDATE_UNIFORM4uiv(...) ASSERT_PROGRAM_ACTIVE; glUniform4uiv(__VA_ARGS__)
#define UPDATE_UNIFORM4fv(...)  ASSERT_PROGRAM_ACTIVE; glUniform4fv(__VA_ARGS__)

#define UPDATE_UNIFORM_MAT2(...)   ASSERT_PROGRAM_ACTIVE; glUniformMatrix2fv(__VA_ARGS__)
#define UPDATE_UNIFORM_MAT2x3(...) ASSERT_PROGRAM_ACTIVE; glUniformMatrix2x3fv(__VA_ARGS__)
#define UPDATE_UNIFORM_MAT2x4(...) ASSERT_PROGRAM_ACTIVE; glUniformMatrix2x4fv(__VA_ARGS__)

#define UPDATE_UNIFORM_MAT3x2(...) ASSERT_PROGRAM_ACTIVE; glUniformMatrix3x2fv(__VA_ARGS__)
#define UPDATE_UNIFORM_MAT3(...)   ASSERT_PROGRAM_ACTIVE; glUniformMatrix3fv(__VA_ARGS__)
#define UPDATE_UNIFORM_MAT3x4(...) ASSERT_PROGRAM_ACTIVE; glUniformMatrix3x4fv(__VA_ARGS__)

#define UPDATE_UNIFORM_MAT4x2(...) ASSERT_PROGRAM_ACTIVE; glUniformMatrix4x2fv(__VA_ARGS__)
#define UPDATE_UNIFORM_MAT4x3(...) ASSERT_PROGRAM_ACTIVE; glUniformMatrix4x3fv(__VA_ARGS__)
#define UPDATE_UNIFORM_MAT4(...)   ASSERT_PROGRAM_ACTIVE; glUniformMatrix4fv(__VA_ARGS__)
#endif 


namespace ShaderInterface {

	static const uint_fast8_t NOT_IN_VECTOR = 255u;
	static const GLint NOT_IN_LIST = -2;
	static constexpr size_t CACHED_UNIFORM_LOCATION_HASH_SIZE = 107u; //Should be prime number?


	UniformLocationTracker::UniformLocationTracker(const GLuint& programID) {
		mProgramID = programID;
		mTrackedLocationsCount = 0ULL;

		//mCachedUniformLocations.rehash(CACHED_UNIFORM_LOCATION_HASH_SIZE);

		initializeListLocations();
	}

	UniformLocationTracker::~UniformLocationTracker() noexcept {

	}


	//Update uniform  //see: https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glUniform.xhtml
	void UniformLocationTracker::updateUniform1i(const GLchar * uniformName, const GLint x) {
        UPDATE_UNIFORM1i(lookupUniformLocation(i1_, uniformName, UniformType::INT1), x);
        //glUniform1i(lookupUniformLocation(i1_, uniformName, UniformType::INT1), x);
	}
	void UniformLocationTracker::updateUniform1u(const GLchar * uniformName, const GLuint x) {
        UPDATE_UNIFORM1ui(lookupUniformLocation(u1_, uniformName, UniformType::UINT1), x);
	}
	void UniformLocationTracker::updateUniform1f(const GLchar * uniformName, const GLfloat x) {
        UPDATE_UNIFORM1f(lookupUniformLocation(f1_, uniformName, UniformType::FLOAT1), x);
	}
	void UniformLocationTracker::updateUniform1iv(const GLchar * uniformName, const GLint * valPtr, GLsizei count) {
        UPDATE_UNIFORM1iv(lookupUniformLocation(iv1_, uniformName, UniformType::INT1V), count, valPtr);
	}
	void UniformLocationTracker::updateUniform1uv(const GLchar * uniformName, const GLuint * valPtr, GLsizei count) {
        UPDATE_UNIFORM1uiv(lookupUniformLocation(uv1_, uniformName, UniformType::UINT1V), count, valPtr);
	}
	void UniformLocationTracker::updateUniform1fv(const GLchar * uniformName, const GLfloat * valPtr, GLsizei count) {
        UPDATE_UNIFORM1fv(lookupUniformLocation(fv1_, uniformName, UniformType::FLOAT1V), count, valPtr);
	}


	void UniformLocationTracker::updateUniform2i(const GLchar * uniformName, const GLint x, const GLint y) {
        UPDATE_UNIFORM2i(lookupUniformLocation(i2_, uniformName, UniformType::INT2), x, y);
	}
	void UniformLocationTracker::updateUniform2u(const GLchar * uniformName, const GLuint x, const GLuint y) {
        UPDATE_UNIFORM2ui(lookupUniformLocation(u2_, uniformName, UniformType::UINT2), x, y);
	}
	void UniformLocationTracker::updateUniform2f(const GLchar * uniformName, const GLfloat x, const GLfloat y) {
        UPDATE_UNIFORM2f(lookupUniformLocation(f2_, uniformName, UniformType::FLOAT2), x, y);
	}
	void UniformLocationTracker::updateUniform2iv(const GLchar * uniformName, const GLint * valPtr, GLsizei count) {
        UPDATE_UNIFORM2iv(lookupUniformLocation(iv2_, uniformName, UniformType::INT2V), count, valPtr);
	}
	void UniformLocationTracker::updateUniform2uv(const GLchar * uniformName, const GLuint * valPtr, GLsizei count) {
        UPDATE_UNIFORM2uiv(lookupUniformLocation(uv2_, uniformName, UniformType::UINT2V), count, valPtr);
	}
	void UniformLocationTracker::updateUniform2fv(const GLchar * uniformName, const GLfloat * valPtr, GLsizei count) {
        UPDATE_UNIFORM2fv(lookupUniformLocation(fv2_, uniformName, UniformType::FLOAT2V), count, valPtr);
	}


	void UniformLocationTracker::updateUniform3i(const GLchar * uniformName, const GLint x, const GLint y, const GLint z) {
        UPDATE_UNIFORM3i(lookupUniformLocation(i3_, uniformName, UniformType::INT3), x, y, z);
	}
	void UniformLocationTracker::updateUniform3u(const GLchar * uniformName, const GLuint x, const GLuint y, const GLuint z) {
        UPDATE_UNIFORM3ui(lookupUniformLocation(u3_, uniformName, UniformType::UINT3), x, y, z);
	}
	void UniformLocationTracker::updateUniform3f(const GLchar * uniformName, const GLfloat x, const GLfloat y, const GLfloat z) {
        UPDATE_UNIFORM3f(lookupUniformLocation(f3_, uniformName, UniformType::FLOAT3), x, y, z);
	}
	void UniformLocationTracker::updateUniform3iv(const GLchar * uniformName, const GLint * valPtr, GLsizei count) {
        UPDATE_UNIFORM3iv(lookupUniformLocation(iv3_, uniformName, UniformType::INT3V), count, valPtr);
	}
	void UniformLocationTracker::updateUniform3uv(const GLchar * uniformName, const GLuint * valPtr, GLsizei count) {
        UPDATE_UNIFORM3uiv(lookupUniformLocation(uv3_, uniformName, UniformType::UINT3V), count, valPtr);
	}
	void UniformLocationTracker::updateUniform3fv(const GLchar * uniformName, const GLfloat * valPtr, GLsizei count) {
        UPDATE_UNIFORM3fv(lookupUniformLocation(fv3_, uniformName, UniformType::FLOAT3V), count, valPtr);
	}


	void UniformLocationTracker::updateUniform4i(const GLchar * uniformName, const GLint x, const GLint y, const GLint z, const GLint w) {
        UPDATE_UNIFORM4i(lookupUniformLocation(i4_, uniformName, UniformType::INT4), x, y, z, w);
	}
	void UniformLocationTracker::updateUniform4u(const GLchar * uniformName, const GLuint x, const GLuint y, const GLuint z, const GLuint w) {
        UPDATE_UNIFORM4ui(lookupUniformLocation(u4_, uniformName, UniformType::UINT4), x, y, z, w);
	}
	void UniformLocationTracker::updateUniform4f(const GLchar * uniformName, const GLfloat x, const GLfloat y, const GLfloat z, const GLfloat w) {
        UPDATE_UNIFORM4f(lookupUniformLocation(f4_, uniformName, UniformType::FLOAT4), x, y, z, w);
	}
	void UniformLocationTracker::updateUniform4iv(const GLchar * uniformName, const GLint * valPtr, GLsizei count) {
        UPDATE_UNIFORM4iv(lookupUniformLocation(iv4_, uniformName, UniformType::INT4V), count, valPtr);
	}
	void UniformLocationTracker::updateUniform4uv(const GLchar * uniformName, const GLuint * valPtr, GLsizei count) {
        UPDATE_UNIFORM4uiv(lookupUniformLocation(uv4_, uniformName, UniformType::UINT4V), count, valPtr);
	}
	void UniformLocationTracker::updateUniform4fv(const GLchar * uniformName, const GLfloat * valPtr, GLsizei count) {
        UPDATE_UNIFORM4fv(lookupUniformLocation(fv4_, uniformName, UniformType::FLOAT4), count, valPtr);
	}


	//Matrix update functions

	void UniformLocationTracker::updateUniformMat2(const GLchar * uniformName, const glm::mat2 * matrix, GLsizei count) {
		if (count == 0u) { return; }
		else if (count == 1u) {
			UPDATE_UNIFORM_MAT2(lookupUniformLocation(M2x2_, uniformName, UniformType::MAT2), count, GL_FALSE, static_cast<const GLfloat *>(glm::value_ptr(*matrix)));
		}
		else { //The way this is implemented currently involves an intermediate memory allocation for the 
			//   vector to hold the multiple matrices worth of data. As such, this function is
			//   inefficient for updating an array of matrices.
			std::vector<GLfloat> matrixData;
			matrixData.reserve(4u * count);
			for (GLsizei i = 0u; i < count; i++) { //For each matrix 
				const GLfloat * matData = static_cast<const GLfloat *>(glm::value_ptr(*matrix)); //Get matrix data
				for (GLsizei i = 0u; i < 4u; i++) { //Add the 4 entries to the vector
					matrixData.push_back(matData[i]);
				}
			}
            UPDATE_UNIFORM_MAT2(lookupUniformLocation(M2x2_, uniformName, UniformType::MAT2), count, GL_FALSE, matrixData.data());
		}
	}

	void UniformLocationTracker::updateUniformMat2(const GLchar * uniformName, const GLfloat * valPtr, GLsizei count) {
        UPDATE_UNIFORM_MAT2(lookupUniformLocation(M2x2_, uniformName, UniformType::MAT2), count, GL_FALSE, valPtr);
	}

	void UniformLocationTracker::updateUniformMat2x2(const GLchar * uniformName, const glm::mat2 * matrix, GLsizei count) {
		if (count == 0u) { return; }
		else if (count == 1u) {
            UPDATE_UNIFORM_MAT2(lookupUniformLocation(M2x2_, uniformName, UniformType::MAT2), count, GL_FALSE, static_cast<const GLfloat *>(glm::value_ptr(*matrix)));
		}
		else { //The way this is implemented currently involves an intermediate memory allocation for the 
			   //   vector to hold the multiple matrices worth of data. As such, this function is
			   //   inefficient for updating an array of matrices.
			std::vector<GLfloat> matrixData;
			matrixData.reserve(4u * count);
			for (GLsizei i = 0u; i < count; i++) { //For each matrix 
				const GLfloat * matData = static_cast<const GLfloat *>(glm::value_ptr(*matrix)); //Get matrix data
				for (GLsizei i = 0u; i < 4u; i++) { //Add the 4 entries to the vector
					matrixData.push_back(matData[i]);
				}
			}
            UPDATE_UNIFORM_MAT2(lookupUniformLocation(M2x2_, uniformName, UniformType::MAT2), count, GL_FALSE, matrixData.data());
		}
	}

	void UniformLocationTracker::updateUniformMat2x2(const GLchar * uniformName, const GLfloat * valPtr, GLsizei count) {
        UPDATE_UNIFORM_MAT2(lookupUniformLocation(M2x2_, uniformName, UniformType::MAT2), count, GL_FALSE, valPtr);
	}

	void UniformLocationTracker::updateUniformMat2x3(const GLchar * uniformName, const glm::mat2x3 * matrix, GLsizei count) {
		if (count == 0u) { return; }
		else if (count == 1u) {
            UPDATE_UNIFORM_MAT2x3(lookupUniformLocation(M2x3_, uniformName, UniformType::MAT2X3), count, GL_FALSE, static_cast<const GLfloat *>(glm::value_ptr(*matrix)));
		}
		else { //The way this is implemented currently involves an intermediate memory allocation for the 
			   //   vector to hold the multiple matrices worth of data. As such, this function is
			   //   inefficient for updating an array of matrices.
			std::vector<GLfloat> matrixData;
			matrixData.reserve(6u * count);
			for (GLsizei i = 0u; i < count; i++) { //For each matrix 
				const GLfloat * matData = static_cast<const GLfloat *>(glm::value_ptr(*matrix)); //Get matrix data
				for (GLsizei i = 0u; i < 6u; i++) { //Add the 6 entries to the vector
					matrixData.push_back(matData[i]);
				}
			}
            UPDATE_UNIFORM_MAT2x3(lookupUniformLocation(M2x3_, uniformName, UniformType::MAT2X3), count, GL_FALSE, matrixData.data());
		}
	}

	void UniformLocationTracker::updateUniformMat2x3(const GLchar * uniformName, const GLfloat * valPtr, GLsizei count) {
        UPDATE_UNIFORM_MAT2x3(lookupUniformLocation(M2x3_, uniformName, UniformType::MAT2X3), count, GL_FALSE, valPtr);
	}

	void UniformLocationTracker::updateUniformMat2x4(const GLchar * uniformName, const glm::mat2x4 * matrix, GLsizei count) {
		if (count == 0u) { return; }
		else if (count == 1u) {
            UPDATE_UNIFORM_MAT2x4(lookupUniformLocation(M2x4_, uniformName, UniformType::MAT2X4), count, GL_FALSE, static_cast<const GLfloat *>(glm::value_ptr(*matrix)));
		}
		else { //The way this is implemented currently involves an intermediate memory allocation for the 
			   //   vector to hold the multiple matrices worth of data. As such, this function is
			   //   inefficient for updating an array of matrices.
			std::vector<GLfloat> matrixData;
			matrixData.reserve(8u * count);
			for (GLsizei i = 0u; i < count; i++) { //For each matrix 
				const GLfloat * matData = static_cast<const GLfloat *>(glm::value_ptr(*matrix)); //Get matrix data
				for (GLsizei i = 0u; i < 8u; i++) { //Add the 8 entries to the vector
					matrixData.push_back(matData[i]);
				}
			}
            UPDATE_UNIFORM_MAT2x4(lookupUniformLocation(M2x4_, uniformName, UniformType::MAT2X4), count, GL_FALSE, matrixData.data());
		}
	}

	void UniformLocationTracker::updateUniformMat2x4(const GLchar * uniformName, const GLfloat * valPtr, GLsizei count) {
        UPDATE_UNIFORM_MAT2x4(lookupUniformLocation(M2x4_, uniformName, UniformType::MAT2X4), count, GL_FALSE, valPtr);
	}


	void UniformLocationTracker::updateUniformMat3x2(const GLchar * uniformName, const glm::mat3x2 * matrix, GLsizei count) {
		if (count == 0u) { return; }
		else if (count == 1u) {
            UPDATE_UNIFORM_MAT3x2(lookupUniformLocation(M3x2_, uniformName, UniformType::MAT3X2), count, GL_FALSE, static_cast<const GLfloat *>(glm::value_ptr(*matrix)));
		}
		else { //The way this is implemented currently involves an intermediate memory allocation for the 
			   //   vector to hold the multiple matrices worth of data. As such, this function is
			   //   inefficient for updating an array of matrices.
			std::vector<GLfloat> matrixData;
			matrixData.reserve(6u * count);
			for (GLsizei i = 0u; i < count; i++) { //For each matrix 
				const GLfloat * matData = static_cast<const GLfloat *>(glm::value_ptr(*matrix)); //Get matrix data
				for (GLsizei i = 0u; i < 6u; i++) { //Add the 6 entries to the vector
					matrixData.push_back(matData[i]);
				}
			}
            UPDATE_UNIFORM_MAT3x2(lookupUniformLocation(M3x2_, uniformName, UniformType::MAT3X2), count, GL_FALSE, matrixData.data());
		}
	}

	void UniformLocationTracker::updateUniformMat3x2(const GLchar * uniformName, const GLfloat * valPtr, GLsizei count) {
        UPDATE_UNIFORM_MAT3x2(lookupUniformLocation(M3x2_, uniformName, UniformType::MAT3X2), count, GL_FALSE, valPtr);
	}

	void UniformLocationTracker::updateUniformMat3(const GLchar * uniformName, const glm::mat3 * matrix, GLsizei count) {
		if (count == 0u) { return; }
		else if (count == 1u) {
            UPDATE_UNIFORM_MAT3(lookupUniformLocation(M3x3_, uniformName, UniformType::MAT3), count, GL_FALSE, static_cast<const GLfloat *>(glm::value_ptr(*matrix)));
		}
		else { //The way this is implemented currently involves an intermediate memory allocation for the 
			   //   vector to hold the multiple matrices worth of data. As such, this function is
			   //   inefficient for updating an array of matrices.
			std::vector<GLfloat> matrixData;
			matrixData.reserve(9u * count);
			for (GLsizei i = 0u; i < count; i++) { //For each matrix 
				const GLfloat * matData = static_cast<const GLfloat *>(glm::value_ptr(*matrix)); //Get matrix data
				for (GLsizei i = 0u; i < 9u; i++) { //Add the 9 entries to the vector
					matrixData.push_back(matData[i]);
				}
			}
            UPDATE_UNIFORM_MAT3(lookupUniformLocation(M3x3_, uniformName, UniformType::MAT3), count, GL_FALSE, matrixData.data());
		}
	}

	void UniformLocationTracker::updateUniformMat3(const GLchar * uniformName, const GLfloat * valPtr, GLsizei count) {
        UPDATE_UNIFORM_MAT3(lookupUniformLocation(M3x3_, uniformName, UniformType::MAT3), count, GL_FALSE, valPtr);
	}

	void UniformLocationTracker::updateUniformMat3x3(const GLchar * uniformName, const glm::mat3 * matrix, GLsizei count) {
		if (count == 0u) { return; }
		else if (count == 1u) {
            UPDATE_UNIFORM_MAT3(lookupUniformLocation(M3x3_, uniformName, UniformType::MAT3), count, GL_FALSE, static_cast<const GLfloat *>(glm::value_ptr(*matrix)));
		}
		else { //The way this is implemented currently involves an intermediate memory allocation for the 
			   //   vector to hold the multiple matrices worth of data. As such, this function is
			   //   inefficient for updating an array of matrices.
			std::vector<GLfloat> matrixData;
			matrixData.reserve(9u * count);
			for (GLsizei i = 0u; i < count; i++) { //For each matrix 
				const GLfloat * matData = static_cast<const GLfloat *>(glm::value_ptr(*matrix)); //Get matrix data
				for (GLsizei i = 0u; i < 9u; i++) { //Add the 9 entries to the vector
					matrixData.push_back(matData[i]);
				}
			}
            UPDATE_UNIFORM_MAT3(lookupUniformLocation(M3x3_, uniformName, UniformType::MAT3), count, GL_FALSE, matrixData.data());
		}
	}

	void UniformLocationTracker::updateUniformMat3x3(const GLchar * uniformName, const GLfloat * valPtr, GLsizei count) {
        UPDATE_UNIFORM_MAT3(lookupUniformLocation(M3x3_, uniformName, UniformType::MAT3), count, GL_FALSE, valPtr);
	}

	void UniformLocationTracker::updateUniformMat3x4(const GLchar * uniformName, const glm::mat3x4 * matrix, GLsizei count) {
		if (count == 0u) { return; }
		else if (count == 1u) {
            UPDATE_UNIFORM_MAT3x4(lookupUniformLocation(M3x4_, uniformName, UniformType::MAT3X4), count, GL_FALSE, static_cast<const GLfloat *>(glm::value_ptr(*matrix)));
		}
		else { //The way this is implemented currently involves an intermediate memory allocation for the 
			   //   vector to hold the multiple matrices worth of data. As such, this function is
			   //   inefficient for updating an array of matrices.
			std::vector<GLfloat> matrixData;
			matrixData.reserve(12u * count);
			for (GLsizei i = 0u; i < count; i++) { //For each matrix 
				const GLfloat * matData = static_cast<const GLfloat *>(glm::value_ptr(*matrix)); //Get matrix data
				for (GLsizei i = 0u; i < 12u; i++) { //Add the 12 entries to the vector
					matrixData.push_back(matData[i]);
				}
			}
            UPDATE_UNIFORM_MAT3x4(lookupUniformLocation(M3x4_, uniformName, UniformType::MAT3X4), count, GL_FALSE, matrixData.data());
		}
	}

	void UniformLocationTracker::updateUniformMat3x4(const GLchar * uniformName, const GLfloat * valPtr, GLsizei count) {
        UPDATE_UNIFORM_MAT3x4(lookupUniformLocation(M3x4_, uniformName, UniformType::MAT3X4), count, GL_FALSE, valPtr);
	}



	void UniformLocationTracker::updateUniformMat4x2(const GLchar * uniformName, const glm::mat4x2 * matrix, GLsizei count) {
		if (count == 0u) { return; }
		else if (count == 1u) {
            UPDATE_UNIFORM_MAT4x2(lookupUniformLocation(M4x2_, uniformName, UniformType::MAT4X2), count, GL_FALSE, static_cast<const GLfloat *>(glm::value_ptr(*matrix)));
		}
		else { //The way this is implemented currently involves an intermediate memory allocation for the 
			   //   vector to hold the multiple matrices worth of data. As such, this function is
			   //   inefficient for updating an array of matrices.
			std::vector<GLfloat> matrixData;
			matrixData.reserve(8u * count);
			for (GLsizei i = 0u; i < count; i++) { //For each matrix 
				const GLfloat * matData = static_cast<const GLfloat *>(glm::value_ptr(*matrix)); //Get matrix data
				for (GLsizei i = 0u; i < 8u; i++) { //Add the 8 entries to the vector
					matrixData.push_back(matData[i]);
				}
			}
            UPDATE_UNIFORM_MAT4x2(lookupUniformLocation(M4x2_, uniformName, UniformType::MAT4X2), count, GL_FALSE, matrixData.data());
		}
	}

	void UniformLocationTracker::updateUniformMat4x2(const GLchar * uniformName, const GLfloat * valPtr, GLsizei count) {
        UPDATE_UNIFORM_MAT4x2(lookupUniformLocation(M4x2_, uniformName, UniformType::MAT4X2), count, GL_FALSE, valPtr);
	}

	void UniformLocationTracker::updateUniformMat4x3(const GLchar * uniformName, const glm::mat4x3 * matrix, GLsizei count) {
		if (count == 0u) { return; }
		else if (count == 1u) {
            UPDATE_UNIFORM_MAT4x3(lookupUniformLocation(M4x3_, uniformName, UniformType::MAT4X3), count, GL_FALSE, static_cast<const GLfloat *>(glm::value_ptr(*matrix)));
		}
		else { //The way this is implemented currently involves an intermediate memory allocation for the 
			   //   vector to hold the multiple matrices worth of data. As such, this function is
			   //   inefficient for updating an array of matrices.
			std::vector<GLfloat> matrixData;
			matrixData.reserve(12u * count);
			for (GLsizei i = 0u; i < count; i++) { //For each matrix 
				const GLfloat * matData = static_cast<const GLfloat *>(glm::value_ptr(*matrix)); //Get matrix data
				for (GLsizei i = 0u; i < 12u; i++) { //Add the 12 entries to the vector
					matrixData.push_back(matData[i]);
				}
			}
            UPDATE_UNIFORM_MAT4x3(lookupUniformLocation(M4x3_, uniformName, UniformType::MAT4X3), count, GL_FALSE, matrixData.data());
		}
	}

	void UniformLocationTracker::updateUniformMat4x3(const GLchar * uniformName, const GLfloat * valPtr, GLsizei count) {
        UPDATE_UNIFORM_MAT4x3(lookupUniformLocation(M4x3_, uniformName, UniformType::MAT4X3), count, GL_FALSE, valPtr);
	}

	void UniformLocationTracker::updateUniformMat4(const GLchar * uniformName, const glm::mat4 * matrix, GLsizei count) {
		if (count == 0u) { return; }
		else if (count == 1u) {
            UPDATE_UNIFORM_MAT4(lookupUniformLocation(M4x4_, uniformName, UniformType::MAT4), count, GL_FALSE, static_cast<const GLfloat *>(glm::value_ptr(*matrix)));
		}
		else { //The way this is implemented currently involves an intermediate memory allocation for the 
			   //   vector to hold the multiple matrices worth of data. As such, this function is
			   //   inefficient for updating an array of matrices.
			std::vector<GLfloat> matrixData;
			matrixData.reserve(16u * count);
			for (GLsizei i = 0u; i < count; i++) { //For each matrix 
				const GLfloat * matData = static_cast<const GLfloat *>(glm::value_ptr(*matrix)); //Get matrix data
				for (GLsizei i = 0u; i < 16u; i++) { //Add the 16 entries to the vector
					matrixData.push_back(matData[i]);
				}
			}
            UPDATE_UNIFORM_MAT4(lookupUniformLocation(M4x4_, uniformName, UniformType::MAT4), count, GL_FALSE, matrixData.data());
		}
	}

	void UniformLocationTracker::updateUniformMat4(const GLchar * uniformName, const GLfloat * valPtr, GLsizei count) {
        UPDATE_UNIFORM_MAT4(lookupUniformLocation(M4x4_, uniformName, UniformType::MAT4), count, GL_FALSE, valPtr);
	}

	void UniformLocationTracker::updateUniformMat4x4(const GLchar * uniformName, const glm::mat4 * matrix, GLsizei count) {
		if (count == 0u) { return; }
		else if (count == 1u) {
            UPDATE_UNIFORM_MAT4(lookupUniformLocation(M4x4_, uniformName, UniformType::MAT4), count, GL_FALSE, static_cast<const GLfloat *>(glm::value_ptr(*matrix)));
		}
		else { //The way this is implemented currently involves an intermediate memory allocation for the 
			   //   vector to hold the multiple matrices worth of data. As such, this function is
			   //   inefficient for updating an array of matrices.
			std::vector<GLfloat> matrixData;
			matrixData.reserve(16u * count);
			for (GLsizei i = 0u; i < count; i++) { //For each matrix 
				const GLfloat * matData = static_cast<const GLfloat *>(glm::value_ptr(*matrix)); //Get matrix data
				for (GLsizei i = 0u; i < 16u; i++) { //Add the 16 entries to the vector
					matrixData.push_back(matData[i]);
				}
			}
            UPDATE_UNIFORM_MAT4(lookupUniformLocation(M4x4_, uniformName, UniformType::MAT4), count, GL_FALSE, matrixData.data());
		}
	}

	void UniformLocationTracker::updateUniformMat4x4(const GLchar * uniformName, const GLfloat * valPtr, GLsizei count) {
        UPDATE_UNIFORM_MAT4(lookupUniformLocation(M4x4_, uniformName, UniformType::MAT4), count, GL_FALSE, valPtr);
	}



	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////               Private functions               ///////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Helper function for constructor
	void UniformLocationTracker::initializeListLocations() { //Only call from constructor
		i1_ = u1_ = f1_ = iv1_ = uv1_ = fv1_ = NOT_IN_VECTOR;
		i2_ = u2_ = f2_ = iv2_ = uv2_ = fv2_ = NOT_IN_VECTOR;
		i3_ = u3_ = f3_ = iv3_ = uv3_ = fv3_ = NOT_IN_VECTOR;
		i4_ = u4_ = f4_ = iv4_ = uv4_ = fv4_ = NOT_IN_VECTOR;
        M2x2_ = M2x3_ = M2x4_ = NOT_IN_VECTOR;
        M3x2_ = M3x3_ = M3x4_ = NOT_IN_VECTOR;
        M4x2_ = M4x3_ = M4x4_ = NOT_IN_VECTOR;

		mNextListIndex = 0u; //Initialize to the index that will be assigned to the first element emplaced into the vector
	}


	//Note-to-self: I can probably remove the 'UniformType' enum completely since ULBuckets will be constructed in the correct place automatically. 
	//Updated Note-To-self: Don't remove UniformType because I now use it for CachedUniformLocations as well...     I can probably delete both of these messages
	inline GLint UniformLocationTracker::lookupUniformLocation(uint_fast8_t& listIndex, const char * name, UniformType ut) {
		if (listIndex == NOT_IN_VECTOR) { //If no uniforms of this type have been tracked yet
			mUniformLocationLists.emplace_back(); //Emplace a new list
			listIndex = mNextListIndex++;
			mUniformLocationLists[listIndex].emplace_front(mProgramID, name, ut);
			return mUniformLocationLists[listIndex].front().getLocation();
		}
		else { //Else if uniforms of this type already are being tracked
			GLint uniformLocation = findInList(listIndex, name);
			if (uniformLocation != NOT_IN_LIST) { //Try to find the uniform in the list
				return uniformLocation;
			}
			else {
				mUniformLocationLists[listIndex].emplace_back(mProgramID, name, ut); //Put a new bucket for the uniform at the end of the list
				return mUniformLocationLists[listIndex].back().getLocation();
			}
		}
	}

	GLint UniformLocationTracker::findInList(uint_fast8_t listIndex, const char * name) { //Returns a uniform location or -2 if not found in list  (-1 already has meaning within OpenGL)
		//Make sure that listIndex is valid (can remove this check once I am sure code is operating correctly
		//if (static_cast<size_t>(listIndex) > (mUniformLocationLists.size() - 1ull)) {
		//	fprintf(WRNLOG, "\nWARNING! In UniformLocationTracker within the function call 'findInList',\n");
		//	fprintf(WRNLOG, "         a list index of %u was requested but only %zu lists are in the vector!\n", listIndex, mUniformLocationLists.size());
		//	fprintf(WRNLOG, "         Clearly this should not happen...Check Ya Code!\n");
		//	throw std::out_of_range("The list to search in the function call findInList within a UniformLocationTracker object has index outside the range of tracked lists!\n");
		//}

		if (listIndex == NOT_IN_VECTOR) {
			throw std::out_of_range("OOps! It looks like the list index was never set when the function findInList in the class UniformLocationTracker was called!\n");
		}

		std::list<UniformLocationBucket>::iterator iter = mUniformLocationLists[listIndex].begin();
		for (; iter != mUniformLocationLists[listIndex].end(); iter++) {
			if ((*iter).checkForMatch(name)) {
				return (*iter).getLocation();
			}
		}

		return NOT_IN_LIST;
	}

} // namespace ShaderInterface