//See Header file for detail. This class works within the UniformLocationBucket-UniformLocationTracker system
//Created by Forrest Miller on July 23, 2018

#include "CachedUniformLocation.h"


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


}
