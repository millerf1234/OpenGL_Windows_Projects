// (Note 7/24/2018)
//   Most of the following description is true for this class but there may be some
//    slight inconsistencies going forward. I intend to rewrite my descriptions once I
//	  get more of the implementation settled and I get used to working with these objects.
//
//
//A simple structure for tracking uniform locations. Copying is allowed
//Note that these buckets just hold a number representing a uniform's location and don't store any information on the uniforms value(s)
//
// NOTE: The difference between this struct and the CachedUniformLocation class is this struct is just a wrapper for data and is
//		 mostly intended for internal-use-only by a UniformLocationTracker. Whilestwhile, the UniformLocationTracker comes with an
//		 interface for allowing calls to bypass the UniformLocationTracker to result in faster updates.
//
// Actually I tweaked my implementation slightly right after first writing this, so having these buckets know their uniform type is probably surpurfulous since they
// will be arranged in their containers by type. 
//
//
//Created by Forrest Miller on 7/20/2018

#pragma once

#ifndef UNIFORM_LOCATION_BUCKET
#define UNIFORM_LOCATION_BUCKET

#ifndef WRNLOG
#define WRNLOG stderr
#endif 

#include <string>

#include "GlobalIncludes.h"

namespace ShaderInterface {

	enum class UniformType {
		INT1, UINT1, FLOAT1, INT1V, UINT1V, FLOAT1V,
		INT2, UINT2, FLOAT2, INT2V, UINT2V, FLOAT2V,
		INT3, UINT3, FLOAT3, INT3V, UINT3V, FLOAT3V,
		INT4, UINT4, FLOAT4, INT4V, UINT4V, FLOAT4V,
		MAT2, MAT3, MAT4, MAT2X3, MAT3X2, MAT2X4, MAT4X2, MAT3X4, MAT4X3,   //All matrices are of fv (float vector) type
		UNSPECIFIED
	};

	class UniformLocationBucket final {
	public:
		//Constructor
		UniformLocationBucket() = delete;
		UniformLocationBucket(GLuint programID, const char * str, UniformType type = UniformType::UNSPECIFIED) {
			mType_ = type;
			mName_ = std::string(str); //The odd syntax here just ensures an unnecessary copy is not made 
			mLocation_ = glGetUniformLocation(programID, str);  //Crashes here if programID is invalid...
			if (mLocation_ == -1) {
				fprintf(WRNLOG, "\nWarning! Unable to get uniform location for uniform: %s\n", str);
				fprintf(WRNLOG, "This is expected behavior if uniform is unused within the shader\n");
				mError_ = true;
			}
			else {
				mError_ = false;
			}
		}
		//Destructor
        ~UniformLocationBucket() = default;
		//Copy/Move fully enabled
		UniformLocationBucket(const UniformLocationBucket& that) {
			this->mLocation_ = that.mLocation_;
			this->mName_ = that.mName_;
			this->mError_ = that.mError_;
			this->mType_ = that.mType_;
		}
		UniformLocationBucket(UniformLocationBucket&& that) noexcept {
			this->mLocation_ = that.mLocation_;
			this->mName_ = that.mName_;
			this->mError_ = that.mError_;
			this->mType_ = that.mType_;
		}

		UniformLocationBucket& operator=(const UniformLocationBucket& that) {
			if (this != &that) {
				this->mLocation_ = that.mLocation_;
				this->mName_ = that.mName_;
				this->mError_ = that.mError_;
				this->mType_ = that.mType_;
			}
			return *this;
		}

		UniformLocationBucket& operator=(UniformLocationBucket&& that) noexcept {
			if (this != &that) {
				this->mLocation_ = that.mLocation_;
				this->mName_ = that.mName_;
				this->mError_ = that.mError_;
				this->mType_ = that.mType_;
			}
			return *this;
		}

		void setType(UniformType type) {
			if (mType_ != UniformType::UNSPECIFIED) { //If a type has already been set for this uniform location
				fprintf(WRNLOG, "WARNING! Unable to set type for uniform %s.\nReason: This uniform already has a type!\n\n", mName_.c_str());
			}
			else {
				mType_ = type;
			}
		}

		bool error() const { return mError_; }
		GLint getLocation() const { return mLocation_; }
		const char * getName() const { return mName_.c_str(); }
		UniformType getType() const { return mType_; }

		bool checkForMatch(const char * uniformName) const {
			if (mName_.compare(uniformName) == 0) {
				return true;
			}
			return false;
		}

	private:
		std::string mName_;
		GLint mLocation_;
		bool mError_;
		UniformType mType_;
	};

} //namespace ShaderInterface

#endif //#ifndef UNIFORM_LOCATION_BUCKET