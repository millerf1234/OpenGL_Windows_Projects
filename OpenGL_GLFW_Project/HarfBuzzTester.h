//Class to just play around with how harfbuzz works

#pragma once

#ifndef HARFBUZZ_TESTER
#define HARFBUZZ_TESTER

#include "ProjectSetup.h"
#include "ProjectConstants.h"
#include "ProjectSetup.h"

class HarfBuzzTester {
public:
	HarfBuzzTester();
	~HarfBuzzTester();

	void runTest();

private:
	bool error;

	hb_buffer_t * hbBuf;
	hb_font_t * hbFont;

	FT_Long ftFaceIndex;
	FT_Face * ftFace;
	FT_Library * ftFontLibrary;


	//Helper functions  (Should be called in descending order)
	bool createHarfBuzzBuffer(); 
	bool guessTheScriptLanguageAndDirectionOfTheBuffer();
	bool initializeFreeTypeLibrary();
	bool createAFaceAndAFont(); //Requires an initialized FreeType Library
};

#endif //HARFBUZZ_TESTER