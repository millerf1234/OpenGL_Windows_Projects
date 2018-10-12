//See header file "FilepathWrapper.h" for details. Note that 
// the implementation for the static member function 'file_exists()'
// depends on some preprocessor statements in the header file.
//
// 
// The class currently targets C++17, but if/when C++20 comes out and
// std::filesystem becomes standardized, this code can/should be updated 
// to use 'std::filesystem' instead of 'experimental::filesystem'.
//
//Date:       October 11, 2018
//Programmer: Forrest Miller

#include "FilepathWrapper.h"

void Filepath::initialize() {
	mPath_ = "";
	mFileExists_ = false;
	mExtension_ = ""; //= "UNKNOWN";
	mExtensionExists_ = false;
}

Filepath::Filepath(const char * fp) {
	//Record filepath
	mPath_ = fp;
	//Check if file exists
	mFileExists_ = file_exists(mPath_.c_str());
	//Find and (if found) record file extension
	if ( mFileExists_ ) 
		findAndExtractFileExtension(mPath_);
}

Filepath::Filepath(std::string fp) {
	//Record filepath
	mPath_ = fp;
	//Check if file exists
	mFileExists_ = file_exists(mPath_.c_str());
	//Find and (if found) record file extension
	if ( mFileExists_ ) 
		findAndExtractFileExtension(mPath_);
}

Filepath::Filepath(const Filepath& that) {
	this->mPath_ = that.mPath_;
	this->mFileExists_ = that.mFileExists_;
	this->mExtension_ = that.mExtension_;
	this->mExtensionExists_ = that.mExtensionExists_;
}

Filepath::Filepath(Filepath&& that) {
	//One of these next 2 may be slightly faster. Will have to test each one day...
	this->mPath_ = std::move(that.mPath_); 
	//this->mPath_.swap(that.mPath_);

	this->mFileExists_ = that.mFileExists_;

	//Same deal as before, these next 2 lines do the same thing but one may execute faster
	this->mExtension_ = std::move(that.mExtension_);
	//this->mExtension_.swap(that.mPath_);
	
	this->mExtensionExists_ = that.mExtensionExists_;
}

Filepath::~Filepath() {

}

Filepath& Filepath::operator=(const Filepath& that) {
	if (this != &that) {
		this->mPath_ = that.mPath_;
		this->mFileExists_ = that.mFileExists_;
		this->mExtension_ = that.mExtension_;
		this->mExtensionExists_ = that.mExtensionExists_;
	}
	return *this;
}

Filepath& Filepath::operator=(Filepath&& that) {
	if (this != &that) {
		//Choose one of the following 2 lines:
		//this->mPath_ = std::move(that.mPath_);
		this->mPath_.swap(that.mPath_);

		this->mFileExists_ = that.mFileExists_;

		//Chose one of the following 2 lines:
		//this->mExtension_ = std::move(that.mExtension_);
		this->mExtension_.swap(that.mExtension_);

		this->mExtensionExists_ = that.mExtensionExists_;
	}
	return *this;
}


std::string Filepath::extension() const {
	return mExtension_;
}

std::string Filepath::findAndExtractFileExtension(const std::string& fp) {
	if (fp.length() < 2) {
		return "";
	}
	
	int lastPeriodIndex = getIndexOfLastPeriodInString(fp);
	if (lastPeriodIndex == NOT_FOUND) {
		return "";
	}
	else {
		std::string debug = fp.substr(lastPeriodIndex);
		fprintf(stdout, "\n\nExtension is: %s\n\n", debug.c_str());
		return fp.substr(lastPeriodIndex); 
	}
}

//The implementation of this function depends of if 'filesystem' is available. 
//See header file for details. 'filesystem' is expected to become standard in C++20. 
//'filesystem' has been part of std::experimental since C++14.
//
// Implementation Note: My implementation for both of these functions was directly inspired (copied)
// from the answers on this stack exchange thread: https://stackoverflow.com/questions/12774207/fastest-way-to-check-if-a-file-exist-using-standard-c-c11-c
bool Filepath::file_exists(const char * fp) {
#ifdef CHECK_USING_EXPERIMENTAL_FILESYSTEM_FUNCTION
	//As of C++14:
	return std::experimental::filesystem::exists(fp);
#elif defined CHECK_USING_OS_DEPENDENT_IMPLEMENTATION
#ifdef _WIN32
#include <io.h> 
#define access    _access_s
#else
#include <unistd.h>
#endif //_WIN32	
	return ( access(fp.c_str(), 0) == 0 );
#ifdef _WIN32
#undef access
#endif //_WIN32
#endif //CHECK_USING_EXPERIMENTAL_FILESYSTEM_FUNCTION
}

int Filepath::getIndexOfLastPeriodInString(const std::string& fp) {
	std::string::const_iterator lastPeriod;
	lastPeriod = std::find(fp.begin(), fp.end(), '.');
	if (lastPeriod == fp.end()) {
		return NOT_FOUND;
	}
	else { //else we have determined the string contains at least one period
		int periodIndex; 
		do { 
			periodIndex = std::distance(fp.begin(), lastPeriod);
			lastPeriod = std::find(++lastPeriod, fp.end(), '.');
		} while ( lastPeriod != fp.end() );
		return periodIndex;
	}
}
