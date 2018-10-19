// Description:  The AsciiAsset class is a simple wrapper for an ASCII-based asset 
//               (for instance '.obj' and '.mtl' assets are ASCII-based) which provides
//               provides a simple interface for dealing with the data.
//               This class has no notion of what type of file it is wrapping, its only job is
//               to manage filetext aquisition and management. It is the responsibility of code
//               using this class to make sure that the aquired filetext is not an empty string.
//
// (NOTE ON HOW THIS FITS INTO OVERALL PROJECT): This class is designed to be held by other classes
//                                               which manage parsing and storing the loaded data. 
//                                               Thus this class is seperate from the whole 'AssetInterface'
//                                               inheritance structure.
//
// Programmer:   Forrest Miller
// Date:         October 9, 2018  
//
//
//
// POSSIBLE ISSUES/PITFALLS/INCONSISTENCIES:
//
//       Portability Issue:    
//                              This class relies on the potentially non-portable 'file_exists()'
//                                function found in "FilepathWrapper.h" file. The only 
//                                place this function is called is in this class's constructor.
//
//       FileText Line Indexing Inconsistency:
//                              The public interface functions 'getLineLength()' and 'getLine()'
//                                treat the first line of text of the filetext string as line 0. 
//                                However this might not match up properly with both general 
//                                intuition and the member variable 'mFileTextLineCount_'.
//                                Basically just be carful when using these functions to ensure
//                                the correct line of text is being accessed.
//                             
//     
// Note from Programmer:   This class has undergone some modifications since I first wrote it 
//                         and as such it could benefit from some refactoring to make parts of
//                         its implementation more coherent and make more sense. For example, 
//                         this class manages its filepath itself instead of relying on the Filepath
//                         object type, but still uses the static functionality of the Filepath 
//                         class to verify a files existance. 
//                         Basically this class is function as is but it's implementation could 
//                         desperatly use an overhaul and some refactoring. 

#pragma once

#ifndef ASCII_ASSET_H_
#define ASCII_ASSET_H_

#include <iostream>   //std::string, fprintf, etc...
#include <fstream>    //provides file i/o
#include <sstream>    //for stringstream functionality (might not need this explicitly)
#include <algorithm>  //for std::find
#include <vector>	  //for std::vector

#include "FilepathWrapper.h"  //Really this class should use the object defined in this header for filepath 
//                            //management, but right now this class does it's own filepath management and just
//                            //uses a few static functions from this file. 

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
		//     const char * fp       --  A filepath to an ascii-encoded resource
		//     bool storeLocalCopy   --  Dictates whether an object-local copy of
		//                                  the resource should be aquired
		AsciiAsset(const char * fp, bool storeLocalCopy = true);

		//Creates an AsciiAsset object based off the provided filepath. On
		//construction, an attempt will be made to open the file at the provided
		//filepath. The file will be automatically loaded into an object-local C++
		//std::string unless specifially told not to through the parameter 'storeLocalCopy'
		//
		//  Parameters:
		//        std::string fp      --  A filepath to an ascii-encoded resource
		//        bool storeLocalCopy --  Dictates whether an object-local copy of
		//                                     the resource should be aquired
		AsciiAsset(const std::string& fp, bool storeLocalCopy = true);

		//Creates a copy of another AsciiAsset object
		AsciiAsset(const AsciiAsset& that);

		//Emplaces another AsciiAsset object into this one
		AsciiAsset(AsciiAsset&& that);


		//------------------------------------------------------------
		//                      Destructor
		//------------------------------------------------------------

		~AsciiAsset() { }

		//------------------------------------------------------------
		//                          Operators
		//------------------------------------------------------------

		//Object will be cast to a bool based off the validity of its filepath.
		//If a valid file exists at the filepath held by this object, then this
		//will cast the object to 'true'. If there is an issue with the filepath, 
		//then this will cast the object to 'false'.
		explicit operator bool() const { return (this->mValidFilepath_); }

		/*  Note: This is a bad idea
		//Object will be cast to a string containing whatever is stored in its 
		//local copy of the filetext. If the file is invalid or no local file-text
		//copy was aquired, then this will just be an empty string.
		operator std::string() const { return (std::move(this->getTextCopy())); }
		*/

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

		//Returns the number of lines of the loaded filetext. If no local filetext was loaded, 
		//then this function will return 0.
		int getNumberOfLines() const {
			if (mHasLocalCopyOfFileText_) {
				return mLineOffsets_.size();
			}
			return 0;
		}
		
		//Retrieves the number of characters on the specified line. If the parameter
		//'line' does not correspond to a valid line number, this function returns 0.
		//Note that a newline character at the end of the line will be included as part of the
		//line length.
		//The first line of FileText is treated by this function as line number 0. 
		size_t getLineLength(int line) const;

		//Retrieves the line of text at the specified line. Line indexing starts at 0.
		//Please stay in bounds.
		std::string getLine(int line) const;
		
		//Gets the length of the stored file-text. Will return 0 if this object has an invalid
		//filepath or if no local copy of the file was ever made.
		size_t getStoredTextLength() const { return mFileText_.length(); }

		//------------------------------------------------------------
		//                          Setters
		//------------------------------------------------------------

		//Changes the filepath to the new filepath. This will erase all of the current state
		//of the object. Use the bool parameter to dictate whether or not to load an object-local
		//copy of the filetext.
		void changeFilepath(const char * newFP, bool storeLocalCopyOfNewFile = true) {
			(*this) = std::move(AsciiAsset(newFP, storeLocalCopyOfNewFile)); //Basically just reconstruct the object with new filepath
		}

		//Changes the filepath to the new filepath. This will erase all of the current state
		//of the object. Use the bool parameter to dictate whether or not to load an object-local
		//copy of the filetext.
		void changeFilepath(const std::string& newFP, bool storeLocalCopyOfNewFile = true) {
			(*this) = std::move(AsciiAsset(newFP, storeLocalCopyOfNewFile)); //Basically just reconstruct the object with new filepath
		}


		//------------------------------------------------------------
		//                    Interface Functions
		//------------------------------------------------------------

		//Loads a string copy of the filetext into this object's memory. Intended to 
		//allow finer control over if/when this class aquires filetext. Has no effect if
		//a copy of the FileText has already been aquired.
		void aquireLocalCopyOfFileText();
		

		//This function will remove all of the lines of the filetext string held within this
		//object that begin with the specified character. Note that doing so will reset this 
		//object's internel LineOffset vector, which may cause issues with iterators and such
		//set before this function was called. This function will ignore whitespace and tabs
		//when searching for a first character.
		//Requires locally-stored filetext. The characters ' ', '\n' and '\t' are illegal.
		void removeLinesBeginningWithCharacter(char c);


		//Requires locally-stored filetext. Returns a vector containing all of the lines 
		//in the filetext that contain the specified substring.
		//It is illegal to specify a substring containing any newline characters.
		//std::vector<int> findAllLinesThatContainSubstr(const std::string& substr) const;

		//Returns the filepath for the Ascii-file wrapped by this class
		std::string getFilepath() const { return mFilepath_; }


	private:        //Fields
		std::string mFilepath_;
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
		void parseFileText(); //Basically build the vector of NewLineLocations    
		//void populateNewLineLocationsVector();

		//If a file contains text but no newlines, tack one on at the end manually
		void addNewlineToOneLineFiletexts();
	};

} //namespace AssetLoadingInternal 

#endif ASCII_ASSET_H_