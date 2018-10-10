//See header file for more detail

#include "AsciiAsset.h"

namespace AssetLoadingInternal {

	void AsciiAsset::initialize() {
		mFilepath_ = "\0";
		mFileText_ = "";
		mFileTextLineCount_ = 0;

		mValidFilepath_ = false;
		mHasLocalCopyOfFileText_ = false;
	}

	AsciiAsset::AsciiAsset() {

	}





} //namespace AssetLoadingInternal