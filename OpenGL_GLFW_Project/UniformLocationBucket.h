//A simple structure for tracking uniform locations. Copying is allowed
//Note that these buckets just hold a number representing a uniform's location and don't store any information on the uniforms value(s)
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
#endif //#ifndef WRNLOG

#include <string>

#include "ProjectConstants.h"
#include "ProjectParameters.h"

enum class UniformType {INT1, UINT1, FLOAT1, INT1V, UINT1V, FLOAT1V,
						INT2, UINT2, FLOAT2, INT2V, UINT2V, FLOAT2V,
						INT3, UINT3, FLOAT3, INT3V, UINT3V, FLOAT3V,
						INT4, UINT4, FLOAT4, INT4V, UINT4V, FLOAT4V,
						MAT2, MAT3, MAT4, MAT2X3, MAT3X2, MAT2X4, MAT4X2, MAT3X4, MAT4X3,   //All matrices are of fv (float vector) type
						UNSPECIFIED};

typedef struct UniformLocationBucket {
	//Constructor
	UniformLocationBucket() = delete; 
	UniformLocationBucket(GLuint programID, const char * str, UniformType type = UniformType::UNSPECIFIED) {
		mType = type;
		name = std::move(std::string(str));
		location = glGetUniformLocation(programID, str);  //Crashes here if programID is invalid...
		if (location = -1) {
			fprintf(WRNLOG, "\nWarning! Unable to get uniform location for uniform: %s\n", str);
			fprintf(WRNLOG, "This is expected behavior if uniform is unused within the shader\n");
			mError = true;
		}
		else {
			mError = false;
		}
	}
	UniformLocationBucket(const UniformLocationBucket& that) {
		this->location = that.location;
		this->name = that.name;
		this->mError = that.mError;
		this->mType = that.mType;
	}
	UniformLocationBucket(UniformLocationBucket&& that) {
		this->location = that.location;
		this->name = that.name;
		this->mError = that.mError;
		this->mType = that.mType;
	}

	UniformLocationBucket& operator=(const UniformLocationBucket& that) {
		if (this != &that) {
			this->location = that.location;
			this->name = that.name;
			this->mError = that.mError;
			this->mType = that.mType;
		}
		return *this;
	}

	UniformLocationBucket& operator=(UniformLocationBucket&& that) {
		this->location = that.location;
		this->name = that.name;
		this->mError = that.mError;
		this->mType = that.mType;
		return *this;
	}

	void setType(UniformType type) {
		if (mType != UniformType::UNSPECIFIED) { //If a type has already been set for this uniform location
			fprintf(WRNLOG, "WARNING! Unable to set type for uniform %s.\nReason: This uniform already has a type!\n\n", name.c_str());
		}
		else {
			mType = type;
		}
	}


	bool error() const { return mError; }
	GLint getLocation() const { return location; }
	const char * getName() const { return name.c_str(); }
	UniformType getType() const { return mType; }

	bool checkForMatch(const char * uniformName) const {
		if (name.compare(uniformName) == 0) {
			return true;
		}
		return false;
	}

private:
	std::string name;
	GLint location;
	bool mError;
	UniformType mType;
} UniformLocationBucket;


#endif //#ifndef UNIFORM_LOCATION_BUCKET