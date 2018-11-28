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

#pragma once

#ifndef FILEPATH_H_
#define FILEPATH_H_

#include <fstream>
#include <sstream>
#include <algorithm>

//For pre-CPP17 compilers, give the macro __has_include a definition if none currently exists
#ifndef __has_include
#define __has_include(x) false
#endif


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//A new addition to C++17 is the Pre-Processor extension '__has_include()'
//for checking if a header file is available. see:
//           https://stackoverflow.com/questions/142877/can-the-c-preprocessor-be-used-to-tell-if-a-file-exists
//Since this code is in a project that is targeting C++17, it will use this function.
#if __has_include(<experimental/filesystem>)
#define CHECK_USING_EXPERIMENTAL_FILESYSTEM_FUNCTION
#include <experimental/filesystem> 
#else 
#define CHECK_USING_OS_DEPENDENT_IMPLEMENTATION
#endif //__has_include(<experimental/filesystem>)
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

static inline constexpr int NOT_FOUND = -1;

class Filepath {
public:
	Filepath() = delete;
	Filepath(const char * fp);
	Filepath(std::string fp);

	Filepath(const Filepath&);
	Filepath(Filepath&&);
	~Filepath();

	Filepath& operator=(const Filepath&);
	Filepath& operator=(Filepath&&);

	//Checks to see if the file at the filepath exists
	bool exists() const { return mFileExists_; }

	//Checks to see if a file extension was detected in the filepath
	bool extensionExists() const { return mExtensionExists_; }

	//Returns the extension of the stored filepath. The extension is defined as 
	//all of the characters occuring after the last '.' character in the 
	//string, not including the '.' itself. If no '.' character is present,
	//an empty string will be returned
	std::string extension() const;


	/////////////////////////////////
	//   Static Utility Functions  //
	/////////////////////////////////

	//Returns empty string if no file extension is found
	static std::string findAndExtractFileExtension(const std::string& fp);

	//Checks to see if the file string corresponds to an actual file on the system
	static bool file_exists(const char * fp);

private:
	std::string mPath_;
	bool mFileExists_;
	std::string mExtension_; //The ".xxxx" at the end of a filepath (does not necessarily exist for all filepaths)
	bool mExtensionExists_;  


	/////////////////////////////////
	//   Private Helper Functions  //
	/////////////////////////////////
	void initialize();
	

	//Returns -1 if no period is found in the path
	static int getIndexOfLastPeriodInString(const std::string& );
};


#endif //FILEPATH_H_


//#endif //defined IS_FINISHED