//Description:  The AsciiAsset class is a simple wrapper for an ASCII-based asset 
//				(for instance '.obj' and '.mtl' assets are ASCII-based) which provides
//			    provides a simple interface for dealing with the data.
//              This class has no notion of what type of file it is wrapping, its only job is
//				to manage filetext aquisition and management. It is the responsibility of code
//				using this class to make sure that the aquired filetext is not an empty string.
//
//Programmer:   Forrest Miller
//Date:         October 9, 2018  

#pragma once

#ifndef ASCII_ASSET_H_
#define ASCII_ASSET_H_

#include <iostream>   //std::string, fprintf, etc...
#include <fstream>    //provides file i/o
#include <sstream>    //for stringstream functionality (might not need this explicitly)
#include <algorithm>  //for std::find
#include <vector>	  //for std::vector


#include "FilepathHelperFunctions.h"
#include "LoggingMessageTargets.h"

namespace AssetLoadingInternal {

	class AsciiAsset {
	public:
		//-------------------------------------------------------
		//                      Constructors
		//-------------------------------------------------------

		//Creates an empty AsciiAsset object. The created object will be treated as
		//an asset which was constructed with an invalid filepath and this objects
		//stored string copy of the filetext will be empty. 
		AsciiAsset();

		//Creates an AsciiAsset object based off the provided filepath. On
		//construction, an attempt will be made to open the file at the provided
		//filepath. The file will be automatically loaded into an object-local C++
		//std::string unless specifially told not to through the parameter 
		//'storeLocalCopy'.
		//   Parameters:
		//     const char * fp       --  A filepath to an ascii-based resource
		//     bool storeLocalCopy   --  Dictates whether an object-local copy of
		//                                  the resource should be aquired
		AsciiAsset(const char * fp, bool storeLocalCopy = true);

		//Creates a copy of another AsciiAsset object
		AsciiAsset(const AsciiAsset& that);

		//Emplaces another AsciiAsset object into this one
		AsciiAsset(AsciiAsset&& that);


		//------------------------------------------------------------
		//                      Destructor
		//------------------------------------------------------------

		~AsciiAsset() { /*std::unique_ptr handles releasing aquired resources*/ }


		//------------------------------------------------------------
		//                          Operators
		//------------------------------------------------------------

		//Object will be cast to a bool based off the validity of its filepath.
		//If a valid file exists at the filepath held by this object, then this
		//will cast the object to 'true'. If there is an issue with the filepath, 
		//then this will cast the object to 'false'.
		explicit operator bool() { return (this->mValidFilepath_); }

		//Object will be cast to a string containing whatever is stored in its 
		//local copy of the filetext. If the file is invalid or no local file-text
		//copy was aquired, then this will just be an empty string.
		operator std::string() const { return (std::move(this->getTextCopy())); }

		//Object will be cast to a c-string containing whatever is stored in its
		//local copy of the asset. If the filepath is invalid or if no local copy
		//was ever loaded, then this will just be an empty string.
		operator const char*() const { return mFileText_.c_str(); }

		//Returns the character located at the specified offset into this object's 
		//stored filetext. Will cause issues if index goes beyond the end of this object's
		//stored filetext string or if called on an object that does not store a 
		//local copy of it's filetext. Will return '\0' if filepath is invalid.
		const char * operator[] (size_t offset) const {
			if (mHasLocalCopyOfFileText_) {
				if (mValidFilepath_ && (offset < mFileText_.length())) {
					const char * tempCopy = mFileText_.c_str();
					tempCopy += offset;
					return tempCopy;
					//return (mFileText_.substr(offset, 1u).c_str());
				}
			}
			return '\0';
		}

		//Copies the contents of a different AsciiAsset object.
		AsciiAsset& operator=(const AsciiAsset& that);

		//Moves the contents of a different AsciiAsset object into this object.
		AsciiAsset& operator=(AsciiAsset&& that);


		//------------------------------------------------------------
		//                          Getters
		//------------------------------------------------------------

		//Retrieves the text aquired from the file at the stored filepath. This function's
		//implementation is state dependent according to the following: 
		//		(i)    If this object has a valid filepath and already has a local copy of
		//             the filetext, then this function will just create and return a copy of 
		//             its locally-stored filetext.
		//      (ii)   If this object has a valid filepath but does not have a stored local
		//             copy of the filetext, the file will be opened and parsed (without a
		//             local copy being saved in the process). Note that this means multiple
		//             calls to this function without a localally stored copy will result in
		//             multiple file accesses
		//      (iii)  If this object was provided an invalid filepath (or if a filepath was 
		//             never specified after this object was constructed by the default 
		//             constructor) then this function will just return an empty string.
		std::string getTextCopy() const; 

		//Retrieves the line of text at the specified line. Line indexing starts at 0.
		//Please stay in bounds.
		std::string getLine(int line) const;

		//Retrieves the line of text at the specified line number as a c-string. Line
		//indexing begins at 0. Getting the line as a cstring may be useful when parsing 
		//the file.
		const char * getLine_CStr(int line) const;
		
		//Gets the length of the stored file-text. Will return 0 if this object has an invalid
		//filepath or if no local copy of the file was ever made.
		size_t getStoredTextLength() const { return mFileText_.length(); }

		//------------------------------------------------------------
		//                          Setters
		//------------------------------------------------------------

		//NOTE:This setter is not worth implementing at the moment. Just reconstruct a new object 
		//     and avoid the default constructor.
		////Basically equivalent to comepletly reconstructing this object from a new filepath.
		//void changeFilepath(const char * newFP);



		//------------------------------------------------------------
		//                    Interface Functions
		//------------------------------------------------------------

		//     todo  --   Write a better description for this function...
		//This function is intended to allow for finer control over when an AsciiAsset 
		//parses an ASCII-based file into its own locally stored memory. This
		//object will replace the contents of its current local copy of the 
		//Ascii filetext, then will attempt to open the resource at the resource filepath
		//and extract a copy of the data. An invalid filepath will cause the operation 
		//to abort, however no exception will be thrown for failure at opening a file
		//(at least, no exceptions will be thrown by this class, but system code called by
		//this function may still throw).
		void aquireLocalCopyOfFileText();

		//This function will remove all of the lines of the filetext string held within this
		//object that begin with the specified character. Note that doing so will reset this 
		//object's internel LineOffset vector, which may cause issues with iterators and such
		//set before this function was called. This function will ignore whitespace and tabs
		//when searching for a first character.
		void removeLinesBeginningWithCharacter(char c);

	private:        //Fields
		const char * mFilepath_;
		std::string mFileText_;
		int mFileTextLineCount_;

		typedef struct NewLineLocation { //The start of a non-empty filetext string will be considered 
			int lineNumber;              //'location 0' representing line number 0
			int offset;
			int lineLength;
			NewLineLocation(int number, int offset) : lineNumber(number), offset(offset) { lineLength = 0; }
			NewLineLocation(int number, int offset, int lineLength) : lineNumber(number), offset(offset), lineLength(lineLength) { ; }
		} NewLineLocation;
		
		std::vector<NewLineLocation> mLineOffsets_;

		//These next 2 variables represent the object's state
		bool mValidFilepath_;        
		bool mHasLocalCopyOfFileText_;


		//Internal Helper Functions
		void initialize();
		void loadFile();
		void loadFileNoLocalCopy(std::string & target) const; //Bypasses this object's stored filetext string
		//void checkFilepathValidity();
		void parseFiletext(); //Basically build the vector of NewLineLocations    
		//void populateNewLineLocationsVector();

		//If a file contains text but no newlines, tack one on at the end manually
		void addNewlineToOneLineFiletexts();
	};

} //namespace AssetLoadingInternal 

#endif ASCII_ASSET_H_