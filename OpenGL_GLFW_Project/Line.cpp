//Implementation for the Line class. See header file for more detail.
//Also see Face.h, which is closly related to this class.
//
// Programmer:  Forrest Miller
// Date(s):     Mid-Late October 2018

#include "Line.h"

#include "LoggingMessageTargets.h"

namespace AssetLoadingInternal {

	//Construct the line from existing offset data
	Line::Line(std::array<Offset, 2> endpoints, bool fixIndexing) : mEndpoints_(endpoints) {
		mValidData_ = true;
		
		if (fixIndexing) {
			if (mEndpoints_[0u] != 0u) {  //Can't decrement Offset below 0
				mEndpoints_[0u]--;
			}
			else {
				mValidData_ = false;
			}
			if (mEndpoints_[1u] != 0u) {
				mEndpoints_[1u]--;
			}
			else {
				mValidData_ = false;
			}

			if (!mValidData_) {
				fprintf(ERRLOG, "\nERROR! Unable to fix indexing for line [%u, %u] because\n"
					"an index is already '0'!\n", endpoints[0], endpoints[1]);
			}
		}
	}

	//Construct the line from a line of filetext
	Line::Line(const char * lineLine, bool fixIndexing) {
		mValidData_ = false;
		const char * lineIterator = lineLine;
		//Make sure the line is actually a line
		if ((*lineIterator != 'l') || (*lineIterator != 'L')) { //Sure, why not accept a capital L as well... close enough
			fprintf(ERRLOG, "\nError parsing line %s!\nLine must begin with the character 'l'!\n", lineLine);
			mEndpoints_[0] = 0u;
			mEndpoints_[1] = 0u;
			return;
		}
		else { //Else we read an 'l', so we can advance to the next character
			lineIterator++;
		}

		//The parse here will be simple because the line must be just position data, with no slashes or anything fancy
		int positionsParsed = 0;
		while ((*lineIterator != '\n') && (*lineIterator != '\0')) {
			if (*lineIterator == ' ') {
				lineIterator++;
				break;
			}
			else if (isNumber(lineIterator)) {
				if (isZero(lineIterator)) {
					lineIterator++;
					if (isNumber(lineIterator)) {
						continue;
					}
					else {
						fprintf(ERRLOG, "\nERROR! Parsed an index of 0 from the line %s\nLine indexing is expected to begin at 1!\n",
							lineLine);
						mValidData_ = false;
						mEndpoints_[0] = 0u;
						mEndpoints_[1] = 0u;
						return;
					}
				}
				else {
					if (positionsParsed < LINE_VERTICE_COUNT) {
						mEndpoints_[positionsParsed++] = static_cast<Offset>(strtoul(lineIterator, NULL, 0)); //This is a c function, but it works here just as well
						lineIterator++;
						while (isNumber(lineIterator)) {
							lineIterator++;
						}
					}
				}
			}
		}
		if (positionsParsed != LINE_VERTICE_COUNT) {
			fprintf(ERRLOG, "\nERROR! %d vertices were read from the line %s!\nA line should have exactly %d vertices!\n",
				positionsParsed, lineLine, LINE_VERTICE_COUNT);
			mValidData_ = false;
		}
		else {
			mValidData_ = true;
		}
	}


} //AssetLoadingInternal