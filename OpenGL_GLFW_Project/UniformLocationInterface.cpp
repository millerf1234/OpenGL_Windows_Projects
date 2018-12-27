//See header file for details

#include "UniformLocationInterface.h"

namespace ShaderInterface {

	using namespace std::placeholders; //see https://en.cppreference.com/w/cpp/utility/functional/function

	void UniformLocationInterface::initialize() {
		mActivated_ = false;
		mUniformLocationTracker_ = nullptr;
	}

	UniformLocationInterface::UniformLocationInterface() {
		initialize();
		linkFunctionPointersToDeactivatedBehaviorFunction();
	}

	UniformLocationInterface::UniformLocationInterface(const GLuint programID) {
		initialize(); 
		activateUniformLocationTracker(programID);
	}

	UniformLocationInterface::~UniformLocationInterface() {

	}


	UniformLocationInterface::UniformLocationInterface(UniformLocationInterface&& that) noexcept {
		initialize(); 
		mActivated_ = that.mActivated_;
		mUniformLocationTracker_ = std::move(that.mUniformLocationTracker_);
		transferFunctionPointers(that);
		that.mActivated_ = false;
	}


	UniformLocationInterface& UniformLocationInterface::operator=(UniformLocationInterface&& that) noexcept {
		if (this != &that) {
			mActivated_ = that.mActivated_;
			mUniformLocationTracker_ = std::move(that.mUniformLocationTracker_);
			transferFunctionPointers(that);
			that.mActivated_ = false;
		}
		return *this;
	}

	void UniformLocationInterface::activateUniformLocationTracker(const GLuint programID) {
		if (mActivated_) {
			fprintf(ERRLOG, "\nError! Unable to activate UniformLocationTracker in UniformLocationInterface %u!"
				"This UniformLocationInterface has already been activated for program %u!\n", programID, getProgramID());
			return;
		}
		mUniformLocationTracker_ = std::make_unique<UniformLocationTracker>(programID);
		if (mUniformLocationTracker_) {
			linkFunctionPointersToUniformLocationTracker();
			mActivated_ = true;
		}
	}

	size_t UniformLocationInterface::getNumberOfTrackedUniforms() const {
		if (mActivated_) {
			return mUniformLocationTracker_->getNumberOfTrackedUniforms();
		}
		return 0u;
	}

	size_t UniformLocationInterface::getNumberOfCachedUniforms() const {
		if (mActivated_) {
			return mUniformLocationTracker_->getNumberOfCachedUniforms();
		}
		return 0u;
	}

	GLuint UniformLocationInterface::getProgramID() const {
		if (mActivated_) {
			return mUniformLocationTracker_->getProgramID();
		}
		return 0u;
	}

	void UniformLocationInterface::transferFunctionPointers(UniformLocationInterface & that) {
		fptr_UpdateUniform1i_ = that.fptr_UpdateUniform1i_;
		fptr_UpdateUniform1u_ = that.fptr_UpdateUniform1u_;
		fptr_UpdateUniform1f_ = that.fptr_UpdateUniform1f_;
		fptr_UpdateUniform1iv_ = that.fptr_UpdateUniform1iv_;
		fptr_UpdateUniform1uv_ = that.fptr_UpdateUniform1uv_;
		fptr_UpdateUniform1fv_ = that.fptr_UpdateUniform1fv_;

		fptr_UpdateUniform2i_ = that.fptr_UpdateUniform2i_;
		fptr_UpdateUniform2u_ = that.fptr_UpdateUniform2u_;
		fptr_UpdateUniform2f_ = that.fptr_UpdateUniform2f_;
		fptr_UpdateUniform2iv_ = that.fptr_UpdateUniform2iv_;
		fptr_UpdateUniform2uv_ = that.fptr_UpdateUniform2uv_;
		fptr_UpdateUniform2fv_ = that.fptr_UpdateUniform2fv_;

		fptr_UpdateUniform3i_ = that.fptr_UpdateUniform3i_;
		fptr_UpdateUniform3u_ = that.fptr_UpdateUniform3u_;
		fptr_UpdateUniform3f_ = that.fptr_UpdateUniform3f_;
		fptr_UpdateUniform3iv_ = that.fptr_UpdateUniform3iv_;
		fptr_UpdateUniform3uv_ = that.fptr_UpdateUniform3uv_;
		fptr_UpdateUniform3fv_ = that.fptr_UpdateUniform3fv_;

		fptr_UpdateUniform4i_ = that.fptr_UpdateUniform4i_;
		fptr_UpdateUniform4u_ = that.fptr_UpdateUniform4u_;
		fptr_UpdateUniform4f_ = that.fptr_UpdateUniform4f_;
		fptr_UpdateUniform4iv_ = that.fptr_UpdateUniform4iv_;
		fptr_UpdateUniform4uv_ = that.fptr_UpdateUniform4uv_;
		fptr_UpdateUniform4fv_ = that.fptr_UpdateUniform4fv_;

		fptr_UpdateUniformMat2_ = that.fptr_UpdateUniformMat2_;
		fptr_UpdateUniformMat2Array_ = fptr_UpdateUniformMat2Array_;
		fptr_UpdateUniformMat2x3_ = that.fptr_UpdateUniformMat2x3_;
		fptr_UpdateUniformMat2x3Array_ = fptr_UpdateUniformMat2x3Array_;
		fptr_UpdateUniformMat2x4_ = that.fptr_UpdateUniformMat2x4_;
		fptr_UpdateUniformMat2x4Array_ = fptr_UpdateUniformMat2x4Array_;

		fptr_UpdateUniformMat3x2_ = that.fptr_UpdateUniformMat3x2_;
		fptr_UpdateUniformMat3x2Array_ = fptr_UpdateUniformMat3x2Array_;
		fptr_UpdateUniformMat3_ = that.fptr_UpdateUniformMat3_;
		fptr_UpdateUniformMat3Array_ = fptr_UpdateUniformMat3Array_;
		fptr_UpdateUniformMat3x4_ = that.fptr_UpdateUniformMat3x4_;
		fptr_UpdateUniformMat3x4Array_ = fptr_UpdateUniformMat3x4Array_;

		fptr_UpdateUniformMat4x2_ = that.fptr_UpdateUniformMat4x2_;
		fptr_UpdateUniformMat4x2Array_ = fptr_UpdateUniformMat4x2Array_;
		fptr_UpdateUniformMat4x3_ = that.fptr_UpdateUniformMat4x3_;
		fptr_UpdateUniformMat4x3Array_ = fptr_UpdateUniformMat4x3Array_;
		fptr_UpdateUniformMat4_ = that.fptr_UpdateUniformMat4_;
		fptr_UpdateUniformMat4Array_ = fptr_UpdateUniformMat4Array_;

		that.linkFunctionPointersToDeactivatedBehaviorFunction(); //Deactived the source's function pointers
	}

	void UniformLocationInterface::linkFunctionPointersToUniformLocationTracker() {
		//These function calls might look really complicated, but all they are really doing is setting each fptr to the corresponding
		//member function of this objects' UniformLocationTracker 'mUniformLocationTracker_'. The std::bind is necessary because
		//every member function for any object has an implicit first parameter (called 'this') which is just a pointer to the object
		//the function is a member of. The std::bind makes sure this 'this' pointer to the mUniformLocationTracker_ gets inserted as
		//the first argument whenever a fptr_UpdateUniformx is called.
		
		//fptr_UpdateUniform1i_ = std::mem_fn(&UniformLocationTracker::updateUniform1f); //In theory it could be done this way too... But this didn't work for me
		fptr_UpdateUniform1i_ = std::bind(&UniformLocationTracker::updateUniform1i, mUniformLocationTracker_.get(), _1, _2);  //_1, _2  are called placeholders
		fptr_UpdateUniform1u_ = std::bind(&UniformLocationTracker::updateUniform1u, *(mUniformLocationTracker_.get()), _1, _2);
		fptr_UpdateUniform1f_ = std::bind(&UniformLocationTracker::updateUniform1f, mUniformLocationTracker_.get(), _1, _2);
		fptr_UpdateUniform1iv_ = std::bind(&UniformLocationTracker::updateUniform1iv, mUniformLocationTracker_.get(), _1, _2, _3);
		fptr_UpdateUniform1uv_ = std::bind(&UniformLocationTracker::updateUniform1uv, mUniformLocationTracker_.get(), _1, _2, _3 );
		fptr_UpdateUniform1fv_ = std::bind(&UniformLocationTracker::updateUniform1fv, mUniformLocationTracker_.get(), _1, _2, _3);

		fptr_UpdateUniform2i_ = std::bind(&UniformLocationTracker::updateUniform2i, mUniformLocationTracker_.get(), _1, _2, _3);
		fptr_UpdateUniform2u_ = std::bind(&UniformLocationTracker::updateUniform2u, mUniformLocationTracker_.get(), _1, _2, _3);
		fptr_UpdateUniform2f_ = std::bind(&UniformLocationTracker::updateUniform2f, mUniformLocationTracker_.get(), _1, _2, _3);
		fptr_UpdateUniform2iv_ = std::bind(&UniformLocationTracker::updateUniform2iv, mUniformLocationTracker_.get(), _1, _2, _3);
		fptr_UpdateUniform2uv_ = std::bind(&UniformLocationTracker::updateUniform2uv, mUniformLocationTracker_.get(), _1, _2, _3);
		fptr_UpdateUniform2fv_ = std::bind(&UniformLocationTracker::updateUniform2fv, mUniformLocationTracker_.get(), _1, _2, _3);

		fptr_UpdateUniform3i_ = std::bind(&UniformLocationTracker::updateUniform3i, mUniformLocationTracker_.get(), _1, _2, _3, _4);
		fptr_UpdateUniform3u_ = std::bind(&UniformLocationTracker::updateUniform3u, mUniformLocationTracker_.get(), _1, _2, _3, _4);
		fptr_UpdateUniform3f_ = std::bind(&UniformLocationTracker::updateUniform3f, mUniformLocationTracker_.get(), _1, _2, _3, _4);
		fptr_UpdateUniform3iv_ = std::bind(&UniformLocationTracker::updateUniform3iv, mUniformLocationTracker_.get(), _1, _2, _3);
		fptr_UpdateUniform3uv_ = std::bind(&UniformLocationTracker::updateUniform3uv, mUniformLocationTracker_.get(), _1, _2, _3);
		fptr_UpdateUniform3fv_ = std::bind(&UniformLocationTracker::updateUniform3fv, mUniformLocationTracker_.get(), _1, _2, _3);

		fptr_UpdateUniform4i_ = std::bind(&UniformLocationTracker::updateUniform4i, mUniformLocationTracker_.get(), _1, _2, _3, _4, _5);
		fptr_UpdateUniform4u_ = std::bind(&UniformLocationTracker::updateUniform4u, mUniformLocationTracker_.get(), _1, _2, _3, _4, _5);
		fptr_UpdateUniform4f_ = std::bind(&UniformLocationTracker::updateUniform4f, mUniformLocationTracker_.get(), _1, _2, _3, _4, _5);
		fptr_UpdateUniform4iv_ = std::bind(&UniformLocationTracker::updateUniform4iv, mUniformLocationTracker_.get(), _1, _2, _3);
		fptr_UpdateUniform4uv_ = std::bind(&UniformLocationTracker::updateUniform4uv, mUniformLocationTracker_.get(), _1, _2, _3);
		fptr_UpdateUniform4fv_ = std::bind(&UniformLocationTracker::updateUniform4fv, mUniformLocationTracker_.get(), _1, _2, _3);

		//So std::bind struggles here a bit because the 'updateUniformMatx()' functions are overloaded for both glm matrices and float arrays. Luckily,
		//based  off a hint I found at https://anteru.net/blog/2007/05/17/177/index.html there is a way to static_cast each overloaded function to its
		//correct signature. Unfortunatly it takes the already-ugly syntax up to 11. Enjoy! 
		fptr_UpdateUniformMat2_ = std::bind(static_cast<void (UniformLocationTracker::*)(const GLchar*, const glm::mat2 *, GLsizei)> (&UniformLocationTracker::updateUniformMat2), mUniformLocationTracker_.get(), _1, _2, _3);
		fptr_UpdateUniformMat2Array_ = std::bind(static_cast<void (UniformLocationTracker::*)(const GLchar*, const GLfloat *, GLsizei)> (&UniformLocationTracker::updateUniformMat2), mUniformLocationTracker_.get(), _1, _2, _3);
		fptr_UpdateUniformMat2x3_ = std::bind(static_cast<void (UniformLocationTracker::*)(const GLchar*, const glm::mat2x3 *, GLsizei)> (&UniformLocationTracker::updateUniformMat2x3), mUniformLocationTracker_.get(), _1, _2, _3);
		fptr_UpdateUniformMat2x3Array_ = std::bind(static_cast<void (UniformLocationTracker::*)(const GLchar*, const GLfloat *, GLsizei)> (&UniformLocationTracker::updateUniformMat2x3), mUniformLocationTracker_.get(), _1, _2, _3);
		fptr_UpdateUniformMat2x4_ = std::bind(static_cast<void (UniformLocationTracker::*)(const GLchar*, const glm::mat2x4 *, GLsizei)> (&UniformLocationTracker::updateUniformMat2x4), mUniformLocationTracker_.get(), _1, _2, _3);
		fptr_UpdateUniformMat2x4Array_ = std::bind(static_cast<void (UniformLocationTracker::*)(const GLchar*, const GLfloat *, GLsizei)> (&UniformLocationTracker::updateUniformMat2x4), mUniformLocationTracker_.get(), _1, _2, _3);

		fptr_UpdateUniformMat3x2_ = std::bind(static_cast<void (UniformLocationTracker::*)(const GLchar*, const glm::mat3x2 *, GLsizei)> (&UniformLocationTracker::updateUniformMat3x2), mUniformLocationTracker_.get(), _1, _2, _3);
		fptr_UpdateUniformMat3x2Array_ = std::bind(static_cast<void (UniformLocationTracker::*)(const GLchar*, const GLfloat *, GLsizei)> (&UniformLocationTracker::updateUniformMat3x2), mUniformLocationTracker_.get(), _1, _2, _3);
		fptr_UpdateUniformMat3_ = std::bind(static_cast<void (UniformLocationTracker::*)(const GLchar*, const glm::mat3 *, GLsizei)> (&UniformLocationTracker::updateUniformMat3), mUniformLocationTracker_.get(), _1, _2, _3);
		fptr_UpdateUniformMat3Array_ = std::bind(static_cast<void (UniformLocationTracker::*)(const GLchar*, const GLfloat *, GLsizei)> (&UniformLocationTracker::updateUniformMat3), mUniformLocationTracker_.get(), _1, _2, _3);
		fptr_UpdateUniformMat3x4_ = std::bind(static_cast<void (UniformLocationTracker::*)(const GLchar*, const glm::mat3x4 *, GLsizei)> (&UniformLocationTracker::updateUniformMat3x4), mUniformLocationTracker_.get(), _1, _2, _3);
		fptr_UpdateUniformMat3x4Array_ = std::bind(static_cast<void (UniformLocationTracker::*)(const GLchar*, const GLfloat *, GLsizei)> (&UniformLocationTracker::updateUniformMat3x4), mUniformLocationTracker_.get(), _1, _2, _3);

		fptr_UpdateUniformMat4x2_ = std::bind(static_cast<void (UniformLocationTracker::*)(const GLchar*, const glm::mat4x2 *, GLsizei)> (&UniformLocationTracker::updateUniformMat4x2), mUniformLocationTracker_.get(), _1, _2, _3);
		fptr_UpdateUniformMat4x2Array_ = std::bind(static_cast<void (UniformLocationTracker::*)(const GLchar*, const GLfloat *, GLsizei)> (&UniformLocationTracker::updateUniformMat4x2), mUniformLocationTracker_.get(), _1, _2, _3);
		fptr_UpdateUniformMat4x3_ = std::bind(static_cast<void (UniformLocationTracker::*)(const GLchar*, const glm::mat4x3 *, GLsizei)> (&UniformLocationTracker::updateUniformMat4x3), mUniformLocationTracker_.get(), _1, _2, _3);
		fptr_UpdateUniformMat4x3Array_ = std::bind(static_cast<void (UniformLocationTracker::*)(const GLchar*, const GLfloat *, GLsizei)> (&UniformLocationTracker::updateUniformMat4x3), mUniformLocationTracker_.get(), _1, _2, _3);
		fptr_UpdateUniformMat4_ = std::bind(static_cast<void (UniformLocationTracker::*)(const GLchar*, const glm::mat4 *, GLsizei)> (&UniformLocationTracker::updateUniformMat4), mUniformLocationTracker_.get(), _1, _2, _3);
		fptr_UpdateUniformMat4Array_ = std::bind(static_cast<void (UniformLocationTracker::*)(const GLchar*, const GLfloat *, GLsizei)> (&UniformLocationTracker::updateUniformMat4), mUniformLocationTracker_.get(), _1, _2, _3);
	}

	void UniformLocationInterface::linkFunctionPointersToDeactivatedBehaviorFunction() {
		fptr_UpdateUniform1i_ = warnNotActive1i;
		fptr_UpdateUniform1u_ = warnNotActive1u;
		fptr_UpdateUniform1f_ = warnNotActive1f;
		fptr_UpdateUniform1iv_ = warnNotActiveiv;
		fptr_UpdateUniform1uv_ = warnNotActiveuv;
		fptr_UpdateUniform1fv_ = warnNotActivefv;

		fptr_UpdateUniform2i_ = warnNotActive2i;
		fptr_UpdateUniform2u_ = warnNotActive2u;
		fptr_UpdateUniform2f_ = warnNotActive2f;
		fptr_UpdateUniform2iv_ = warnNotActiveiv;
		fptr_UpdateUniform2uv_ = warnNotActiveuv;
		fptr_UpdateUniform2fv_ = warnNotActivefv;

		fptr_UpdateUniform3i_ = warnNotActive3i;
		fptr_UpdateUniform3u_ = warnNotActive3u;
		fptr_UpdateUniform3f_ = warnNotActive3f;
		fptr_UpdateUniform3iv_ = warnNotActiveiv;
		fptr_UpdateUniform3uv_ = warnNotActiveuv;
		fptr_UpdateUniform3fv_ = warnNotActivefv;

		fptr_UpdateUniform4i_ = warnNotActive4i;
		fptr_UpdateUniform4u_ = warnNotActive4u;
		fptr_UpdateUniform4f_ = warnNotActive4f;
		fptr_UpdateUniform4iv_ = warnNotActiveiv;
		fptr_UpdateUniform4uv_ = warnNotActiveuv;
		fptr_UpdateUniform4fv_ = warnNotActivefv;

		fptr_UpdateUniformMat2_ = warnNotActiveMat2;
		fptr_UpdateUniformMat2Array_ = warnNotActivefv;
		fptr_UpdateUniformMat2x3_ = warnNotActiveMat2x3;
		fptr_UpdateUniformMat2x3Array_ = warnNotActivefv;
		fptr_UpdateUniformMat2x4_ = warnNotActiveMat2x4;
		fptr_UpdateUniformMat2x4Array_ = warnNotActivefv;

		fptr_UpdateUniformMat3x2_ = warnNotActiveMat3x2;
		fptr_UpdateUniformMat3x2Array_ = warnNotActivefv;
		fptr_UpdateUniformMat3_ = warnNotActiveMat3;
		fptr_UpdateUniformMat3Array_ = warnNotActivefv;
		fptr_UpdateUniformMat3x4_ = warnNotActiveMat3x4;
		fptr_UpdateUniformMat3x4Array_ = warnNotActivefv;

		fptr_UpdateUniformMat4x2_ = warnNotActiveMat4x2;
		fptr_UpdateUniformMat4x2Array_ = warnNotActivefv;
		fptr_UpdateUniformMat4x3_ = warnNotActiveMat4x3;
		fptr_UpdateUniformMat4x3Array_ = warnNotActivefv;
		fptr_UpdateUniformMat4_ = warnNotActiveMat4;
		fptr_UpdateUniformMat4Array_ = warnNotActivefv;
	}
	

} //namespace ShaderInterface
