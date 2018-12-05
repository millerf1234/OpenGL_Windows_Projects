//See header file for details

#include "UniformLocationInterface.h"

//namespace ShaderInterface {

	using std::placeholders::_1; //see https://en.cppreference.com/w/cpp/utility/functional/function
	using std::placeholders::_2;

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


	UniformLocationInterface::UniformLocationInterface(UniformLocationInterface&& that) {
		initialize(); 
		mActivated_ = that.mActivated_;
		mUniformLocationTracker_ = std::move(that.mUniformLocationTracker_);
		transferFunctionPointers(that);
		that.mActivated_ = false;
	}


	UniformLocationInterface& UniformLocationInterface::operator=(UniformLocationInterface&& that) {
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
		
		//fptr_UpdateUniform1i_ = std::mem_fn(&UniformLocationTracker::updateUniform1f); //Really the C++11 way would be to write them all this way... but oh well
		fptr_UpdateUniform1i_ = std::bind(&UniformLocationTracker::updateUniform1i, mUniformLocationTracker_.get(), _1, _2);
		fptr_UpdateUniform1u_ = std::bind(&UniformLocationTracker::updateUniform1u, *(mUniformLocationTracker_.get()), _1);
		fptr_UpdateUniform1f_ = std::bind(&UniformLocationTracker::updateUniform1f, mUniformLocationTracker_.get(), _1);
		fptr_UpdateUniform1iv_ = std::bind(&UniformLocationTracker::updateUniform1iv, mUniformLocationTracker_.get(), _1);
		fptr_UpdateUniform1uv_ = std::bind(&UniformLocationTracker::updateUniform1uv, mUniformLocationTracker_.get(), _1);
		fptr_UpdateUniform1fv_ = std::bind(&UniformLocationTracker::updateUniform1fv, mUniformLocationTracker_.get(), _1);

		fptr_UpdateUniform2i_ = std::bind(&UniformLocationTracker::updateUniform2i, mUniformLocationTracker_.get(), _1);
		fptr_UpdateUniform2u_ = std::bind(&UniformLocationTracker::updateUniform2u, mUniformLocationTracker_.get(), _1);
		fptr_UpdateUniform2f_ = std::bind(&UniformLocationTracker::updateUniform1f, mUniformLocationTracker_.get(), _1);
		fptr_UpdateUniform2iv_ = std::bind(&UniformLocationTracker::updateUniform2iv, mUniformLocationTracker_.get(), _1);
		fptr_UpdateUniform2uv_ = std::bind(&UniformLocationTracker::updateUniform2uv, mUniformLocationTracker_.get(), _1);
		fptr_UpdateUniform2fv_ = std::bind(&UniformLocationTracker::updateUniform2fv, mUniformLocationTracker_.get(), _1);

		fptr_UpdateUniform3i_ = std::bind(&UniformLocationTracker::updateUniform3i, mUniformLocationTracker_.get(), _1);
		fptr_UpdateUniform3u_ = std::bind(&UniformLocationTracker::updateUniform3u, mUniformLocationTracker_.get(), _1);
		fptr_UpdateUniform3f_ = std::bind(&UniformLocationTracker::updateUniform1f, mUniformLocationTracker_.get(), _1);
		fptr_UpdateUniform3iv_ = std::bind(&UniformLocationTracker::updateUniform3iv, mUniformLocationTracker_.get(), _1);
		fptr_UpdateUniform3uv_ = std::bind(&UniformLocationTracker::updateUniform3uv, mUniformLocationTracker_.get(), _1);
		fptr_UpdateUniform3fv_ = std::bind(&UniformLocationTracker::updateUniform3fv, mUniformLocationTracker_.get(), _1);

		fptr_UpdateUniform4i_ = std::bind(&UniformLocationTracker::updateUniform4i, mUniformLocationTracker_.get(), _1);
		fptr_UpdateUniform4u_ = std::bind(&UniformLocationTracker::updateUniform4u, mUniformLocationTracker_.get(), _1);
		fptr_UpdateUniform4f_ = std::bind(&UniformLocationTracker::updateUniform4f, mUniformLocationTracker_.get(), _1);
		fptr_UpdateUniform4iv_ = std::bind(&UniformLocationTracker::updateUniform4iv, mUniformLocationTracker_.get(), _1);
		fptr_UpdateUniform4uv_ = std::bind(&UniformLocationTracker::updateUniform4uv, mUniformLocationTracker_.get(), _1);
		fptr_UpdateUniform4fv_ = std::bind(&UniformLocationTracker::updateUniform4fv, mUniformLocationTracker_.get(), _1);

		fptr_updateUniformMat2_ = std::bind(&UniformLocationTracker::updateUniformMat2, mUniformLocationTracker_.get(), _1);
		fptr_updateUniformMat2Array_ = std::bind(&UniformLocaTionTracker::updateUniformMat2, mUniformLocationTracker_.get(), _1);
		fptr_updateUniformMat2x3_ = std::bind(&UniformLocationTracker::updateUniformMat2x3, mUniformLocationTracker_.get(), _1);
		fptr_updateUniformMat2x3Array_ = std::bind(&UniformLocationTracker::updateUniformMat2x3, mUniformLocationTracker_.get(), _1);
		fptr_updateUniformMat2x4_ = std::bind(&UniformLocationTracker::updateUniformMat2x4, mUniformLocationTracker_.get(), _1);
		fptr_updateUniformMat2x4Array_ = std::bind(&UniformLocationTracker::updateUniformMat2x4, mUniformLocationTracker_.get(), _1);

		fptr_updateUniformMat3x2_ = std::bind(&UniformLocationTracker::updateUniformMat3x2, mUniformLocationTracker_.get(), _1);
		fptr_updateUniformMat3x2Array_ = std::bind(&UniformLocationTracker::updateUniformMat3x2, mUniformLocationTracker_.get(), _1);
		fptr_updateUniformMat3_ = std::bind(&UniformLocationTracker::updateUniformMat3, mUniformLocationTracker_.get(), _1);
		fptr_updateUniformMat3Array_ = std::bind(&UniformLocationTracker::updateUniformMat3, mUniformLocationTracker_.get(), _1);
		fptr_updateUniformMat3x4_ = std::bind(&UniformLocationTracker::updateUniformMat3x4, mUniformLocationTracker_.get(), _1);
		fptr_updateUniformMat3x4Array_ = std::bind(&UniformLocationTracker::updateUniformMat3x4, mUniformLocationTracker_.get(), _1);

		fptr_updateUniformMat4x2_ = std::bind(&UniformLocationTracker::updateUniformMat4x2, mUniformLocationTracker_.get(), _1);
		fptr_updateUniformMat4x2Array_ = std::bind(&UniformLocationTracker::updateUniformMat4x2, mUniformLocationTracker_.get(), _1);
		fptr_updateUniformMat4x3_ = std::bind(&UniformLocationTracker::updateUniformMat4x3, mUniformLocationTracker_.get(), _1);
		fptr_updateUniformMat4x3Array_ = std::bind(&UniformLocationTracker::updateUniformMat34x3, mUniformLocationTracker_.get(), _1);
		fptr_updateUniformMat4_ = std::bind(&UniformLocationTracker::updateUniformMat4, mUniformLocationTracker_.get(), _1);
		fptr_updateUniformMat4Array_ = std::bind(&UniformLocationTracker::updateUniformMat4, mUniformLocationTracker_.get(), _1);
	}

	void UniformLocationInterface::linkFunctionPointersToDeactivatedBehaviorFunction() {
		fptr_UpdateUniform1i_ = warnNotActive;
		fptr_UpdateUniform1u_ = warnNotActive;
		fptr_UpdateUniform1f_ = warnNotActive;
		fptr_UpdateUniform1iv_ = warnNotActive;
		fptr_UpdateUniform1uv_ = warnNotActive;
		fptr_UpdateUniform1fv_ = warnNotActive;

		fptr_UpdateUniform2i_ = warnNotActive;
		fptr_UpdateUniform2u_ = warnNotActive;
		fptr_UpdateUniform2f_ = warnNotActive;
		fptr_UpdateUniform2iv_ = warnNotActive;
		fptr_UpdateUniform2uv_ = warnNotActive;
		fptr_UpdateUniform2fv_ = warnNotActive;

		fptr_UpdateUniform3i_ = warnNotActive;
		fptr_UpdateUniform3u_ = warnNotActive;
		fptr_UpdateUniform3f_ = warnNotActive;
		fptr_UpdateUniform3iv_ = warnNotActive;
		fptr_UpdateUniform3uv_ = warnNotActive;
		fptr_UpdateUniform3fv_ = warnNotActive;

		fptr_UpdateUniform4i_ = warnNotActive;
		fptr_UpdateUniform4u_ = warnNotActive;
		fptr_UpdateUniform4f_ = warnNotActive;
		fptr_UpdateUniform4iv_ = warnNotActive;
		fptr_UpdateUniform4uv_ = warnNotActive;
		fptr_UpdateUniform4fv_ = warnNotActive;

		fptr_updateUniformMat2_ = warnNotActive;
		fptr_updateUniformMat2Array_ = warnNotActive;
		fptr_updateUniformMat2x3_ = warnNotActive;
		fptr_updateUniformMat2x3Array_ = warnNotActive;
		fptr_updateUniformMat2x4_ = warnNotActive;
		fptr_updateUniformMat2x4Array_ = warnNotActive;

		fptr_updateUniformMat3x2_ = warnNotActive;
		fptr_updateUniformMat3x2Array_ = warnNotActive;
		fptr_updateUniformMat3_ = warnNotActive;
		fptr_updateUniformMat3Array_ = warnNotActive;
		fptr_updateUniformMat3x4_ = warnNotActive;
		fptr_updateUniformMat3x4Array_ = warnNotActive;

		fptr_updateUniformMat4x2_ = warnNotActive;
		fptr_updateUniformMat4x2Array_ = warnNotActive;
		fptr_updateUniformMat4x3_ = warnNotActive;
		fptr_updateUniformMat4x3Array_ = warnNotActive;
		fptr_updateUniformMat4_ = warnNotActive;
		fptr_updateUniformMat4Array_ = warnNotActive;
	}
	

//} //namespace ShaderInterface
