//File:  Face.cpp
//Description: 
//      Implementation for the 'FaceComponent' and 'Face' classes.
//      It turns out the 'FaceComponent' class fits snuggly in 
//      the header.
//
//
// Programmer:   Forrest Miller
// Date:         October 18, 2018
//
//
//  Side Note:  This code is written pretty slopily, it should not be used for 
//              an example of what the code I typically write looks like.

#include "Face.h"

namespace AssetLoadingInternal {

	Face::FaceComponent::~FaceComponent() { ; }
	Face::~Face() { ; }

	void Face::initialize() {
		mValidData_ = false;
		mIsQuad_ = false;
		mHasTexCoords_ = false;
		mHasNormals_ = false;
	}

	Face::Face() {
		initialize();
		mPositions_ = FaceComponent();
		mTexCoords_ = FaceComponent();
		mNormals_ = FaceComponent();
		/*
		  //This class implements logic to prevent inadvertant access to uninitialized
		  //data so it is unnecessary to initialize all of these member FaceComponent
		  //objects
		Offset maxOffset = std::numeric_limits<std::size_t>::max();
		mPositions_= FaceComponent(maxOffset, maxOffset, maxOffset);
		mTexCoords_ = FaceComponent(maxOffset, maxOffset, maxOffset);
		mNormals_ = FaceComponent(maxOffset, maxOffset, maxOffset);
		*/
	}

	Face::Face(const char * faceLine, bool fixIndexing) {
		initialize();
		if (*faceLine != 'f') {
			fprintf(ERRLOG, "\nError parsing face from string: %s\n"
				"The first character in this string must be 'f'!\n",
				faceLine);
			return;
		}
		else { //else we have read the 'f' char, so chances are pretty good it's a face line
			faceLine++; //Move past the 'f'
		}

		
		if (!parseFaceLine(faceLine)) { //parseFaceLine handles setting member fields as appropriate
			return;
		}

		if (fixIndexing) {
			auto posIndicies = mPositions_.get();
		}

	}

	//Implementation note: This logic is a little sloppy due to the number of different cases that
	//                     need to be handled. This could be cleaned up a bit probably.
	bool Face::operator==(const Face& other) const {
		//Try to rule out situations where inequality is impossible
		if (this == &other) { return true; } //Check for self-comparison
		if ((!this->mValidData_) && (!other.mValidData_)) {
			return INVALID_FACES_HAVE_EQUIVALENCE; //return value is defined in header
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
		//The following variables are used to facilitate parsing the line of text
		enum class NextComponentType { POSITION, TEX_COORD, NORMAL };
		NextComponentType next = NextComponentType::POSITION; //The first value read will be a position value. 
		bool previousCharWasASlash = false; //Used to determine where in string we are
		Offset positions[QUAD_VERTICE_COUNT], texCoord[QUAD_VERTICE_COUNT], normals[QUAD_VERTICE_COUNT];
		int readPositions = 0; //By the end this number must be either 3 or 4.  Otherwise invalid parse occured
		int readTexCoords = 0; //By end, this number must be either 0, 3, or 4.        "           "
		int readNormals = 0;  //By end, this number must be either 0, 3, or 4.         "           "
		bool parseError = false;

		const char * stringIter = faceLine;
		while ((*stringIter != '\n') && (*stringIter != '\0') && (!parseError)) {
			////////////////////////////////////////////////////////////////////////////////////////
			//////////   A Valid ".OBJ" face line should always take one of the following paths ////
			//////////    (as opposed to the invalid paths marked near the bottom of this loop) ////
			////////////////////////////////////////////////////////////////////////////////////////
			if (*stringIter == ' ') {
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
						break;//                          //looking for a normal.
					case NextComponentType::NORMAL: //If we hit a second slash while looking for a normal, then the file is corrupt/invalid.
						fprintf(ERRLOG, "\nERROR: A '/' was encountered while trying to read a normal! This line is ill-formed!\n");
						parseError = true;
						continue;
					}
				}
			}

			else if (isNumber(stringIter)) {
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
						fprintf(ERRLOG, "\nError! Too many positions were read!\n");
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
			else if ( (*stringIter >= 'a') && (*stringIter <= 'z') ) {
				fprintf(WRNLOG, "\nUh-Oh! It looks like the Face parser went on to a new line!\n"
					"Here is the input provided to this Face constructor:\n\tf%s\n", faceLine);
				parseError = true;
				continue;
			}
			stringIter++;
		} //End of while loop 

		if (!parseError) {
			//Perform a check on each parsed dataset to make sure the correct amount of data was parsed.
			if (!verifyPositions(readPositions, faceLine)) {
				mValidData_ = false;
			}
			if (!verifyTexCoords(readTexCoords, faceLine)) {
				mHasTexCoords_ = false;
			}
			if (!verifyNormals(readNormals, faceLine)) {
				//mValidData = false;
				mHasNormals_ = false;
			}

			//print a message for debug purposes before returning...
			fprintf(MSGLOG, "Face Parsed! %u characters read from line \"f%s\"\n",
				std::distance(faceLine, stringIter) + 1, faceLine); //The '+ 1' is for the 'f' that gets removed early on
			if (!mHasNormals_) { normals[0] = 0u; normals[1] = 0u; normals[2] = 0u; }
			if (!mHasTexCoords_) { texCoord[0] = 0u; texCoord[1] = 0u; texCoord[2] = 0u; }
			fprintf(MSGLOG, "Line Reconstruction: \"f %u/%u/%u %u/%u/%u/ %u/%u/%u\"\n", positions[0], texCoord[0], normals[0], positions[1], texCoord[1], normals[1], positions[2], texCoord[2], normals[2]);
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
			fprintf(ERRLOG, "\nERROR Parsing Face! %u characters read from line\n"
				"before error! Text that was parsed:\t\"f%s\"\n",
				(std::distance(faceLine, stringIter) + 1), faceLine); //The '+ 1' is for the 'f' that gets removed early on
		}
		return parseError;
	}


	bool Face::verifyPositions(const int & parsedPositions, const char * faceLine) const {
		if (parsedPositions < TRIANGLE_VERTICE_COUNT) {
			fprintf(ERRLOG, "\nError parsing file! Only %d position indices were read for this face!\n"
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
		else if ((parsedTexCoord == 3) && (!mIsQuad_))
			return true;
		else if ((parsedTexCoord == 4) && (mIsQuad_))
			return true;
		else {
			fprintf(ERRLOG, "\nAn issue occured parsing the TexCoords from the line:\n\t\"%s\"\n",
				faceLine);
			return false;
		}
	}

	bool Face::verifyNormals(const int & parsedNormals, const char * faceLine) const {
		if (parsedNormals == 0)
			return true;
		else if ((parsedNormals == 3) && (!mIsQuad_))
			return true;
		else if ((parsedNormals == 4) && (mIsQuad_))
			return true;
		else {
			fprintf(ERRLOG, "\nAn issue occured parsing the Normals from the line:\n\t\"%s\"\n",
				faceLine);
			return false;
		}
	}

	//This function assumes the three verifaction functions have already run and returned true.
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
	


} //namespace AssetLoadingInternal