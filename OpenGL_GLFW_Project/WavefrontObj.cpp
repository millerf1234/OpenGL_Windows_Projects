//File:  WavefrontObj.cpp
//Description:    This file contains the implementation for the WavefrontObj class. See the 
//                header file 'WavefrontObj.h' for more details.
//
//Programmer:     Forrest Miller
//Date:           October 15, 2018


#include "WavefrontObj.h"

#ifndef WAVEFRONT_OBJ_INCOMPLETE_ //Code is disabled since I never finished it and it isn't used anywhere

void WavefrontObj::initialize() {
	mFile_ = nullptr;
	
}

WavefrontObj::WavefrontObj(const char * filepath) : AssetInterface() {
	initialize();
	if (!aquireFile(filepath)) {
		logError("Unable to acquire parse-able filetext!");
		mLoaded_ = false;
		mValid_ = false;
		return;
	}
	//parseFile();
}

WavefrontObj::WavefrontObj(std::string filepath) : AssetInterface() {
	initialize();
	if (!aquireFile(filepath)) {
		logError("Unable to acquire parse-able filepath!");
		mLoaded_ = false;
		mValid_ = false; 
		return;
	}
	//parseFile();
}



//Private helper function definitions 

void WavefrontObj::logError(const char* errorDescription) const {
	if (mFile_) {
		fprintf(ERRLOG, "\nError encountered with Wavefront Obj \"%s.\"\nError reason:\n\t%s\n", mFile_->getFilepath().c_str(), errorDescription);
	}
	else {
		if (errorDescription != nullptr) {
			fprintf(ERRLOG, "\nError encountered while trying to load a Wavefront Obj file.\n"
				"Unfortunately the Filewrapper is null, so no information on file name can be provided!\n"
				"The error message is: %s", errorDescription);
		}
		else {
			fprintf(ERRLOG, "\nERROR within WavefrontObj class!\n"
                "Both Filewrapper object and error message are NULL!\n");
		}
	}
}


bool WavefrontObj::aquireFile(std::string filepath) {
	mFile_ = std::make_unique <AssetLoadingInternal::AsciiAsset>(filepath/*.c_str()*/);
	if (!mFile_) {
		return false;
	}
	else {
		mFile_->aquireLocalCopyOfFileText(); //Load the file as a string
		if (mFile_->getStoredTextLength() == 0) { 
			fprintf(ERRLOG, "\nERROR! The '.obj' file \"%s\" is empty!\n", filepath.c_str());
		}
		return false;
	}
	return true; //File was successfully acquired
}

#endif //WAVEFRONT_OBJ_INCOMPLETE_