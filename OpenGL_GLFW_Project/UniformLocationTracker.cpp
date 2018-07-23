//Created by Forrest Miller on July 20, 2018

// Implementation Details:
//The uniform locations are each tracked with a wrapper struct called UniformLocationBucket.
//Each UniformLocationTracker will have a vector containing lists of UniformLocationBuckets, with 
//the vector initially containing no lists. Each time an update is requested for a uniform location,
//the vector is checked to see if it contains any already-tracked uniform locations of that type*. 
//If there are no elements of that type, then a new list for that type is created into the vector.
//If there are already elements of that type, then the list is searched for a match. 
//If no match is found, then a new bucket is appended to the end of the list. 
//
// Doing this makes finding already tracked uniforms quick to reconnect with (though in retrospect a hash table would be faster)
//
// *Instead of searching through the entire vector, each list type has a corresponding variable that is associated with
//  the list's position in the vector. 


#include "UniformLocationTracker.h"

static const uint8_t NOT_IN_VECTOR = 255u;
static const GLint NOT_IN_LIST = -2;
static constexpr size_t CACHED_UNIFORM_LOCATION_HASH_SIZE = 107; //Should be prime number?


UniformLocationTracker::UniformLocationTracker(const GLuint& programID) {
	mProgramID = programID;
	mTrackedLocationsCount = mCachedLocationsCount = 0u;
	
	mCachedUniformLocations.rehash(CACHED_UNIFORM_LOCATION_HASH_SIZE);

	initializeListLocations();
}

UniformLocationTracker::~UniformLocationTracker() {

}



//Update uniform  //see: https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glUniform.xhtml
void UniformLocationTracker::updateUniform1i(const GLchar * uniformName, const GLint x) {
	glUniform1i(lookupUniformLocation(i1_, uniformName, UniformType::INT1), x);
}
void UniformLocationTracker::updateUniform1u(const GLchar * uniformName, const GLuint x) {
	glUniform1ui(lookupUniformLocation(u1_, uniformName, UniformType::UINT1), x);
}
void UniformLocationTracker::updateUniform1f(const GLchar * uniformName, const GLfloat x) {
	glUniform1f(lookupUniformLocation(f1_, uniformName, UniformType::FLOAT1), x);
}
void UniformLocationTracker::updateUniform1iv(const GLchar * uniformName, const GLint * valPtr, GLsizei count) {
	glUniform1iv(lookupUniformLocation(iv1_, uniformName, UniformType::INT1V), count, valPtr);
}
void UniformLocationTracker::updateUniform1uv(const GLchar * uniformName, const GLuint * valPtr, GLsizei count) {
	glUniform1uiv(lookupUniformLocation(uv1_, uniformName, UniformType::UINT1V), count, valPtr);
}
void UniformLocationTracker::updateUniform1fv(const GLchar * uniformName, const GLfloat * valPtr, GLsizei count) {
	glUniform1fv(lookupUniformLocation(fv1_, uniformName, UniformType::FLOAT1V), count, valPtr);
}


void UniformLocationTracker::updateUniform2i(const GLchar * uniformName, const GLint x, const GLint y) {
	glUniform2i(lookupUniformLocation(i2_, uniformName, UniformType::INT2), x, y);
}
void UniformLocationTracker::updateUniform2u(const GLchar * uniformName, const GLuint x, const GLuint y) {
	glUniform2ui(lookupUniformLocation(u2_, uniformName, UniformType::UINT2), x, y);
}
void UniformLocationTracker::updateUniform2f(const GLchar * uniformName, const GLfloat x , const GLfloat y) {
	glUniform2f(lookupUniformLocation(f2_, uniformName, UniformType::FLOAT2), x, y);
}
void UniformLocationTracker::updateUniform2iv(const GLchar * uniformName, const GLint * valPtr, GLsizei count) {
	glUniform2iv(lookupUniformLocation(iv2_, uniformName, UniformType::INT2V), count, valPtr);
}
void UniformLocationTracker::updateUniform2uv(const GLchar * uniformName, const GLuint * valPtr, GLsizei count) {
	glUniform2uiv(lookupUniformLocation(uv2_, uniformName, UniformType::UINT2V), count, valPtr);
}
void UniformLocationTracker::updateUniform2fv(const GLchar * uniformName, const GLfloat * valPtr, GLsizei count) {
	glUniform2fv(lookupUniformLocation(fv2_, uniformName, UniformType::FLOAT2V), count, valPtr);
}


void UniformLocationTracker::updateUniform3i(const GLchar * uniformName, const GLint x, const GLint y, const GLint z) {
	glUniform3i(lookupUniformLocation(i3_, uniformName, UniformType::INT3), x, y, z);
}
void UniformLocationTracker::updateUniform3u(const GLchar * uniformName, const GLuint x, const GLuint y, const GLuint z) {
	glUniform3ui(lookupUniformLocation(u3_, uniformName, UniformType::UINT3), x, y, z);
}
void UniformLocationTracker::updateUniform3f(const GLchar * uniformName, const GLfloat x, const GLfloat y, const GLfloat z) {
	glUniform3f(lookupUniformLocation(f3_, uniformName, UniformType::FLOAT3), x, y, z);
}
void UniformLocationTracker::updateUniform3iv(const GLchar * uniformName, const GLint * valPtr, GLsizei count) {
	glUniform3iv(lookupUniformLocation(iv3_, uniformName, UniformType::INT3V), count, valPtr);
}
void UniformLocationTracker::updateUniform3uv(const GLchar * uniformName, const GLuint * valPtr, GLsizei count) {
	glUniform3uiv(lookupUniformLocation(uv3_, uniformName, UniformType::UINT3V), count, valPtr);
}
void UniformLocationTracker::updateUniform3fv(const GLchar * uniformName, const GLfloat * valPtr, GLsizei count) {
	glUniform3fv(lookupUniformLocation(fv3_, uniformName, UniformType::FLOAT3V), count, valPtr);
}


void UniformLocationTracker::updateUniform4i(const GLchar * uniformName, const GLint x, const GLint y, const GLint z, const GLint w) {
	glUniform4i(lookupUniformLocation(i4_, uniformName, UniformType::INT4), x, y, z, w);
}
void UniformLocationTracker::updateUniform4u(const GLchar * uniformName, const GLuint x, const GLuint y, const GLuint z, const GLuint w) {
	glUniform4ui(lookupUniformLocation(u4_, uniformName, UniformType::UINT4), x, y, z, w);
}
void UniformLocationTracker::updateUniform4f(const GLchar * uniformName, const GLfloat x, const GLfloat y, const GLfloat z, const GLfloat w) {
	glUniform4f (lookupUniformLocation(f4_, uniformName, UniformType::FLOAT4), x, y, z, w);
}
void UniformLocationTracker::updateUniform4iv(const GLchar * uniformName, const GLint * valPtr, GLsizei count) {
	glUniform4iv(lookupUniformLocation(iv4_, uniformName, UniformType::INT4V), count, valPtr);
}
void UniformLocationTracker::updateUniform4uv(const GLchar * uniformName, const GLuint * valPtr , GLsizei count) {
	glUniform4uiv(lookupUniformLocation(uv4_, uniformName, UniformType::UINT4V), count, valPtr);
}
void UniformLocationTracker::updateUniform4fv(const GLchar * uniformName, const GLfloat * valPtr, GLsizei count) {
	glUniform4fv(lookupUniformLocation(fv4_, uniformName, UniformType::FLOAT4), count, valPtr);
}



void UniformLocationTracker::updateUniformMat2x2(const GLchar * uniformName, const GLfloat * valPtr, GLsizei count, GLboolean transpose) {
	glUniformMatrix2fv(lookupUniformLocation(M2x2_, uniformName, UniformType::MAT2), count, transpose, valPtr);
}
void UniformLocationTracker::updateUniformMat2x3(const GLchar * uniformName, const GLfloat * valPtr, GLsizei count, GLboolean transpose) {
	glUniformMatrix2x3fv(lookupUniformLocation(M2x3_, uniformName, UniformType::MAT2X3), count, transpose, valPtr);
}
void UniformLocationTracker::updateUniformMat2x4(const GLchar * uniformName, const GLfloat * valPtr, GLsizei count, GLboolean transpose) {
	glUniformMatrix2x4fv(lookupUniformLocation(M2x4_, uniformName, UniformType::MAT2X4), count, transpose, valPtr);
}

void UniformLocationTracker::updateUniformMat3x2(const GLchar * uniformName, const GLfloat * valPtr, GLsizei count, GLboolean transpose) {
	glUniformMatrix3x2fv(lookupUniformLocation(M3x2_, uniformName, UniformType::MAT3X2), count, transpose, valPtr);
}
void UniformLocationTracker::updateUniformMat3x3(const GLchar * uniformName, const GLfloat * valPtr, GLsizei count, GLboolean transpose) {
	glUniformMatrix3fv(lookupUniformLocation(M3x3_, uniformName, UniformType::MAT3), count, transpose, valPtr);
}
void UniformLocationTracker::updateUniformMat3x4(const GLchar * uniformName, const GLfloat * valPtr, GLsizei count, GLboolean transpose) {
	glUniformMatrix3x4fv(lookupUniformLocation(M3x4_, uniformName, UniformType::MAT3X4), count, transpose, valPtr);
}

void UniformLocationTracker::updateUniformMat4x2(const GLchar * uniformName, const GLfloat * valPtr, GLsizei count, GLboolean transpose) {
	glUniformMatrix4x2fv(lookupUniformLocation(M4x2_, uniformName, UniformType::MAT4X2), count, transpose, valPtr);
}
void UniformLocationTracker::updateUniformMat4x3(const GLchar * uniformName, const GLfloat * valPtr, GLsizei count, GLboolean transpose) {
	glUniformMatrix4x3fv(lookupUniformLocation(M4x3_, uniformName, UniformType::MAT4X3), count, transpose, valPtr);
}
void UniformLocationTracker::updateUniformMat4x4(const GLchar * uniformName, const GLfloat * valPtr, GLsizei count, GLboolean transpose) {
	glUniformMatrix4fv(lookupUniformLocation(M4x4_, uniformName, UniformType::MAT4), count, transpose, valPtr); 
}




std::shared_ptr<CachedUniformLocation> UniformLocationTracker::getCachedUniform1i(const GLchar * uniformName) {
	if (seeIfUniformLocationHasAlreadyBeenCached(uniformName))
		return mCachedUniformLocations[uniformName];
	else {
		return addCachedUniformLocation(uniformName, mProgramID, UniformType::INT1); //const GLchar * name, GLuint programID, UniformType type, GLsizei count, GLboolean transpose
	}
}
std::shared_ptr<CachedUniformLocation> UniformLocationTracker::getCachedUniform1u(const GLchar * uniformName) {

}
std::shared_ptr<CachedUniformLocation> UniformLocationTracker::getCachedUniform1f(const GLchar * uniformName) {

}
std::shared_ptr<CachedUniformLocation> UniformLocationTracker::getCachedUniform1iv(const GLchar * uniformName, GLsizei count) {

}
std::shared_ptr<CachedUniformLocation> UniformLocationTracker::getCachedUniform1uv(const GLchar * uniformName, GLsizei count) {

}
std::shared_ptr<CachedUniformLocation> UniformLocationTracker::getCachedUniform1fv(const GLchar * uniformName, GLsizei count) {

}


std::shared_ptr<CachedUniformLocation> UniformLocationTracker::getCachedUniform2i(const GLchar * uniformName) {

}
std::shared_ptr<CachedUniformLocation> UniformLocationTracker::getCachedUniform2u(const GLchar * uniformName) {

}
std::shared_ptr<CachedUniformLocation> UniformLocationTracker::getCachedUniform2f(const GLchar * uniformName) {

}
std::shared_ptr<CachedUniformLocation> UniformLocationTracker::getCachedUniform2iv(const GLchar * uniformName, GLsizei count) {

}
std::shared_ptr<CachedUniformLocation> UniformLocationTracker::getCachedUniform2uv(const GLchar * uniformName, GLsizei count) {

}
std::shared_ptr<CachedUniformLocation> UniformLocationTracker::getCachedUniform2fv(const GLchar * uniformName, GLsizei count) {

}


std::shared_ptr<CachedUniformLocation> UniformLocationTracker::getCachedUniform3i(const GLchar * uniformName) {

}
std::shared_ptr<CachedUniformLocation> UniformLocationTracker::getCachedUniform3u(const GLchar * uniformName) {

}
std::shared_ptr<CachedUniformLocation> UniformLocationTracker::getCachedUniform3f(const GLchar * uniformName) {

}
std::shared_ptr<CachedUniformLocation> UniformLocationTracker::getCachedUniform3iv(const GLchar * uniformName, GLsizei count) {

}
std::shared_ptr<CachedUniformLocation> UniformLocationTracker::getCachedUniform3uv(const GLchar * uniformName, GLsizei count) {

}
std::shared_ptr<CachedUniformLocation> UniformLocationTracker::getCachedUniform3fv(const GLchar * uniformName, GLsizei count) {

}


std::shared_ptr<CachedUniformLocation> UniformLocationTracker::getCachedUniform4i(const GLchar * uniformName) {

}
std::shared_ptr<CachedUniformLocation> UniformLocationTracker::getCachedUniform4u(const GLchar * uniformName) {

}
std::shared_ptr<CachedUniformLocation> UniformLocationTracker::getCachedUniform4f(const GLchar * uniformName) {

}
std::shared_ptr<CachedUniformLocation> UniformLocationTracker::getCachedUniform4iv(const GLchar * uniformName, GLsizei count) {

}
std::shared_ptr<CachedUniformLocation> UniformLocationTracker::getCachedUniform4uv(const GLchar * uniformName, GLsizei count) {

}
std::shared_ptr<CachedUniformLocation> UniformLocationTracker::getCachedUniform4fv(const GLchar * uniformName, GLsizei count) {

}


std::shared_ptr<CachedUniformLocation> UniformLocationTracker::getCachedUniformMat2x2(const GLchar * uniformName, GLsizei count, GLboolean transpose) {

}
std::shared_ptr<CachedUniformLocation> UniformLocationTracker::getCachedUniformMat2x3(const GLchar * uniformName, GLsizei count, GLboolean transpose) {

}
std::shared_ptr<CachedUniformLocation> UniformLocationTracker::getCachedUniformMat2x4(const GLchar * uniformName, GLsizei count, GLboolean transpose) {

}


std::shared_ptr<CachedUniformLocation> UniformLocationTracker::getCachedUniformMat3x2(const GLchar * uniformName, GLsizei count, GLboolean transpose) {

}
std::shared_ptr<CachedUniformLocation> UniformLocationTracker::getCachedUniformMat3x3(const GLchar * uniformName, GLsizei count, GLboolean transpose) {

}
std::shared_ptr<CachedUniformLocation> UniformLocationTracker::getCachedUniformMat3x4(const GLchar * uniformName, GLsizei count, GLboolean transpose) {

}


std::shared_ptr<CachedUniformLocation> UniformLocationTracker::getCachedUniformMat4x2(const GLchar * uniformName, GLsizei count, GLboolean transpose) {

}
std::shared_ptr<CachedUniformLocation> UniformLocationTracker::getCachedUniformMat4x3(const GLchar * uniformName, GLsizei count, GLboolean transpose) {
	 
}
std::shared_ptr<CachedUniformLocation> UniformLocationTracker::getCachedUniformMat4x4(const GLchar * uniformName, GLsizei count, GLboolean transpose) {

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
	M2x2_ = M2x3_ = M2x4_ = M3x2_ = M3x3_ = M3x4_ = M4x2_ = M4x3_ = M4x4_ = NOT_IN_VECTOR;

	mNextListIndex = 0u; //Initialize to the index that will be assigned to the first element emplaced into the vector
}


//Note-to-self: I can probably remove the 'UniformType' enum completetly since ULBuckets will be constructed in the correct place automatically. 
//Updated Note-To-self: Don't remove UniformType becuase I now use it for CachedUniformLocations as well...     I can probably delete both of these messages
inline GLint UniformLocationTracker::lookupUniformLocation(uint8_t& listIndex, const char * name, UniformType ut) {
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

GLint UniformLocationTracker::findInList(uint8_t listIndex, const char * name) { //Returns a uniform location or -2 if not found in list  (-1 already has meaning within OpenGL)
	//Make sure that listIndex is valid (can remove this check once I am sure code is operating correctly
	if (static_cast<size_t>(listIndex) > (mUniformLocationLists.size() - 1ull)) {
		fprintf(WRNLOG, "\nWARNING! In UniformLocationTracker within the function call 'findInList',\n");
		fprintf(WRNLOG, "         a list index of %u was requested but only %zu lists are in the vector!\n", listIndex, mUniformLocationLists.size());
		fprintf(WRNLOG, "         Clearly this should not happen...Check Ya Code!\n");
		throw std::out_of_range("The list to search in the function call findInList within a UniformLocationTracker object has index outside the range of tracked lists!\n");
	}

	if (listIndex == NOT_IN_VECTOR) {
		throw std::out_of_range("OOps! It looks like the list index was never set when the function findInList in the class UniformLocationTracker was called!\n");
	}

	std::list<UniformLocationBucket>::iterator iter = mUniformLocationLists[listIndex].begin();
	for ( ; iter != mUniformLocationLists[listIndex].end(); iter++) {
		if ((*iter).checkForMatch(name)) {
			return (*iter).getLocation();
		}
	}

	return NOT_IN_LIST;
}





bool UniformLocationTracker::seeIfUniformLocationHasAlreadyBeenCached(const GLchar * uniformName) {
	std::shared_ptr<CachedUniformLocation> cachedLocation = mCachedUniformLocations[uniformName];
	//Important Note: This function's implementation relies on the behavior of 'operator[]' of an unordered_map. When using
	//'operator[]' on an unordered_map, the map is searched using the provided key. If an element exists at that key, then
	//the unordered_map returns a reference to that element. However, if an element is not found associated with the key, the 
	//unordered_map creates a new element of that type using the elements default constructor (in this case shared_ptr's default
	//constructor is called, which in turn calls the default CachedUniformLocation constructor). Thus in the following code,
	//to tell if a CachedUniformLocation already exists, it suffices to just see if the object has been properly initialized.

	if (cachedLocation->mWasInitialized) { //I am able to access the CachedUniformLocation's private parts directly because this class is friends with the CachedUniformLocation class.
		return true;
	}
	return false;
}

std::shared_ptr<CachedUniformLocation> UniformLocationTracker::addCachedUniformLocation(const GLchar * uniformName, UniformType type, GLsizei count, GLboolean transpose) {
	std::shared_ptr<CachedUniformLocation> cachedLocation = mCachedUniformLocations[uniformName];
	if (cachedLocation->mWasInitialized) {
		if (cachedLocation->mUniformType == UniformType::UNSPECIFIED) {
			throw std::logic_error("Error! For some reason a cachedUniformLocation was initialized while its type was still set to unspecified!\n");
		}
		else {
			fprintf(WRNLOG, "\nWARNING! An initialized CachedUniformLocation for the variable %s\nhas already been initialized, but is now getting re-initialized!\n", cachedLocation->mUniformName);
		}
	}
	cachedLocation.reset(new CachedUniformLocation(uniformName, mProgramID, type, count, transpose)); //Re-initialize with the proper data
	return cachedLocation;
}

