//This class is intented to help a ShaderProgram object manage it's tracked uniform locations. 

#pragma once

#ifndef UNIFORM_LOCATION_TRACKER
#define UNIFORM_LOCATION_TRACKER

#include <list>
#include <vector>

#include "ProjectConstants.h"
#include "ProjectParameters.h"
#include "UniformLocationBucket.h"



class UniformLocationTracker {
public:
	UniformLocationTracker() = delete;
	UniformLocationTracker(const GLuint& programID);
	~UniformLocationTracker();

	//Copying
	UniformLocationTracker(const UniformLocationTracker&) = default; //Nothing fancy required
	UniformLocationTracker(UniformLocationTracker&&) = default;
	UniformLocationTracker& operator=(const UniformLocationTracker&) = default;
	UniformLocationTracker& operator=(UniformLocationTracker&&) = default;



	//Update uniform
	//see: https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glUniform.xhtml
	void updateUniform1i(const char *, const GLint);
	void updateUniform1u(const char *, const GLuint);
	void updateUniform1f(const char *, const GLfloat);
	void updateUniform1iv(const char *, const GLint *, GLsizei count = 1); //A vector is a pointer to an array of the element instead of seperate arguments
	void updateUniform1uv(const char *, const GLuint *, GLsizei count = 1);
	void updateUniform1fv(const char *, const GLfloat *, GLsizei count = 1);

	void updateUniform2i(const char *, const GLint, const GLint);
	void updateUniform2u(const char *, const GLuint, const GLuint);
	void updateUniform2f(const char *, const GLfloat, const GLfloat);
	void updateUniform2iv(const char *, const GLint *, GLsizei count = 1); 
	void updateUniform2uv(const char *, const GLuint *, GLsizei count = 1);
	void updateUniform2fv(const char *, const GLfloat *, GLsizei count = 1);

	void updateUniform3i(const char *, const GLint, const GLint, const GLint);
	void updateUniform3u(const char *, const GLuint, const GLuint, const GLuint);
	void updateUniform3f(const char *, const GLfloat, const GLfloat, const GLfloat);
	void updateUniform3iv(const char *, const GLint *, GLsizei count = 1);
	void updateUniform3uv(const char *, const GLuint *, GLsizei count = 1);
	void updateUniform3fv(const char *, const GLfloat *, GLsizei count = 1);
	
	void updateUniform4i(const char *, const GLint, const GLint, const GLint, const GLint);
	void updateUniform4u(const char *, const GLuint, const GLuint, const GLuint, const GLuint);
	void updateUniform4f(const char *, const GLfloat, const GLfloat, const GLfloat, const GLfloat);
	void updateUniform4iv(const char *, const GLint *, GLsizei count = 1);
	void updateUniform4uv(const char *, const GLuint *, GLsizei count = 1);
	void updateUniform4fv(const char *, const GLfloat *, GLsizei count = 1);
	
	void updateUniformMat2x2(const char *, const GLfloat *, GLsizei count = 1, GLboolean transpose = false);
	void updateUniformMat2x3(const char *, const GLfloat *, GLsizei count = 1, GLboolean transpose = false);
	void updateUniformMat2x4(const char *, const GLfloat *, GLsizei count = 1, GLboolean transpose = false);

	void updateUniformMat3x2(const char *, const GLfloat *, GLsizei count = 1, GLboolean transpose = false);
	void updateUniformMat3x3(const char *, const GLfloat *, GLsizei count = 1, GLboolean transpose = false);
	void updateUniformMat3x4(const char *, const GLfloat *, GLsizei count = 1, GLboolean transpose = false);

	void updateUniformMat4x2(const char *, const GLfloat *, GLsizei count = 1, GLboolean transpose = false);
	void updateUniformMat4x3(const char *, const GLfloat *, GLsizei count = 1, GLboolean transpose = false);
	void updateUniformMat4x4(const char *, const GLfloat *, GLsizei count = 1, GLboolean transpose = false);


	size_t getNumberOfTrackedUniforms() const { return mTrackedLocationsCount; }
	GLuint getProgramID() const { return mProgramID; }

private: //See .cpp file for implementation details
	GLuint mProgramID;
	int mTrackedLocationsCount;

	std::vector<std::list<UniformLocationBucket>> mUniformLocationLists; //A vector of lists of uniform locations
	
	
	//List index for each type 
	uint8_t i1_, u1_, f1_, iv1_, uv1_, fv1_;
	uint8_t	i2_, u2_, f2_, iv2_, uv2_, fv2_;
	uint8_t	i3_, u3_, f3_, iv3_, uv3_, fv3_;
	uint8_t	i4_, u4_, f4_, iv4_, uv4_, fv4_;
	uint8_t M2x2_, M2x3_, M2x4_, M3x2_, M3x3_, M3x4_, M4x2_, M4x3_, M4x4_;
	
	uint8_t mNextListIndex;


	
	GLint lookupUniformLocation(uint8_t& listIndex, const char *, UniformType); //listIndex may be updated during function call
	GLint findInList(uint8_t, const char *); //Returns a uniform location or -2 if not found in list  (-1 already has meaning within OpenGL)


	void initializeListLocations(); //Only call from constructor




	//bool checkForOpenGLError(); //see: https://www.khronos.org/opengl/wiki/OpenGL_Error  //Use callback to catch error globally instead...
};


#endif //#ifndef UNIFORM_LOCATION_TRACKER