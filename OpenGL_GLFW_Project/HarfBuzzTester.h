//Class to just play around with how harfbuzz works

#pragma once

#ifndef HARFBUZZ_TESTER
#define HARFBUZZ_TESTER

#include <sstream>

#include "ProjectSetup.h"
#include "ProjectConstants.h"
#include "ProjectParameters.h"


class HarfBuzzTester {
public:
	HarfBuzzTester();
	~HarfBuzzTester();

	void runTest();

private:
	bool error;
	bool fontWasShaped;

	hb_buffer_t * hbBuf;
	hb_font_t * hbFont;

	hb_glyph_info_t * hbGlyphInfo;
	unsigned int hbGlyphInfoArraySize;
	hb_glyph_position_t * hbGlyphPosition;
	unsigned int hbGlyphPositionsArraySize;


	FT_Long ftFaceIndex;
	FT_Face ftFace;
	bool hasFace;
	FT_Library ftFontLibrary;
	bool hasFontLibrary;


	//Helper functions  (Should be called in descending order)
	bool createHarfBuzzBuffer(); 
	bool guessTheScriptLanguageAndDirectionOfTheBuffer();
	bool initializeFreeTypeLibrary();
	bool createAFaceAndAFont(); //Requires an initialized FreeType Library
	bool syncCreatedFontWithHarfBuzz();
	bool shape();
	bool getGlyphAndPositionInformation(); 
	bool iterateOverEachGlyph(); //Rename this function? "extractGlyphVerts?"

};

#endif //HARFBUZZ_TESTER