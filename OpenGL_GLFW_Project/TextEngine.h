//This class is intended to provide efficent text rendering through FreeType
//Created by Forrest Miller on July 19, 2018

#pragma once

#ifndef TEXT_ENGINE_H
#define TEXT_ENGINE_H

#include <list>

//#include "ProjectSetup.h"
#include "ProjectConstants.h"
#include "ProjectParameters.h"
#include "TextLine.h"


class TextEngine {
public:
	TextEngine();
	TextEngine(const char * fontFilePath);
	~TextEngine();

	void loadFont(const char * fontFilePath);

	FT_Error getError() const { return mErrorStatus; }
	bool ready() const { return mReady; }
	bool validFontFilePath() const { return mValidFontFilePath;  }



private: 
	bool mReady;
	bool mValidFontFilePath;
	std::list<std::unique_ptr<TextLine>> mTextLinesToRender;
	const char * mFontPath; //Only 1 font in use at a time (for now)

	FT_Error mErrorStatus;
	FT_Long mFaceIndex;
	FT_Face mFace;
	FT_Library mLibrary;

	void initialize();
	void initializeFreeTypeLibrary();
	void initializeFromFontFilePath();

	bool checkForError();

};

#endif //#ifndef TEXT_ENGINE_H