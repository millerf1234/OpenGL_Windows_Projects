//UNFINISHED!
#if defined IS_FINISHED

//See header file "FilepathWrapper.h" for details
//
//Date:       October 11, 2018
//Programmer: Forrest Miller

#include "FilepathWrapper.h"



Filepath::Filepath(const char * fp) {
	//record filepath

	//check if file exists

	//find and record file extension
}

Filepath::Filepath(std::string fp) {
	//record filepath
	
	//check if file exists
	
	//find and record file extension
}

Filepath::~Filepath() {

}


std::string Filepath::extension() const {
	return mExtension_;
}

int Filepath::getIndexOfLastPeriodInPath() {
	std::string::const_iterator lastPeriod;
	lastPeriod = std::find(mPath_.begin(), mPath_.end(), '.');
	if (lastPeriod == mPath_.end()) {
		return -1;
	}

}

#endif //defined IS_FINISHED