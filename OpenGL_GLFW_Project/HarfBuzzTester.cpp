#include "HarfBuzzTester.h"

//Code is based off of: https://harfbuzz.github.io/hello-harfbuzz.html
//Also see FreeType API: https://www.freetype.org/freetype2/docs/reference/ft2-base_interface.html#FT_Library


static constexpr size_t ITERATIONS_TO_PRINT = 60ul;

HarfBuzzTester::HarfBuzzTester() {
	error = false;
	fontWasShaped = false;

	//Harfbuzz types and handles
	hbGlyphInfoArraySize = 0u; //It turns out InfoArraySize and PositionsArraySize should be the same variable 
	hbGlyphPositionsArraySize = 0u;
	hbBuf = nullptr;
	hbFont = nullptr;
	hbGlyphInfo = nullptr;
	hbGlyphPosition = nullptr;

	//FreeType types and handles
	ftFaceIndex = static_cast<FT_Long>(0L);


	hasFace = false;
	hasFontLibrary = false;
}


HarfBuzzTester::~HarfBuzzTester() {

	//Call HarfBuzz Cleanup functions
	if (hbFont) {
		hb_font_destroy(hbFont);
		//hbFont = nullptr;
	}
	if (hbBuf) {
		hb_buffer_destroy(hbBuf);
		//hbBuf = nullptr;
	}

	//Call FreeType Cleanup Functions
	//I think HarfBuzz handles releasing the face since I am doing the reference function call... maybe
	/*if (hasFace) {
		FT_Done_Face(ftFace);
		hasFace = false;
	}*/
	//library should be the last thing freed from FT
	if (hasFontLibrary) {
		FT_Done_FreeType(ftFontLibrary);
		hasFontLibrary = false;
	}

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
	if (!syncCreatedFontWithHarfBuzz()) {
		return;
	}

	fprintf(MSGLOG, "Step 4)\n");
	if (!shape()) {
		return;
	}

	fprintf(MSGLOG, "Step 5)\n");
	if (!getGlyphAndPositionInformation()) {
		return;
	}

	fprintf(MSGLOG, "Step 6)\n");
	if (!iterateOverEachGlyph()) {
		return;
	}

	fprintf(MSGLOG, "\n---------HarfBuzz Test Completed Successfully---------\n");
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

	FT_Error noError = static_cast<FT_Error>(0);
	if (noError != FT_Init_FreeType(&ftFontLibrary)) {
		fprintf(ERRLOG, "\nERROR creating font library with FreeType!\n");
		return false;
	}
	hasFontLibrary = true;
	return true;
}






bool HarfBuzzTester::createAFaceAndAFont() {
	if (!hasFontLibrary) {
		fprintf(ERRLOG, "ERROR! Unable to have freetype create a face and font without a FreeType library first being initialized!\n");
		return false;
	}

	fprintf(MSGLOG, "\tUsing FreeType font library to create a Face and Font...\n");
	
	const char * font_path = "Fonts\\Roboto-Black.ttf"; //Since this is just a demo project, leave filepath hardcoded and don't use filepath in ProjectResourceFilepath.h
	fprintf(MSGLOG, "\tUsing font: %s\n", font_path);

	FT_Error noError = static_cast<FT_Error>(0);
	if (noError != FT_New_Face(ftFontLibrary, font_path, ftFaceIndex, &ftFace)) {
		fprintf(ERRLOG, "\nERROR! FreeType encountered an issue while attempting to\n");
		fprintf(ERRLOG, "create a new face from the font at fontpath:\n\t");
		fprintf(ERRLOG, "%s\n", font_path);
		return false;
	}
	hasFace = true;
	return true;
}






bool HarfBuzzTester::syncCreatedFontWithHarfBuzz() {
	fprintf(MSGLOG, "\tSyncing Font with HarfBuzz...\n");
	if (!hasFontLibrary || !hasFace) {
		fprintf(ERRLOG, "\nUnable to sync created font with HarfBuzz because either the Font Library or Face is invalid!\n");
		return false;
	}

	//Doing the "create_referenced" function will keep the FT_Face alive for as long as the HB_Face is alive
	//see: https://github.com/harfbuzz/harfbuzz/blob/master/src/hb-ft.h   around line 75 (discription applies to both font and face)
	hbFont = hb_ft_font_create_referenced(ftFace); //? referenced? 
	
	if (!hbFont) {
		return false;
	}
	fprintf(MSGLOG, "\thbFont created. hbFont is %p\n", hbFont);
	return true;
}







bool HarfBuzzTester::shape() {
	fprintf(MSGLOG, "\tHarfBuzz is shaping font glyphs...\n");
	
	if (!hasFontLibrary || !hasFace || !hbFont) {
		fprintf(ERRLOG, "\nError shaping font! An issue has occured before this step is ready to commence!\n");
		return false;
	}

	hb_shape(hbFont, hbBuf, NULL, 0);

	fprintf(MSGLOG, "\tFont Successfully shaped!\n");

	fontWasShaped = true;
	return true;
}






bool HarfBuzzTester::getGlyphAndPositionInformation() {
	fprintf(MSGLOG, "\tRetreiving generated Glyph and Position information...\n");

	//Perform safety check to make sure we are ready for this step
	if (!fontWasShaped) {
		fprintf(ERRLOG, "\nError! Unable to retrieve GLyph and Position information because the font was never shaped!\n");
		return false;
	}

	//Retrieve Glyph Info
	hbGlyphInfo = hb_buffer_get_glyph_infos(hbBuf, &hbGlyphInfoArraySize);
	if (hbGlyphInfo) {
		fprintf(MSGLOG, "\tGlyph Information successfully Retrieved for %u glyphs!\n", hbGlyphInfoArraySize);
	}
	else {
		fprintf(ERRLOG, "\nERROR occured while trying to retieve Glyph info!\n");
		return false;
	}

	//Retrieve Glyph Positions
	hbGlyphPosition = hb_buffer_get_glyph_positions(hbBuf, &hbGlyphPositionsArraySize);
	if (hbGlyphPosition) {
		fprintf(MSGLOG, "\tGlyph Positions successfully Retrieved for %u glyphs!\n", hbGlyphPositionsArraySize);
	}
	else {
		fprintf(ERRLOG, "\nERROR occured while trying to retieve Glyph Positions!\n");
		return false;
	}

	return true;
}








bool HarfBuzzTester::iterateOverEachGlyph() {
	fprintf(MSGLOG, "\tIterating over each glyph to \"Draw\" the font!\n");

	if ((hbGlyphInfoArraySize == 0u) || (hbGlyphPositionsArraySize == 0u)) {
		fprintf(ERRLOG, "\nERROR! One or both of the glyph information/position arrays is empty! Nothing to iterate over!\n");
		return false;
	}

	//Use a stringstream obj
	std::ostringstream glyphData;
	glyphData << "\t\tGlyph Data\n";

	for (size_t i = static_cast<size_t>(0ul); i < glm::min(ITERATIONS_TO_PRINT, hbGlyphInfoArraySize); i++) {
		glyphData << "GlyphID: " << hbGlyphInfo[i].codepoint << "\n";
		glyphData << "\tx_offset = " << hbGlyphPosition[i].x_offset / 64.0 << "\ty_offset = " << hbGlyphPosition[i].y_offset / 64.0 << "\n";
		glyphData << "\tx_advance = " << hbGlyphPosition[i].x_advance / 64.0 << "\ty_advance = " << hbGlyphPosition[i].y_advance / 64.0 << "\n\n";
		//glyphData << "Raw: " << hbGlyphPosition[i].y_offset << "\n";

	}

	std::string s = glyphData.str();
	fprintf(MSGLOG, "%s\n", s.c_str());


	return true;
}