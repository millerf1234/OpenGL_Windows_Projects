#include "TextEngine.h"



TextEngine::TextEngine() {
	initialize();
}

TextEngine::TextEngine(const char * fontFilePath) {
	initialize();
	initializeFreeTypeLibrary();
	loadFont(fontFilePath);
}

TextEngine::~TextEngine() {
	if (mFace) {
		FT_Done_Face(mFace);
	}

	if (mLibrary) {
		FT_Done_FreeType(mLibrary);
		mLibrary = nullptr;
	}
}


void TextEngine::loadFont(const char * fontFilePath) {
	mFontPath = fontFilePath;
	initializeFromFontFilePath();
}



void TextEngine::initialize() {
	mReady = false;
	mValidFontFilePath = false;
	mFontPath = "\0"; 

	mErrorStatus = FT_Err_Ok;
	mFaceIndex = 0l;
	mFace = nullptr;
	mLibrary = nullptr;

}



void TextEngine::initializeFreeTypeLibrary() {
	fprintf(MSGLOG, "\nInitializing FreeType library!\n");
	if (mLibrary) {
		fprintf(ERRLOG, "\nError initializing FreeType library. It looks like a library has already been initialized!\n");
		return;
	}
	mErrorStatus = FT_Init_FreeType(&mLibrary);
	if (checkForError()) {
		fprintf(ERRLOG, "\nERROR OCCURED INITIALIZING freetype library. Error code is: %d\n", mErrorStatus);
		mLibrary = nullptr;
		return;
	}

}



void TextEngine::initializeFromFontFilePath() {
	if (!mLibrary) {
		initializeFreeTypeLibrary();
		//fprintf(ERRLOG, "\nError initializing Text Engine font from filepath! Please make sure a Free Type library has been initialized first!\n");
		//return;
	}
	if (mValidFontFilePath) {
		fprintf(ERRLOG, "\mError trying to set a new text file path, this object already had a text filepath set!\n");
		return;
	}
	if (strcmp(mFontPath, "\0") == 0){
		fprintf(ERRLOG, "\nERROR! Please use a valid filepath for initializing this TextEngine object!\n");
		return;
	}

	fprintf(MSGLOG, "Initializing TextEngine from font filepath: %s\n", mFontPath);

	mErrorStatus = FT_New_Face(mLibrary, mFontPath, mFaceIndex, &mFace);
	if (checkForError()) {
		fprintf(ERRLOG, "\nERROR! FreeType was unable to load the font at filepath: %s\n", mFontPath);
		return;
	}
	else {
		mValidFontFilePath = true;
		mReady = true;
	}

}




bool TextEngine::checkForError() {
	return (mErrorStatus != FT_Err_Ok);
}
