#include "HarfBuzzTester.h"

//Code is based off of: https://harfbuzz.github.io/hello-harfbuzz.html
//Also see FreeType API: https://www.freetype.org/freetype2/docs/reference/ft2-base_interface.html#FT_Library

HarfBuzzTester::HarfBuzzTester() {
	error = false;

	//Harfbuzz types and handles
	hbBuf = nullptr;
	hbFont = nullptr;

	//FreeType types and handles
	ftFaceIndex = static_cast<FT_Long>(0L);
	ftFontLibrary = nullptr;
	ftFace = nullptr;
}


HarfBuzzTester::~HarfBuzzTester() {
	if (ftFontLibrary) {
		FT_Done_FreeType(*ftFontLibrary);
		ftFontLibrary = nullptr;
	}

	//if ()  //Delete face? Do before delete library?

}


void HarfBuzzTester::runTest() {
	fprintf(MSGLOG, "\n-----------------Testing Harfbuzz!-----------------\n");
	fprintf(MSGLOG, "Beginning HarfBuzz Test Sequence...\n");

	fprintf(MSGLOG, "Step 1)\n");
	if (!createHarfBuzzBuffer()) {
		return;
	}

	fprintf(MSGLOG, "Step 2)\n");
	if (!guessTheScriptLanguageAndDirectionOfTheBuffer()) {
		return;
	}

	fprintf(MSGLOG, "Step 3)\n");
	if (!initializeFreeTypeLibrary()) {
		return;
	}
	if (!createAFaceAndAFont()) {
		return;
	}


}


bool HarfBuzzTester::createHarfBuzzBuffer() {
	static const char * testText = "TEST TEXT\0\0";
	
	fprintf(MSGLOG, "\tCreating HarfBuzz Buffer...\n");

	hbBuf = hb_buffer_create();
	if (!hbBuf) {
		fprintf(ERRLOG, "ERROR creating HarfBuzz buffer!\n");
		error = true;
		return false;
	}
	fprintf(MSGLOG, "\tPlacing test text string into HarfBuzz buffer...\n");
	fprintf(MSGLOG, "\t\tTest string is: %s\n", testText);
	hb_buffer_add_utf8(hbBuf, testText, strlen(testText), 0, strlen(testText));

	return true;
}

bool HarfBuzzTester::guessTheScriptLanguageAndDirectionOfTheBuffer() {
	fprintf(MSGLOG, "\tGuessing the Script, Language and Direction of the buffered text...\n");
	hb_buffer_guess_segment_properties(hbBuf);
	return true;
}

bool HarfBuzzTester::initializeFreeTypeLibrary() {
	fprintf(MSGLOG, "\tInitializing FreeType Font Library...\n");
	if (FT_Init_FreeType(ftFontLibrary) != 0) {
		fprintf(ERRLOG, "\nERROR creating font library with FreeType!\n");
		return false;
	}
	return true;
}

bool HarfBuzzTester::createAFaceAndAFont() {
	fprintf(MSGLOG, "\tUsing FreeType font library to create a Face and Font...\n");
	
	const char * font_path = "\0";
	fprintf(MSGLOG, "\tUsing font: %s\n", font_path);

	FT_Error noError = static_cast<FT_Error>(0);
	if (noError != FT_New_Face(*ftFontLibrary, font_path, ftFaceIndex, ftFace)) {
		fprintf(ERRLOG, "\nERROR! FreeType encountered an issue while attempting to\n");
		fprintf(ERRLOG, "create a new face from the font at fontpath:\n\t");
		fprintf(ERRLOG, "%s\n", font_path);
		return false;
	}
	return true;
}