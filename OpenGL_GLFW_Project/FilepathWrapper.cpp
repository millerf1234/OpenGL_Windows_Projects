//See header file "FilepathWrapper.h" for details. Note that 
// the implementation for the static member function 'file_exists()'
// depends on some preprocessor statements in the header file.
//
// 
// Requires C++17 for std::filesystem functionality
//
//Date:       October 11, 2018
//            December 3, 2018   --  Added std::filesystem::last_write_time() checking 
//                                   allow app to see if new version of a file is available
//Programmer: Forrest Miller

#include "FilepathWrapper.h"

void FilepathWrapper::initialize() {
	mPath_ = "";
	mFileExists_ = false;
	mExtension_ = ""; //= "UNKNOWN";
	mExtensionExists_ = false;
	mLastWriteTime_ = std::nullopt;
}

FilepathWrapper::FilepathWrapper(const char * fp) {
	//Record filepath
	mPath_ = fp;
	//Check if file exists
	mFileExists_ = file_exists(mPath_.c_str());
	//Find and (if found) record file extension
	if (mFileExists_) {
		mExtension_ = findAndExtractFileExtension(mPath_);
		if (mExtension_.length() == 0u) {
			mExtensionExists_ = false;
		}
		//Attempt to record the time that the file was most recently updated
		std::filesystem::file_time_type timeOfMostRecentModification;
		if (getTimeOfFilesMostRecentUpdate(mPath_, timeOfMostRecentModification)) {
			mLastWriteTime_ = std::make_optional<std::filesystem::file_time_type>(timeOfMostRecentModification);
		}
		else {
			mLastWriteTime_ = std::nullopt;
		}
	}
	else {
		mExtensionExists_ = false;
		mExtension_ = "";
		mLastWriteTime_ = std::nullopt;
	}
}

FilepathWrapper::FilepathWrapper(const std::string& fp) {
	//Record filepath
	mPath_ = fp;
	//Check if file exists
	mFileExists_ = file_exists(mPath_.c_str());
	//Find and (if found) record file extension
	if (mFileExists_) {
		mExtension_ = findAndExtractFileExtension(mPath_);
		if (mExtension_.length() == 0u) {
			mExtensionExists_ = false;
		}
		//Attempt to record the time that the file was most recently updated
		std::filesystem::file_time_type timeOfMostRecentModification;
		if (getTimeOfFilesMostRecentUpdate(mPath_, timeOfMostRecentModification)) {
			mLastWriteTime_ = std::make_optional<std::filesystem::file_time_type>(timeOfMostRecentModification);
		}
		else {
			mLastWriteTime_ = std::nullopt;
		}
	}
	else {
		mExtensionExists_ = false;
		mExtension_ = "";
		mLastWriteTime_ = std::nullopt;
	}
}

FilepathWrapper::FilepathWrapper(const FilepathWrapper& that) {
	this->mPath_ = that.mPath_;
	this->mFileExists_ = that.mFileExists_;
	this->mExtension_ = that.mExtension_;
	this->mExtensionExists_ = that.mExtensionExists_;
	this->mLastWriteTime_ = that.mLastWriteTime_;
}

FilepathWrapper::FilepathWrapper(FilepathWrapper&& that) {
	this->mPath_ = std::move(that.mPath_); 
	this->mFileExists_ = that.mFileExists_;
	this->mExtension_ = std::move(that.mExtension_);
	this->mExtensionExists_ = that.mExtensionExists_;
	this->mLastWriteTime_ = that.mLastWriteTime_;
}

FilepathWrapper::~FilepathWrapper() {

}

FilepathWrapper& FilepathWrapper::operator=(const FilepathWrapper& that) {
	if (this != &that) {
		this->mPath_ = that.mPath_;
		this->mFileExists_ = that.mFileExists_;
		this->mExtension_ = that.mExtension_;
		this->mExtensionExists_ = that.mExtensionExists_;
		this->mLastWriteTime_ = that.mLastWriteTime_;
	}
	return *this;
}

FilepathWrapper& FilepathWrapper::operator=(FilepathWrapper&& that) {
	if (this != &that) {
		this->mPath_ = std::move(that.mPath_);
		this->mFileExists_ = that.mFileExists_;
		this->mExtension_ = std::move(that.mExtension_);
		this->mExtensionExists_ = that.mExtensionExists_;
		this->mLastWriteTime_ = that.mLastWriteTime_;
	}
	return *this;
}

std::string FilepathWrapper::extension() const {
	return mExtension_;
}

bool FilepathWrapper::hasUpdatedFileAvailable() noexcept {
	//If the file exists and a last-modified time for the file has been previously stored by this object
	if ( (!mFileExists_) || (!mLastWriteTime_) ) { 
		return false;
	}
	std::filesystem::file_time_type timeOfMostRecentModification;
	if (!getTimeOfFilesMostRecentUpdate(mPath_, timeOfMostRecentModification)) {
		return false;
	}
	else { //File exists and a valid most-recent-modification-time was able to be aquired through the OS
		if (timeOfMostRecentModification > mLastWriteTime_) { //See if retrieved time is newer than this object's stored time
			mLastWriteTime_ = std::make_optional<std::filesystem::file_time_type>(timeOfMostRecentModification);
			return true;
		}
		else {
			return false;
		}
	}
}


std::string FilepathWrapper::findAndExtractFileExtension(const std::string& fp) {
	int lastPeriodIndex = getIndexOfLastPeriodInString(fp);
	if (lastPeriodIndex == NOT_FOUND) {
		return "";
	}
	else {
		std::string debug = fp.substr(lastPeriodIndex);
		return fp.substr(lastPeriodIndex); 
	}
}

bool FilepathWrapper::getTimeOfFilesMostRecentUpdate(const std::string& fp, std::filesystem::file_time_type& lastUpdateTime) {
	if (!file_exists(fp.c_str())) {
		return false;
	}
	std::error_code errorCodeFromOS;
	lastUpdateTime = std::filesystem::last_write_time(fp, errorCodeFromOS);
	if (!errorCodeFromOS) { //SUCCESS!
		return true;
	}
	else {  //The OS reported an error when trying to retrieve the time of this files' most recent modification
		fprintf(stderr, "\nERROR!\nAn Error occured in the OS while trying to retrieve the most\n"
			"recent time the file %s\n"
			"was updated!\nError Message: %s\n", fp.c_str(), errorCodeFromOS.message().c_str());
		return false;
	}
}

bool FilepathWrapper::file_exists(const char * fp) {
	return std::filesystem::exists(fp);
}


int FilepathWrapper::getIndexOfLastPeriodInString(const std::string& fp) {
	std::string::const_iterator lastPeriod;
	lastPeriod = std::find(fp.begin(), fp.end(), '.');
	if (lastPeriod == fp.end()) {
		return NOT_FOUND;
	}
	else { //else we have found the period
		return std::distance(fp.begin(), lastPeriod);
	}
}
