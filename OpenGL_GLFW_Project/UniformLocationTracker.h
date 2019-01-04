//This class is intented to help a ShaderProgram object manage it's tracked uniform locations. 
//Note that this class is a friend of CachedUniformLocation
//
// Usage comment: The easiest way to use this class is to just call the appropriate 'updateUniformxx()' functions
//				  while the corrosponding ShaderProgram is bound as active and with parameters matching the name of
//				  the uniform in the program and with the data value. 
//				
//				  For (theoretically) faster uniform updates it is possible to call one of the 'getCachedUniformxx()'
//				  functions which return a weak_ptr to a cached uniform location. This returned pointer can then be 
//				  used to update uniforms directly, skipping the lookup this object has to perform each time one of the
//			      simpler 'updateUniformxx()' functions is called.
//
// Dependencies:  This class requires the inclusion of the GLM extension "type_ptr" which allows GLM vectors and
//                matrices to be used for the uniform updates. 
//
//Created by Forrest Miller on July 20, 2018 
//Completed on July 24, 2018 by Forrest Miller


#pragma once

#ifndef UNIFORM_LOCATION_TRACKER
#define UNIFORM_LOCATION_TRACKER

#include <list>
#include <vector>
#include <unordered_map>
#include <memory>

#include "GlobalIncludes.h"
#include "UniformLocationBucket.h"
#include "CachedUniformLocation.h"

namespace ShaderInterface {

	class CachedUniformLocation; //Instances of CachedUniformLocation objects can be created and handed out through functions calls in this class

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



		//Update uniform Functions
		//see: https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glUniform.xhtml
		void updateUniform1i(const GLchar *, const GLint);
		void updateUniform1u(const GLchar *, const GLuint);
		void updateUniform1f(const GLchar *, const GLfloat);
		void updateUniform1iv(const GLchar *, const GLint *, GLsizei count = 1); //A vector is a pointer to an array of the element instead of seperate arguments
		void updateUniform1uv(const GLchar *, const GLuint *, GLsizei count = 1);
		void updateUniform1fv(const GLchar *, const GLfloat *, GLsizei count = 1);

		void updateUniform2i(const GLchar *, const GLint, const GLint);
		void updateUniform2u(const GLchar *, const GLuint, const GLuint);
		void updateUniform2f(const GLchar *, const GLfloat, const GLfloat);
		void updateUniform2iv(const GLchar *, const GLint *, GLsizei count = 1);
		void updateUniform2uv(const GLchar *, const GLuint *, GLsizei count = 1);
		void updateUniform2fv(const GLchar *, const GLfloat *, GLsizei count = 1);

		void updateUniform3i(const GLchar *, const GLint, const GLint, const GLint);
		void updateUniform3u(const GLchar *, const GLuint, const GLuint, const GLuint);
		void updateUniform3f(const GLchar *, const GLfloat, const GLfloat, const GLfloat);
		void updateUniform3iv(const GLchar *, const GLint *, GLsizei count = 1);
		void updateUniform3uv(const GLchar *, const GLuint *, GLsizei count = 1);
		void updateUniform3fv(const GLchar *, const GLfloat *, GLsizei count = 1);

		void updateUniform4i(const GLchar *, const GLint, const GLint, const GLint, const GLint);
		void updateUniform4u(const GLchar *, const GLuint, const GLuint, const GLuint, const GLuint);
		void updateUniform4f(const GLchar *, const GLfloat, const GLfloat, const GLfloat, const GLfloat);
		void updateUniform4iv(const GLchar *, const GLint *, GLsizei count = 1);
		void updateUniform4uv(const GLchar *, const GLuint *, GLsizei count = 1);
		void updateUniform4fv(const GLchar *, const GLfloat *, GLsizei count = 1);

		//For the matrices, it is possible to use either a GLM::mat of the
		//corresponding type or to just provide a GLfloat * representing the
		//data. Count can be used to update an array of matrices.

		void updateUniformMat2(const GLchar*, const glm::mat2 *, GLsizei count = 1);
		void updateUniformMat2(const GLchar *, const GLfloat *, GLsizei count = 1);
		void updateUniformMat2x2(const GLchar*, const glm::mat2 *, GLsizei count = 1);
		void updateUniformMat2x2(const GLchar *, const GLfloat *, GLsizei count = 1);
		void updateUniformMat2x3(const GLchar *, const glm::mat2x3 *, GLsizei count = 1);
		void updateUniformMat2x3(const GLchar *, const GLfloat *, GLsizei count = 1);
		void updateUniformMat2x4(const GLchar *, const glm::mat2x4 *, GLsizei count = 1);
		void updateUniformMat2x4(const GLchar *, const GLfloat *, GLsizei count = 1);

		
		void updateUniformMat3x2(const GLchar *, const glm::mat3x2 *, GLsizei count = 1);
		void updateUniformMat3x2(const GLchar *, const GLfloat *, GLsizei count = 1);
		void updateUniformMat3(const GLchar *, const glm::mat3 *, GLsizei count = 1);
		void updateUniformMat3(const GLchar *, const GLfloat *, GLsizei count = 1);
		void updateUniformMat3x3(const GLchar *, const glm::mat3 *, GLsizei count = 1);
		void updateUniformMat3x3(const GLchar *, const GLfloat *, GLsizei count = 1);
		void updateUniformMat3x4(const GLchar *, const glm::mat3x4 *, GLsizei count = 1);
		void updateUniformMat3x4(const GLchar *, const GLfloat *, GLsizei count = 1);


		void updateUniformMat4x2(const GLchar *, const glm::mat4x2 *, GLsizei count = 1);
		void updateUniformMat4x2(const GLchar *, const GLfloat *, GLsizei count = 1);
		void updateUniformMat4x3(const GLchar *, const glm::mat4x3 *, GLsizei count = 1);
		void updateUniformMat4x3(const GLchar *, const GLfloat *, GLsizei count = 1);
		void updateUniformMat4(const GLchar *, const glm::mat4 *, GLsizei count = 1);
		void updateUniformMat4(const GLchar *, const GLfloat *, GLsizei count = 1);
		void updateUniformMat4x4(const GLchar *, const glm::mat4 *, GLsizei count = 1);
		void updateUniformMat4x4(const GLchar *, const GLfloat *, GLsizei count = 1);

		//Get CachedUniformLocation functions
		std::weak_ptr<CachedUniformLocation> getCachedUniform1i(const GLchar *);
		std::weak_ptr<CachedUniformLocation> getCachedUniform1u(const GLchar *);
		std::weak_ptr<CachedUniformLocation> getCachedUniform1f(const GLchar *);
		std::weak_ptr<CachedUniformLocation> getCachedUniform1iv(const GLchar *, GLsizei count = 1); //A vector is a pointer to an array of the element instead of seperate arguments
		std::weak_ptr<CachedUniformLocation> getCachedUniform1uv(const GLchar *, GLsizei count = 1);
		std::weak_ptr<CachedUniformLocation> getCachedUniform1fv(const GLchar *, GLsizei count = 1);

		std::weak_ptr<CachedUniformLocation> getCachedUniform2i(const GLchar *);
		std::weak_ptr<CachedUniformLocation> getCachedUniform2u(const GLchar *);
		std::weak_ptr<CachedUniformLocation> getCachedUniform2f(const GLchar *);
		std::weak_ptr<CachedUniformLocation> getCachedUniform2iv(const GLchar *, GLsizei count = 1);
		std::weak_ptr<CachedUniformLocation> getCachedUniform2uv(const GLchar *, GLsizei count = 1);
		std::weak_ptr<CachedUniformLocation> getCachedUniform2fv(const GLchar *, GLsizei count = 1);

		std::weak_ptr<CachedUniformLocation> getCachedUniform3i(const GLchar *);
		std::weak_ptr<CachedUniformLocation> getCachedUniform3u(const GLchar *);
		std::weak_ptr<CachedUniformLocation> getCachedUniform3f(const GLchar *);
		std::weak_ptr<CachedUniformLocation> getCachedUniform3iv(const GLchar *, GLsizei count = 1);
		std::weak_ptr<CachedUniformLocation> getCachedUniform3uv(const GLchar *, GLsizei count = 1);
		std::weak_ptr<CachedUniformLocation> getCachedUniform3fv(const GLchar *, GLsizei count = 1);

		std::weak_ptr<CachedUniformLocation> getCachedUniform4i(const GLchar *);
		std::weak_ptr<CachedUniformLocation> getCachedUniform4u(const GLchar *);
		std::weak_ptr<CachedUniformLocation> getCachedUniform4f(const GLchar *);
		std::weak_ptr<CachedUniformLocation> getCachedUniform4iv(const GLchar *, GLsizei count = 1);
		std::weak_ptr<CachedUniformLocation> getCachedUniform4uv(const GLchar *, GLsizei count = 1);
		std::weak_ptr<CachedUniformLocation> getCachedUniform4fv(const GLchar *, GLsizei count = 1);
		
		std::weak_ptr<CachedUniformLocation> getCachedUnifromMat2(const GLchar *, GLsizei count = 1);
		std::weak_ptr<CachedUniformLocation> getCachedUniformMat2x2(const GLchar *, GLsizei count = 1);
		std::weak_ptr<CachedUniformLocation> getCachedUniformMat2x3(const GLchar *, GLsizei count = 1);
		std::weak_ptr<CachedUniformLocation> getCachedUniformMat2x4(const GLchar *, GLsizei count = 1);

		std::weak_ptr<CachedUniformLocation> getCachedUniformMat3x2(const GLchar *, GLsizei count = 1);
		std::weak_ptr<CachedUniformLocation> getCachedUniformMat3(const GLchar *, GLsizei count = 1);
		std::weak_ptr<CachedUniformLocation> getCachedUniformMat3x3(const GLchar *, GLsizei count = 1);
		std::weak_ptr<CachedUniformLocation> getCachedUniformMat3x4(const GLchar *, GLsizei count = 1);

		std::weak_ptr<CachedUniformLocation> getCachedUniformMat4x2(const GLchar *, GLsizei count = 1);
		std::weak_ptr<CachedUniformLocation> getCachedUniformMat4x3(const GLchar *, GLsizei count = 1);
		std::weak_ptr<CachedUniformLocation> getCachedUniformMat4(const GLchar *, GLsizei count = 1);
		std::weak_ptr<CachedUniformLocation> getCachedUniformMat4x4(const GLchar *, GLsizei count = 1);

		//Gets the number of uniform locations being tracked directly by this object. Currently it is possible for a
		//CachedUniformLocation to be requested which bypasses this object's internal storing of UniformLocationBuckets, so this 
		//count might not actually be the total number of tracked uniforms. Perhaps a better function name would be 
		//getNumberOfInternallyTrackedUniforms(). But that still is a little misleading because technically this object
		//also manages the hash table of Cached Uniforms. 
		size_t getNumberOfTrackedUniforms() const { return mTrackedLocationsCount; }

		//Returns the number of unique CachedUniformLocations that have been given away by this object. In other
		//words, giving away multiple shared_pointers to the same UniformLocation would only count as 1 in the tally.
		size_t getNumberOfCachedUniforms() const { return mCachedLocationsCount; }

		//Returns the GL-assigned program ID that this objects uniform locations are associated with.
		GLuint getProgramID() const { return mProgramID; }

	private: //See .cpp file for implementation details
		GLuint mProgramID;
		size_t mTrackedLocationsCount;
		size_t mCachedLocationsCount;

		std::vector<std::list<UniformLocationBucket>> mUniformLocationLists; //A vector of lists of uniform locations
		std::unordered_map<std::string, std::shared_ptr<CachedUniformLocation>> mCachedUniformLocations;

		//List index for each type (used with mUniformLocationLists)
		uint_fast8_t i1_, u1_, f1_, iv1_, uv1_, fv1_;
		uint_fast8_t i2_, u2_, f2_, iv2_, uv2_, fv2_;
		uint_fast8_t i3_, u3_, f3_, iv3_, uv3_, fv3_;
		uint_fast8_t i4_, u4_, f4_, iv4_, uv4_, fv4_;
		uint_fast8_t M2x2_, M2x3_, M2x4_, M3x2_, M3x3_, M3x4_, M4x2_, M4x3_, M4x4_;
		uint_fast8_t mNextListIndex;

		//Helper functions 
		void initializeListLocations(); //Only call from constructor

		//Helpers for updating uniform locations
		GLint lookupUniformLocation(uint_fast8_t& listIndex, const char *, UniformType); //listIndex may be updated during function call
		GLint findInList(uint_fast8_t, const char *); //Returns a uniform location or -2 if not found in list  (-1 already has meaning within OpenGL)

		//Helper functions for managing cachedUniformLocations
		bool seeIfUniformLocationHasAlreadyBeenCached(const GLchar *);
		std::shared_ptr<CachedUniformLocation> addCachedUniformLocation(const GLchar * uniformName, UniformType, GLsizei count = 1);

	};


} //namespace ShaderInterface

#endif //#ifndef UNIFORM_LOCATION_TRACKER