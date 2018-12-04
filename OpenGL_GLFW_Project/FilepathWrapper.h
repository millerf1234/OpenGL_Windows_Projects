// File:           FilepathWrapper.h
// Class:          FilepathWrapper
// Namespace:      N/A
// Date:           October 11, 2018    
// Programmer:     Forrest Miller
//
// Description:    This class wraps a filepath to keep track of a FilePaths 
//                 state (file exists, file extension) and provides a basic interface.
//				   
// NOTES:          I am adding this class after I have already created several
//				   other classes that handle managing filepaths on their own. 
//				   I am not sure yet if I will get around to changing their
//                 implementations to use this class.
//
//
//
// Windows Filesystem/SystemClock Note: 
//              I copied the following post by Billy Robert O'Neal III found at
//   https://developercommunity.visualstudio.com/content/problem/251213/stdfilesystemfile-time-type-does-not-allow-easy-co.html
//
//                   "system_clock / time_t are inappropriate for storing file times, as they
//                    can represent neither the range, nor the resolution, of timestamps you
//                    can get from the file system. Windows stores file times using the FILETIME
//                    structure, which is a 64 bit value of 100ns intervals from January 1, 1601.
//                    What's worse is that this 1601 date is fairly common to see given that it's
//                    the all zeroes value, and it is far before the earliest date representable with
//                    time_t. std::filesystem can't change the reality of the underlying platform. On
//                    POSIX systems, time_t is similarly insufficient, as their filesystem APIs return
//                    struct timespec instances which can represent nanos (though at least the seconds
//                    part of the timespec can be used as a time_t discarding some information)."
//
//                    Moral of the story:  Until C++20, it is not possible (or advisable) to convert between
//                                         std::filesystem::file_time_type and time_t. 
//                    See also: https://en.cppreference.com/w/cpp/filesystem/file_time_type
//


#pragma once

#ifndef FILEPATH_WRAPPER_H_
#define FILEPATH_WRAPPER_H_

#include <fstream>
#include <sstream>
#include <algorithm>
#include <chrono>     //for tracking  'last_write_time'
#include <filesystem> 
#include <optional>  //To pretect against uninitialized FileWriteTime



static inline constexpr int NOT_FOUND = -1;

class FilepathWrapper {
public:
	FilepathWrapper() = delete;
	FilepathWrapper(const char * fp);
	FilepathWrapper(const std::string& fp);

	FilepathWrapper(const FilepathWrapper&);
	FilepathWrapper(FilepathWrapper&&);
	~FilepathWrapper();

	FilepathWrapper& operator=(const FilepathWrapper&);
	FilepathWrapper& operator=(FilepathWrapper&&);

	//Returns the filepath as a std::string
	std::string filepath() const { return mPath_; }

	//Checks to see if the file at the filepath exists
	bool exists() const { return mFileExists_; }

	//Checks to see if a file extension was detected in the filepath
	bool extensionExists() const { return mExtensionExists_; }

	//Returns the extension of the stored filepath. The extension is defined as 
	//all of the characters occuring after the last '.' character in the 
	//string, not including the '.' itself. If no '.' character is present,
	//an empty string will be returned
	std::string extension() const;

	//Querries this object to determine if it was able to record the time of last modification
	//to its wrapped file. This is necessary because it is not guarenteed that all operating systems
	//will play nicely with having the age of all of their files querried and may fail to return a valid
	//value. The function hasUpdatedFileAvailable() will always return false if this function returns false.
	inline bool isAbleToDiscoverUpdatesToFile() const { return bool(mLastWriteTime_); } //Explicitly call operator bool on std::optional mLastWriteTime_

	//Attempts to querry the file located at this object's stored filepath to see if there 
	//is an updated version of the file available. Will only return true if a newer version
	//of the file is detected, otherwise will always return false. This function will update 
	//this object's local copy of the file's most recent time of modification, so calling this
	//function more than once will result in it returning false for all calls past the first call
	//until the file gets modified again.
	bool hasUpdatedFileAvailable() noexcept;

	/////////////////////////////////
	//   Static Utility Functions  //
	/////////////////////////////////

	//Returns empty string if no file extension is found
	static std::string findAndExtractFileExtension(const std::string& fp);

	//Checks with the operating system to determine the system time when a file was most 
	//recently updated. Will print out an error message if the OS encounters a problem 
	//querring the file and return false, in which case DO NOT USE THE file_time_type variable!
	static bool getTimeOfFilesMostRecentUpdate(const std::string& fp, std::filesystem::file_time_type& lastUpdateTime);

	//Checks to see if the file string corresponds to an actual file on the system
	static bool file_exists(const char * fp);
	
	

private:
	std::string mPath_;
	bool mFileExists_;
	std::string mExtension_; //The ".xxxx" at the end of a filepath (does not necessarily exist for all filepaths)
	bool mExtensionExists_;  

	//bool mHasLastWriteTimeAvailableForComparison_; //boolean flag to ensure that mLastWriteTime_ has an accurate value.
	std::optional<std::filesystem::file_time_type> mLastWriteTime_; //Records the system time when the file was last updated. Can be used to 
	//                                               //tell if a file has an updated version compared with the currently loaded version.


	/////////////////////////////////
	//   Private Helper Functions  //
	/////////////////////////////////
	void initialize();
	

	//Returns -1 if no period is found in the path
	static int getIndexOfLastPeriodInString(const std::string& );

};


#endif //FILEPATH_WRAPPER_H_


