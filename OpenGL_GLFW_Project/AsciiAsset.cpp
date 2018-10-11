//See header file for more detail

#include "AsciiAsset.h"

namespace AssetLoadingInternal {

	void AsciiAsset::initialize() {
		mFilepath_ = "";
		mFileText_ = "";
		mFileTextLineCount_ = 0;

		mValidFilepath_ = false;
		mHasLocalCopyOfFileText_ = false;
	}

	AsciiAsset::AsciiAsset() {
		initialize();
	}

	AsciiAsset::AsciiAsset(const char * fp, bool storeLocalCopy) {
		initialize();
		mFilepath_ = fp;
		mValidFilepath_ = file_exists(mFilepath_); //checkFilepathValidity();
		if (mValidFilepath_ && storeLocalCopy) {
			mHasLocalCopyOfFileText_ = true;
			loadFile();
			addNewlineToOneLineFiletexts();
			parseFileText(); 
		}
	}
	 
	//I don't expect these to be called that much (at least not within a loop)
	AsciiAsset::AsciiAsset(const AsciiAsset& that) {
		initialize();
		std::string tempFilepathCopy = that.mFilepath_;
		this->mFilepath_ = tempFilepathCopy.c_str();
		
		this->mValidFilepath_ = that.mValidFilepath_;
		this->mHasLocalCopyOfFileText_ = that.mHasLocalCopyOfFileText_;
		if (that.mValidFilepath_) {
			if (that.mHasLocalCopyOfFileText_) {
				this->mFileText_ = that.mFileText_;
				this->mFileTextLineCount_ = that.mFileTextLineCount_;
				this->mLineOffsets_ = that.mLineOffsets_;
			}
		}
		else {
			this->mFileText_ = "";
			this->mFileTextLineCount_ = 0;
			this->mLineOffsets_.clear();
		}
	}

	AsciiAsset::AsciiAsset(AsciiAsset&& that) {
		initialize();
		this->mFilepath_ = std::move(that.mFilepath_);
		this->mValidFilepath_ = that.mValidFilepath_;
		this->mHasLocalCopyOfFileText_ = that.mHasLocalCopyOfFileText_;
		if (that.mValidFilepath_) {
			if (that.mHasLocalCopyOfFileText_) {
				this->mFileText_.swap(that.mFileText_);
				this->mFileTextLineCount_ = that.mFileTextLineCount_;
				this->mLineOffsets_.swap(that.mLineOffsets_);
			} 
		}
		else {
			this->mFileText_ = "";
			this->mFileTextLineCount_ = 0;
			this->mLineOffsets_.clear();
		}
	}

	AsciiAsset& AsciiAsset::operator=(const AsciiAsset& that) {
		fprintf(WRNLOG, "\nWarning! AsciiAsset operator= was called (copy, not the move) but I\n"
			"can't shake the sense I made a mistake with this operator. Please verify a correct copy occured!\n");
		initialize();
		std::string tempFilepathCopy = that.mFilepath_;
		this->mFilepath_ = tempFilepathCopy.c_str();

		this->mValidFilepath_ = that.mValidFilepath_;
		this->mHasLocalCopyOfFileText_ = that.mHasLocalCopyOfFileText_;
		if (that.mValidFilepath_) {
			if (that.mHasLocalCopyOfFileText_) {
				this->mFileText_ = that.mFileText_;
				this->mFileTextLineCount_ = that.mFileTextLineCount_;
				this->mLineOffsets_ = that.mLineOffsets_;
			}
		}
		else {
			this->mFileText_ = "";
			this->mFileTextLineCount_ = 0;
			this->mLineOffsets_.clear();
		}
		return (*this);
	}

	AsciiAsset& AsciiAsset::operator=(AsciiAsset&& that) {
		fprintf(WRNLOG, "\nWarning! AsciiAsset operator= was called (the move, not the copy) but I\n"
			"can't shake the sense I made a mistake with this operator. Please verify a correct move occured!\n");
		initialize();
		std::string tempFilepathCopy = that.mFilepath_;
		this->mFilepath_ = tempFilepathCopy.c_str();
		this->mValidFilepath_ = that.mValidFilepath_;
		this->mHasLocalCopyOfFileText_ = that.mHasLocalCopyOfFileText_;

		if (that.mValidFilepath_) {
			if (that.mHasLocalCopyOfFileText_) {
				this->mFileText_ = that.mFileText_;
				this->mFileTextLineCount_ = that.mFileTextLineCount_;
				this->mLineOffsets_ = that.mLineOffsets_;
			}
		}
		else {
			this->mFileText_ = "";
			this->mFileTextLineCount_ = 0;
			this->mLineOffsets_.clear();
		}
		return (*this);
	}

	std::string AsciiAsset::getTextCopy() const {
		if (mValidFilepath_ && mHasLocalCopyOfFileText_) {
			return mFileText_;
		}
		else if (!mHasLocalCopyOfFileText_) {
			std::string temp;
			loadFileNoLocalCopy(temp);
			return std::move(temp); //Hopefully this still doesn't result in multiple unnecessary string-copys 
		}
		else {
			return "";
		}
	}

	std::string AsciiAsset::getLine(int line) const {
		if ( (mFileTextLineCount_ <= line) || (0 > line) ) {
			return "";
		}
		NewLineLocation lineLocation = mLineOffsets_[line];
		return (mFileText_.substr(lineLocation.offset, lineLocation.lineLength));
	}

	const char * AsciiAsset::getLine_CStr(int line) const {
		if ( (mFileTextLineCount_ <= line) || (0 > line) ) {
			return "\0";
		}
		NewLineLocation lineLocation = mLineOffsets_[line];
		return (mFileText_.substr(lineLocation.offset, lineLocation.lineLength).c_str());
	}

	//void AsciiAsset::changeFilepath(const char * newFP) {
	//
	//}

	void AsciiAsset::aquireLocalCopyOfFileText() {
		if (!mValidFilepath_) {
			return;
		}
		else if (mHasLocalCopyOfFileText_) {
			return;
		}
		else {
			mHasLocalCopyOfFileText_ = true;
			loadFile();
			addNewlineToOneLineFiletexts();
			parseFileText();
		}
	}

	//This function assumes that the mLineOffsets_ vector is complete and up to date. This
	//function will update that vector as it proceeds. Psych! Actually if it operates on the file 
	//backwards, then it doesn't have to worry about modifying the vector until after all the lines 
	//were deleted.
	void AsciiAsset::removeLinesBeginningWithCharacter(char c) { //For filtering out comments
		if (!mValidFilepath_ || !mHasLocalCopyOfFileText_) {
			return;
		}
		//If the text is empty
		if (mFileText_.length() <= 1) {
			return;
		}
		//If the vector of offsets has not been populated
		if (mLineOffsets_.size() == 0u) { //If we just have one line of text
			return;
		}

		//Don't remove if c is any of the following 3 char
		if ((c == ' ') || (c == '\n') || (c == '\t') ) {
			if (c == ' ')
				fprintf(WRNLOG, "\nWARNING! Removing character ' ' from the filetext is an invalid operation.\n");
			else if (c == '\n')
				fprintf(WRNLOG, "\nWARNING! Removing character '\\n' from the filetext is an invalid operation.\n");
			else 
				fprintf(WRNLOG, "\nWARNING! Removing character '\\t' from the filetext is an invalid operation.\n");
			
			return;
		}

		//Otherwise, search for lines beginning with the specified character and delete to create an updated filetext
		std::vector<NewLineLocation>::iterator textLineIter = mLineOffsets_.begin();
		std::vector<int> linesToDelete;
		bool readyToMoveToNextLine;

		//Here we just loop through and get a second vector recording lines to delete
		while (textLineIter != mLineOffsets_.end()) { //For each line
			const char * text = mFileText_.c_str();
			text += textLineIter->offset;
			readyToMoveToNextLine = false;
			while ( (!readyToMoveToNextLine) &&
				            ((*text != '\n') && (*text != '\0'))) { //For each character in the line
				if (*text == ' ') { 
					text++; //Skip over spaces
					continue;
				}
				else if (*text == c) {
					linesToDelete.push_back(textLineIter->lineNumber);
					readyToMoveToNextLine = true;
				}
				else {
					readyToMoveToNextLine = true;
				}
			}
			textLineIter++;
		}

		//If we detected the filetext contains lines that are eligable for deletion
		if (linesToDelete.size() > 0u) {
			//Now we can use substrings to create the updated filetext
			std::string updatedFileText;
			int linesToDeleteIndex = 0;
			for (int i = 0; i < mLineOffsets_.size(); i++) {
				if (linesToDelete[linesToDeleteIndex] == i) {
					continue;
				}
				else {
					updatedFileText = mFileText_.substr(mLineOffsets_[i].offset, mLineOffsets_[i].lineLength);
				}
			}

			//fprintf(MSGLOG, "AsciiAsset has removed all lines from FileText that began with char '%c'\n"
			//	"The result of this operation was that %u lines were removed!\n", c, linesToDelete.size());

			mFileText_.swap(updatedFileText);
			parseFileText(); //Reparse the new fileText
		}
	}
	
	void AsciiAsset::loadFile() {
		std::ifstream inFile{ mFilepath_ }; //Open a file stream from the filepath 
		mFileText_ = { std::istreambuf_iterator<char>(inFile), std::istreambuf_iterator<char>() };
	}


	void AsciiAsset::loadFileNoLocalCopy(std::string & target) const {
		std::ifstream inFile{ mFilepath_ }; //Open a file stream from the filepath 
		target = { std::istreambuf_iterator<char>(inFile), std::istreambuf_iterator<char>() };
	}

	//This function is not yet fully implemented. See "FilepathHelperFunctions.h" for replacement
	/*bool AsciiAsset::checkFilepathValidity() {
		std::ifstream inFile;
		inFile.open(mFilepath_);
		if (!(inFile.good())) {
			mValidFilepath_ = false;
			mFileText_ = "";
		}
	}*/

	void AsciiAsset::parseFileText() {
		if (mFileText_.length() > 0u) {
			int lineNumberCounter = 0;
			std::string::iterator textIterator;
			//Find the location of the first newLine character
			textIterator = std::find(mFileText_.begin(), mFileText_.end(), '\n');
			if (textIterator == mFileText_.end()) {
				addNewlineToOneLineFiletexts(); //Add a newline to the text so that 'line 0' can be well defined
				//Note that our iterator may have been invalidated 
				//after modifying the string, so we must reaquire it
				textIterator = std::find(mFileText_.begin(), mFileText_.end(), '\n');
			}
			//So we know at this point that we have textIterator placed on the first newline 
			//that occurs in the file. Thus we can add position 0 as a special case (in that 
			//it's offset isn't to a newline char) before entering the parsing loop.

			//Add location 0 for line 0 to the vector
			mLineOffsets_.emplace_back(lineNumberCounter++, 0, std::distance(mFileText_.begin(), textIterator) + 1);

			//parse the rest of the lines of text
			while ( (++textIterator) != mFileText_.end() ) { //Repeat until we run out of lines
				std::string::iterator startOfCurrentLine = textIterator;
				textIterator = std::find(startOfCurrentLine, mFileText_.end(), '\n');

				//If we reach the end without encountering a newline
				if (textIterator == mFileText_.end()) { 
					textIterator--;
					mLineOffsets_.emplace_back(lineNumberCounter++,                                      //Line number
											   std::distance(mFileText_.begin(), startOfCurrentLine),    //Offset
											   std::distance(startOfCurrentLine, textIterator) + 1);     //Line length
					continue; //Next iteration will cause exit condition to trigger
				}
				else if (startOfCurrentLine == textIterator) { //If the line consists solely of a single newline char
					mLineOffsets_.emplace_back(lineNumberCounter++, std::distance(mFileText_.begin(), startOfCurrentLine), 1);
				}
				else { //We encountered a new line that isn't at the end of the text
					mLineOffsets_.emplace_back(lineNumberCounter++,                                      //Line number
						                       std::distance(mFileText_.begin(), startOfCurrentLine),    //Offset
						                       std::distance(startOfCurrentLine, textIterator) + 1);     //Line Length
				}
			}

		}
		mFileTextLineCount_ = mLineOffsets_.size();
		//fprintf(MSGLOG, "\nThe file %s was parsed. This file is %u lines.\n", mFilepath_, mFileTextLineCount_);
	}

	//I am not sure if this function is actually necessary...
	void AsciiAsset::addNewlineToOneLineFiletexts() {
		if (mFileText_.length() > 0) {
			std::string::iterator it;

			it = std::find(mFileText_.begin(), mFileText_.end(), '\n');
			if (it != mFileText_.end())
				return; //No need to add a newline character
			else
				mFileText_ += '\n'; //Add a newline at the end of the one liner program
		}
	}


} //namespace AssetLoadingInternal