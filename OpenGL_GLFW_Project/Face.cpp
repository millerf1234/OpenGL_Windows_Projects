//File:  Face.cpp
//Description: 
//      Implementation for the 'FaceComponent' and 'Face' classes.
//
//
// Programmer:   Forrest Miller
// Date(s):      October 18-23, 2018

#include "Face.h"

#include <cstdlib>  //strtoul  (Used to convert numbers in a string to unsigned long 'Offset')

#include "LoggingMessageTargets.h"

namespace AssetLoadingInternal {

	bool Face::FaceComponent::compareAsTriangles(const FaceComponent& that) const {
		bool equality = (
			(mVertIndices_[0] == that.mVertIndices_[0]) &&
			(mVertIndices_[1] == that.mVertIndices_[1]) &&
			(mVertIndices_[2] == that.mVertIndices_[2]) );
		return equality;
	}

	void Face::initialize() {
		mValidData_ = false;
		mIsQuad_ = false;
		mHasTexCoords_ = false;
		mHasNormals_ = false;
	}

	Face::Face() {
		initialize();
	}

	Face::Face(const char * faceLine, bool fixIndexing) {
		initialize();
		//if (*faceLine != 'f') {
		//	fprintf(ERRLOG, "\nError parsing face from string: %s\n"
		//		"The first character in this string must be 'f'!\n",
		//		faceLine);
		//	return;
		//}
		//else { //else we have read the 'f' char, so chances are pretty good it's a face line
			faceLine++; //Move past the 'f'
		//}

		
		if (!parseFaceLine(faceLine)) { //parseFaceLine handles setting member fields as appropriate
			return;
		}

		if (fixIndexing) {
			convertIndexingToBeginAtZero();			 
		}
	}

	std::array<std::array<Offset, FACE_COMPONENTS_PER_VERTEX>, TRIANGLE_VERTICE_COUNT> Face::getTriangleFace() {
		std::array<std::array<Offset, FACE_COMPONENTS_PER_VERTEX>, TRIANGLE_VERTICE_COUNT> faceData;
		if (!mValidData_) { //If no valid data at all, return all 0's
			faceData[0] = { 0u, 0u, 0u };
			faceData[1] = { 0u, 0u, 0u };
			faceData[2] = { 0u, 0u, 0u };
		}
		else if (!mHasTexCoords_) { 
			if (!mHasNormals_) { 
				faceData[0] = { mPositions_[0], 0u, 0u };
				faceData[1] = { mPositions_[1], 0u, 0u };
				faceData[2] = { mPositions_[2], 0u, 0u };
			}
			else {
				faceData[0] = { mPositions_[0], 0u, mNormals_[0] };
				faceData[1] = { mPositions_[1], 0u, mNormals_[1] };
				faceData[2] = { mPositions_[2], 0u, mNormals_[2] };
			}
		}
		else { //mHasTexCoords_ is true
			if (!mHasNormals_) {
				faceData[0] = { mPositions_[0], mTexCoords_[0], 0u };
				faceData[1] = { mPositions_[1], mTexCoords_[1], 0u };
				faceData[2] = { mPositions_[2], mTexCoords_[2], 0u };
			}
			else { //All 3 component types are present
				faceData[0] = { mPositions_[0], mTexCoords_[0], mNormals_[0] };
				faceData[1] = { mPositions_[1], mTexCoords_[1], mNormals_[1] };
				faceData[2] = { mPositions_[2], mTexCoords_[2], mNormals_[2] };
			}
		}
		return faceData;
	}

	std::array<std::array<Offset, FACE_COMPONENTS_PER_VERTEX>, QUAD_VERTICE_COUNT> Face::getQuadFace() {
		std::array<std::array<Offset, FACE_COMPONENTS_PER_VERTEX>, QUAD_VERTICE_COUNT> faceData;
		if (!mValidData_) { //If no valid data at all, return all 0's
			faceData[0] = { 0u, 0u, 0u };
			faceData[1] = { 0u, 0u, 0u };
			faceData[2] = { 0u, 0u, 0u };
			faceData[3] = { 0u, 0u, 0u };
		}
		else if (!mHasTexCoords_) {
			if (!mHasNormals_) {
				faceData[0] = { mPositions_[0], 0u, 0u };
				faceData[1] = { mPositions_[1], 0u, 0u };
				faceData[2] = { mPositions_[2], 0u, 0u };
				faceData[3] = { mPositions_[3], 0u, 0u };
			}
			else {
				faceData[0] = { mPositions_[0], 0u, mNormals_[0] };
				faceData[1] = { mPositions_[1], 0u, mNormals_[1] };
				faceData[2] = { mPositions_[2], 0u, mNormals_[2] };
				faceData[3] = { mPositions_[3], 0u, mNormals_[3] };
			}
		}
		else { //mHasTexCoords_ is true
			if (!mHasNormals_) {
				faceData[0] = { mPositions_[0], mTexCoords_[0], 0u };
				faceData[1] = { mPositions_[1], mTexCoords_[1], 0u };
				faceData[2] = { mPositions_[2], mTexCoords_[2], 0u };
				faceData[3] = { mPositions_[3], mTexCoords_[3], 0u };
			}
			else { //All 3 component types are present
				faceData[0] = { mPositions_[0], mTexCoords_[0], mNormals_[0] };
				faceData[1] = { mPositions_[1], mTexCoords_[1], mNormals_[1] };
				faceData[2] = { mPositions_[2], mTexCoords_[2], mNormals_[2] };
				faceData[3] = { mPositions_[3], mTexCoords_[3], mNormals_[3] };
			}
		}
		return faceData;
	}

	//Implementation note: This logic is a little sloppy due to the number of different cases that
	//                     need to be handled. This could be cleaned up a bit probably.
	bool Face::operator==(const Face& other) const {
		//Try to rule out situations where inequality is impossible
		if (this == &other) { return true; } //Check for self-comparison
		if ((!this->mValidData_) && (!other.mValidData_)) {
			return TREAT_INVALID_FACES_AS_EQUIVALENT; //return value is defined in header
		}
		else if (this->mIsQuad_ != other.mIsQuad_) { //If one is a quad and one is a triangle
			return false;
		}
		else if ((this->mHasNormals_ != other.mHasNormals_) ||
			(this->mHasTexCoords_ != other.mHasTexCoords_)) {
			return false;
		}

		else { //Else we were unable to quickly rule out inequality, so compare each Face's components.

			//Need to treat comparison between two quad-faces and two triangle-faces as seperate cases. 
			if (this->mIsQuad_) { //If we have quads, then can do straight comparison
				if (this->mPositions_ != other.mPositions_) {
					return false;
				}
				else if (this->mHasTexCoords_ &&
					(this->mTexCoords_ != other.mTexCoords_)) {
					return false;
				}
				else if (this->mHasNormals_ &&
					(this->mNormals_ != other.mNormals_)) {
					return false;
				}
				else { //None of the comparisons showed inequality, so both faces must be equal
					return true;
				}
			}

			else { //Else if they are both triangles, the special triangle-comparison function is used
				if (this->mPositions_.compareAsTriangles(other.mPositions_)) {
					return false;
				}
				else if (this->mHasTexCoords_ &&
					(this->mTexCoords_.compareAsTriangles(other.mTexCoords_))) {
					return false;
				}
				else if (this->mHasNormals_ &&
					(this->mNormals_.compareAsTriangles(other.mNormals_))) {
					return false;
				}
				else { //None of the comparisons showed inequality, so both faces must be equal
					return true;
				}
			}
		}
	}


	//parseFaceLine() will set member fields 
	bool Face::parseFaceLine(const char * faceLine) {
		//Arrays to hold the data as it is parsed by this function. After the line is fully parsed, these
		//will be passed to the function fillFaceComponents() which sets this object's FaceComponent fields
		Offset positions[QUAD_VERTICE_COUNT], texCoord[QUAD_VERTICE_COUNT], normals[QUAD_VERTICE_COUNT];
		//The following variables are used to track the state of the parsing as the parse loop operates.
		enum class NextComponentType { POSITION, TEX_COORD, NORMAL };
		NextComponentType next = NextComponentType::POSITION; //The first value read will be a position value. 
		bool previousCharWasASlash = false; //Used to determine where in string we are
		int readPositions = 0; //By the end this number must be either 3 or 4.  Otherwise invalid parse occurred
		int readTexCoords = 0; //By end, this number must be either 0, 3, or 4.        "           "
		int readNormals = 0;  //By end, this number must be either 0, 3, or 4.         "           "
		bool parseError = false;

		const char * stringIter = faceLine; //Iterator for the parse loop

		//Some '.obj' files contain only position data and no forward slashes. These will be parsed separately
		if (checkForNoForwardSlashes(stringIter)) { 
			//reset stringIter
			stringIter = faceLine;
			//Do a simplified parse loop to extract position data only
			mHasTexCoords_ = false;
			mHasNormals_ = false;
			parseError = !(parseLineWithNoForwardSlashes(stringIter, positions, readPositions)); //readPositions is passed by reference
			if (readPositions == QUAD_VERTICE_COUNT) {
				mIsQuad_ = true;
			}
		}
		else {
			//reset stringIter
			stringIter = faceLine;
			//The full parse loop:
			while ((*stringIter != '\n') && (*stringIter != '\0') && (!parseError)) {
				////////////////////////////////////////////////////////////////////////////////////////
				//////////   A valid ".OBJ" face line should always take one of the following paths ////
				//////////    (as opposed to the invalid paths marked near the bottom of this loop) ////
				////////////////////////////////////////////////////////////////////////////////////////
				if (*stringIter == ' ') { //If we read a space
					previousCharWasASlash = false;
					next = NextComponentType::POSITION; //Reading a space means we automatically revert to looking for a position
				}
				else if (*stringIter == '/') {
					if (!previousCharWasASlash) {
						previousCharWasASlash = true; //Mark that we read a slash for the next loop iteration
						switch (next) {
						case NextComponentType::POSITION:
							fprintf(ERRLOG, "\nERROR! Expected to read a position value but this face has no position!\n");
							parseError = true;
							continue;
						case NextComponentType::TEX_COORD:
							break;
						case NextComponentType::NORMAL:
							break;
						}
					}
					else { //This is the second slash in a row we encounter
						switch (next) {
						case NextComponentType::POSITION:  //This case shouldn't happen ever unless file is corrupt or my logic is off
							fprintf(ERRLOG, "\nA parse error occured involving the enum 'NextComponentType' in the Face class!\n"
								"Parser is for some reason expecting to read a position after 2 slashes, which is not correct.\n"
								"The line that caused this issue is: \"f%s\"\n", faceLine);
							parseError = true;
							continue;
						case NextComponentType::TEX_COORD: //If we hit a second slash while looking for a tex coord, then
							next = NextComponentType::NORMAL; //there must be no texture coordinates. Thus we can move on to 
							break;                           //looking for a normal.
						case NextComponentType::NORMAL: //If we hit a second slash while looking for a normal, then the file is corrupt/invalid.
							fprintf(ERRLOG, "\nERROR: A '/' was encountered while trying to read a normal! This line is ill-formed!\n");
							parseError = true;
							continue;
						}
					}
				}

				else if (isNumber(stringIter)) {
					if (isZero(stringIter)) { //Check to see if number is a 0
						const char * next = stringIter + 1; //Check to see what the next character is
						if (isNumber(next)) { //If number is written like "01", this technically is still valid, if inadvised
							stringIter = next;
							fprintf(WRNLOG, "\nWARNING! A number starting with '0' was read from the faceline:\n\t\"f%s\"\n"
								"While this is technically valid, please ensure that this is truly what was meant.\n", faceLine);
							continue;
						}
						else {
							fprintf(ERRLOG, "Error! Face indexing begins at 1, but an index of '0' was \n"
								"read from the line: \n\t\"f%s\"\n", faceLine);
							continue;
						}
					}
					if (next == NextComponentType::POSITION) {
						next = NextComponentType::TEX_COORD;
						if (readPositions < QUAD_VERTICE_COUNT) {
							positions[readPositions++] = static_cast<Offset>(strtoul(stringIter, NULL, 0)); //This is a c function, but it works here just as well
							do {
								stringIter++;
							} while (isNumber(stringIter));
							//Let's do a quick (and perhaps unnecessary) reality check. The string iterator should now be on a '/' character.
							if (*stringIter != '/') {
								parseError = true;
							}
							if (readPositions == QUAD_VERTICE_COUNT) {
								mIsQuad_ = true;
							}
						}
						else {
							//This face is an ngon, which would result in an invalid face
                            parseError = true;
						}
					}
					else if (next == NextComponentType::TEX_COORD) {
						if (!previousCharWasASlash) {
							parseError = true;
						}
						else {
							previousCharWasASlash = false;
							next = NextComponentType::NORMAL;
							if (readTexCoords < QUAD_VERTICE_COUNT) {
								texCoord[readTexCoords++] = static_cast<Offset>(strtoul(stringIter, NULL, 0));
								do {
									stringIter++;
								} while (isNumber(stringIter));
								//Let's do a quick (and perhaps unnecessary) reality check. The string iterator should now be on a '/' character.
								if (*stringIter != '/') {
									parseError = true;
								}
							}
							else {
								fprintf(ERRLOG, "\nError! Too many texture coordinates (over %d) were read!\n", QUAD_VERTICE_COUNT);
								parseError = true;
							}
						}
					}
					else { //(next component type must be NORMAL)
						next = NextComponentType::POSITION;
						if (readNormals < QUAD_VERTICE_COUNT) {
							normals[readNormals++] = static_cast<Offset>(strtoul(stringIter, NULL, 0));
							do {
								stringIter++;
							} while (isNumber(stringIter));
						}
						else {
							fprintf(ERRLOG, "\nError! Too many per-vertex Normals (over %d) were read!\n", QUAD_VERTICE_COUNT);
							parseError = true;
						}
					}
					continue;
				}
				////////////////////////////////////////////////////////////////////////////////////////
				//////////   Any of these following paths will always result in an invalid parse    ////
				////////////////////////////////////////////////////////////////////////////////////////
				else if (*stringIter == '-') {
					fprintf(ERRLOG, "\nUH-OH! ERROR! A '-' was encountered for defining a face within this \".obj\" file!\n");
					parseError = true;
					continue;
				}
				else if ((*stringIter >= 'a') && (*stringIter <= 'z')) {
					fprintf(WRNLOG, "\nUh-Oh! It looks like the Face parser went on to a new line!\n"
						"Here is the input provided to this Face constructor:\n\tf%s\n", faceLine);
					parseError = true;
					continue;
				}
				stringIter++;
			}
			//End of parsing 'while' loop 
		}
		if (!parseError) {
			//Perform a check on each parsed dataset to make sure the correct amount of data was parsed.
			if (!verifyPositions(readPositions, faceLine)) {
				mValidData_ = false;
			}
			else {
				mValidData_ = true;
			}

			if (!verifyTexCoords(readTexCoords, faceLine)) {
				mHasTexCoords_ = false;
			}
			else {
				mHasTexCoords_ = true;
			}

			if (!verifyNormals(readNormals, faceLine)) {
				mHasNormals_ = false;
			}
			else {
				mHasNormals_ = true;
			}

			if (!mHasNormals_) { normals[0] = 0u; normals[1] = 0u; normals[2] = 0u; }
			if (!mHasTexCoords_) { texCoord[0] = 0u; texCoord[1] = 0u; texCoord[2] = 0u; }

			/*
			/////////////////////////////////////////////////
			//print a message for debug purposes before returning...
			fprintf(MSGLOG, "\nFace Parsed! %u characters read from line:\n\tf%s",
				std::distance(faceLine, stringIter) + 1, faceLine); //The '+ 1' is for the 'f' that gets removed before this function is called
			fprintf(MSGLOG, "Data stored in this object:\n"
				"Valid Data: %d\thasTexCoord: %d\thasNormals: %d\n\"Values: %u/%u/%u %u/%u/%u/ %u/%u/%u\"\n", mValidData_,
				mHasTexCoords_, mHasNormals_, positions[0], texCoord[0], normals[0], positions[1], texCoord[1], normals[1],
				positions[2], texCoord[2], normals[2]);
			/////////////////////////////////////////////////
			*/
			fillFaceComponents(positions, texCoord, normals);
			if (mValidData_) {
				return true; //Valid data, no Parse Error
			}
			else {
				fprintf(ERRLOG, "\nERROR! It looks like not enough data was parsed from the line:\n\t\"%s\"\n", faceLine);
				return false;
			}
		}
		else {
            fprintf(MSGLOG, "\nNGon detected in main parse function, skipping this line...\n");
            return false; 
			//fprintf(ERRLOG, "\nERROR parsing face! %u characters read from line\n"
			//	"before error! Text that was parsed:\t\"f%s\"\n",
			//	(std::distance(faceLine, stringIter) + 1), faceLine); //The '+ 1' is for the 'f' that gets removed early on
		}
		return parseError;
	}

	bool Face::checkForNoForwardSlashes(const char * stringIter) const {
		while ((*stringIter != '\n') && (*stringIter != '\0')) {
			if (*stringIter == '/') {
				return false;
			}
			else {
				stringIter++;
			}
		}
		return true;
	}

	bool Face::parseLineWithNoForwardSlashes(const char * stringIter, Offset * positionData, int & readPositions) const {
		while ((*stringIter != '\n') && (*stringIter != '\0')) {
			if (*stringIter == ' ') {
				stringIter++;
				continue;
			}
			else if (isNumber(stringIter)) {
				if (isZero(stringIter)) { //Check to see if number is a 0
					const char * next = stringIter + 1; //Check to see what the next character is
					if (isNumber(next)) { //If number is written like "01", this technically is still valid, if inadvised
						stringIter = next; //Just skip over the 0 if there is a number after it
						continue;
					}
					else {
						fprintf(ERRLOG, "Error! Face indexing begins at 1, but an index of '0' was \n"
							"read from the line!\n");
						return false;
					}
				}
				else {
					if (readPositions < QUAD_VERTICE_COUNT) {
						positionData[readPositions++] = static_cast<Offset>(strtoul(stringIter, NULL, 0)); //This is a c function, but it works here just as well
						do {
							stringIter++; //Move the iterator to the end of the number
						} while (isNumber(stringIter));
					}
					else {
						fprintf(ERRLOG, "\nError! Too many positions were read!\n");
						return false;
					}
				}
			}
		}
		return true;
	}

	bool Face::verifyPositions(const int & parsedPositions, const char * faceLine) const {
		if (parsedPositions < TRIANGLE_VERTICE_COUNT) {
			fprintf(ERRLOG, "\nError parsing face! Only %d position indices were read for this face!\n"
				"There must be at least 3 position indices present to form a valid face!\n"
				"The line that is cause issues looks like: \n\tf%s\n", parsedPositions, faceLine);
			return false;
		}
		else if (parsedPositions > QUAD_VERTICE_COUNT) {
			fprintf(ERRLOG, "\nError parsing file! More than 4 positions were read!\n"
				"%d 'positions' (i.e. <x,y,z>) were read in total. The line that caused\n"
				"this issue looks like:\n\tf%s\n",
				parsedPositions, faceLine);
			return false;
		}
		else if (parsedPositions == TRIANGLE_VERTICE_COUNT && mIsQuad_) {
			return false;
		}
		else {
			return true;
		}
	}

	bool Face::verifyTexCoords(const int & parsedTexCoord, const char * faceLine) const {
		if (parsedTexCoord == 0)
			return false;
		else if ((parsedTexCoord == TRIANGLE_VERTICE_COUNT) && (!mIsQuad_))
			return true;
		else if ((parsedTexCoord == QUAD_VERTICE_COUNT) && (mIsQuad_))
			return true;
		else {
			fprintf(ERRLOG, "\nAn issue occured parsing the texture coordinates from the line:\n\t\"%s\"\n"
				"(The texture coordinates are the values between the first and second slashes).\n",
				faceLine);
			return false;
		}
	}

	bool Face::verifyNormals(const int & parsedNormals, const char * faceLine) const {
		if (parsedNormals == 0)
			return false;
		else if ((parsedNormals == TRIANGLE_VERTICE_COUNT) && (!mIsQuad_))
			return true;
		else if ((parsedNormals == QUAD_VERTICE_COUNT) && (mIsQuad_))
			return true;
		else {
			fprintf(ERRLOG, "\nAn issue occured parsing the Normals from the line:\n\t\"%s\"\n"
				"(The Normals are the values after the second slash in each vertex).\n",
				faceLine);
			return false;
		}
	}

	//This function fills this object's FaceComponent members from arrays of data
	//provided as parameters. It is assumed that position data is valid, and that 
	//there is enough data in each array  
	void Face::fillFaceComponents(Offset* positions, Offset* texCoord, Offset* normals) {
		if (!mIsQuad_) {
			mPositions_ = FaceComponent(positions[0], positions[1], positions[2]);
			if (mHasTexCoords_) {
				mTexCoords_ = FaceComponent(texCoord[0], texCoord[1], texCoord[2]);
			}
			if (mHasNormals_) {
				mNormals_ = FaceComponent(normals[0], normals[1], normals[2]);
			}
			mValidData_ = true;
		}
		else {
			mPositions_ = FaceComponent(positions[0], positions[1], positions[2], positions[3]);
			if (mHasTexCoords_) {
				mTexCoords_ = FaceComponent(texCoord[0], texCoord[1], texCoord[2], texCoord[3]);
			}
			if (mHasNormals_) {
				mNormals_ = FaceComponent(normals[0], normals[1], normals[2], normals[3]);
			}
			mValidData_ = true;
		}
	}
	
	//This code here doesn't work!
	void Face::convertIndexingToBeginAtZero() {
		if (mValidData_) {
			mPositions_[0] -= 1u;
			mPositions_[1] -= 1u;
			mPositions_[2] -= 1u;
			if (mIsQuad_)
				mPositions_[3] -= 1u;
			if (mHasTexCoords_) {
				mTexCoords_[0] -= 1u;
				mTexCoords_[1] -= 1u;
				mTexCoords_[2] -= 1u;
				if (mIsQuad_)
					mTexCoords_[3] -= 1u;
			}
			if (mHasNormals_) {
				mNormals_[0] -= 1u;
				mNormals_[1] -= 1u;
				mNormals_[2] -= 1u;
				if (mIsQuad_)
					mNormals_[3] -= 1u;
			}
		}

		//if (mValidData_) {
		//	auto positions = mPositions_.get();
		//	(positions[0]) -= 1u;
		//	(positions[1]) -= 1u;
		//	(positions[2]) -= 1u;
		//	if (mIsQuad_)
		//		positions[3] -= 1u;
		//
		//	if (mHasTexCoords_) {
		//		auto textureCoords = mTexCoords_.get();
		//		textureCoords[0] -= 1u;
		//		textureCoords[1] -= 1u;
		//		textureCoords[2] -= 1u;
		//		if (mIsQuad_)
		//			textureCoords[3] -= 1u;
		//	}
		//	
		//	if (mHasNormals_) {
		//		auto normals = mNormals_.get();
		//		normals[0] -= 1u;
		//		normals[1] -= 1u;
		//		normals[2] -= 1u;
		//		if (mIsQuad_)
		//			normals[3] -= 1u;
		//	}
		//}
	}

} //namespace AssetLoadingInternal