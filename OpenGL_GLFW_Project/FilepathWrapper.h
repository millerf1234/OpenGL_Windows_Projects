//UNFINISHED! I never finished this. See FilepathHelperFunctions(.h/.cpp)
#if defined IS_FINISHED

// File:           FilepathWrapper.h
// Class:          FilepathWrapper
// Namespace:      N/A
// Date:           October 11, 2018    
// Programmer:     Forrest Miller
//
// Description:    This class wraps a filepath to keep track of the paths 
//                 state and provide a basic interface.
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

class Filepath {
public:
	Filepath() = delete;
	Filepath(const char * fp);
	Filepath(std::string fp);
	~Filepath();

	//Checks to see if the file at the filepath exists
	bool exists() const { return mExists_; }

	//Returns the extension on the filepath. The extension is defined as 
	//all of the characters occuring after the last '.' character in the 
	//string, not including the '.' itself. If no '.' character is present,
	//an empty string will be returned
	std::string extension() const;

private:
	std::string mPath_;
	bool mExists_;
	const char * mExtension_;

	//Helper functions
	void findExtension();

	//Returns -1 if no period is found in the path
	int getIndexOfLastPeriodInPath()const ;
};


#endif //FILEPATH_H_


#endif //defined IS_FINISHED