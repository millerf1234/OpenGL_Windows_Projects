#include "QuickObj.h"

QuickObj::QuickObj(std::string filepath, float scale) {
	mScale_ = scale;
	mHasTexCoords_ = false;
	mHasNormals_ = false;
	mFile_ = std::make_unique<AssetLoadingInternal::AsciiAsset>(filepath);

	if (mFile_->getStoredTextLength() > 0u) {
		//preparseFile(); //This is unnecessary 
		parseFile();
	}
	else {
		fprintf(ERRLOG, "\nERROR aquiring file!\n");
	}
}


QuickObj::~QuickObj() {

}


//This is a very quick and dirty implementation. Not clean
void QuickObj::parseFile() {
	//In case a line starting with vp is read (representing freeform geometry),
	//we only need to print out a warning one time
	bool printedFreeformWarning = false; 
	size_t fileSize = mFile_->getNumberOfLines();
	for (size_t i = 0; i < fileSize; i++) {
		std::string line = mFile_->getLine(i);
		const char * lineIter = line.c_str();

		//Skip object tags, group tags and smooth group tags (because this is just the SimpleObj loader after all)
		if ((*lineIter == 'o') || (*lineIter == 'g') || (*lineIter == 's')) {
			continue;
		}

		else if (*lineIter == 'v') {
			//bool isNegative = false; //used to detect negative numbers //UNUSED
			lineIter++;
			if (*lineIter == ' ') {
				eatWhitespace(lineIter);
				loadLineIntoVertex(lineIter, mPositions_);
			}
			else if (*lineIter == 't') {
				mHasTexCoords_ = true;
				lineIter++; //move past the 't'
				eatWhitespace(lineIter);
				loadLineIntoVertex(lineIter, mTexCoords_);
			}
			else if (*lineIter == 'n') {
				mHasNormals_ = true;
				lineIter++;
				eatWhitespace(lineIter);
				loadLineIntoVertex(lineIter, mNormals_);
			}
			else if (*lineIter == 'p') {
				if (!printedFreeformWarning) {
					fprintf(MSGLOG, "\n\nWhoah! Freeform geometry data encountered! This is quite unexpected!\n"
						"It looks like you are trying to load an Advanced OBJ file.\n"
						"Unfortunately freeform geometry is not supported at this time...\n"
						"\t  [All freeform geometry in this file will be skipped]\n");
					printedFreeformWarning = true;
				}
				continue;
			}
			else {
				fprintf(ERRLOG, "\nERROR parsing line %s\n", line.c_str());
			}
		}
		else if (*lineIter == '#') {
			//Skip comments for now...
			continue;
		}

		else if (*lineIter == 'f') {
			mFaces_.emplace_back(AssetLoadingInternal::Face(lineIter, true));
		}
		else if (*lineIter == 'l') {
			mLines_.emplace_back(AssetLoadingInternal::Line(lineIter, true));
		}
		else if (*lineIter == 'm') {
			if (*(lineIter + 1u) == 'g') {
				//It's a merging group, which we will skip
				fprintf(MSGLOG, "Skipping Merging Group: %s", line.c_str());
			}
			else {
				//No material loading (for now)...
				fprintf(MSGLOG, "Skipping Material: %s", line.c_str());
			}
			continue;
		}
		else if (*lineIter == 'u') { //Line probably is a "usemtl ____"  instruction
			//No material using for now
			continue;
		}
		else {
			fprintf(ERRLOG, "Ignoring line: %s\n", line.c_str());
		}
	}

	if (mPositions_.size() > 0) {
		constructVerticesFromParsedData();
	}
	else {
		fprintf(ERRLOG, "\nERROR! Unable to parse file %s!\n", mFile_->getFilepath().c_str());
	}
}

//
//void QuickObj::preparseFile() {
//	//Remove comments
//	mFile_->removeLinesBeginningWithCharacter('#');
//
//	//I know this is wrong, each vector is way too big:
//	size_t dataSize = mFile_->getNumberOfLinesThatBeginWith('v'); //I forgot to add a function that checks the character after the 'v'
//	mPositions_.reserve(dataSize);
//	mTexCoords_.reserve(dataSize);
//	mNormals_.reserve(dataSize);
//
//	//These next 2 are correct though
//	mFaces_.reserve(mFile_->getNumberOfLinesThatBeginWith('f'));
//	mLines_.reserve(mFile_->getNumberOfLinesThatBeginWith('l'));
//}


void QuickObj::constructVerticesFromParsedData() {
	int triangleFaces = 0;
	int quadFaces = 0;
	int lines = mLines_.size();
	for (auto faceIter = mFaces_.begin(); faceIter != mFaces_.end(); faceIter++) {
		if (faceIter->isQuad()) {
			quadFaces++;
		}
		else {
			triangleFaces++;
		}
	}
	fprintf(MSGLOG, "\nParsed  Lines: %d\tTriangleFaces: %d\tQuadFaces: %d\n", lines, triangleFaces, quadFaces);
	fprintf(MSGLOG, "Parsed  Positions: %d\tTexCoords: %d\tNormals: %d\n", mPositions_.size(), mTexCoords_.size(), mNormals_.size());

	if (lines > 0) {
		fprintf(MSGLOG, "\n  [TODO: Implement the converting of parsed 'line' primitive data to GPU-ready data...]\n");
	}

	//Construct each face from the parsed data into the mVertices_ vector, with positions/textureCoords/Normals interlaced
	for (auto faceIter = mFaces_.begin(); faceIter != mFaces_.end(); faceIter++) {
		if (faceIter->isQuad()) { //Quads will be triangulated here:
			auto offsets = faceIter->getQuadFace(); //Get the quad face (note 'offsets' is an array of arrays)
			auto quadCorner0 = offsets[0]; //Get the quad's 4 corners
			auto quadCorner1 = offsets[1];
			auto quadCorner2 = offsets[2];
			auto quadCorner3 = offsets[3];

			//If we just have to worry about position data since there was no texture or normal data:
			if (!(faceIter->hasTexCoord() || faceIter->hasNormals())) {

				//Triangle 1
				auto qc0_pos = mPositions_[quadCorner0[POSITION_INDEX]]; //quad corner 0 _ positions
				mVertices_.push_back(qc0_pos[0]); //corner0 x
				mVertices_.push_back(qc0_pos[1]); //corner0 y
				mVertices_.push_back(qc0_pos[2]); //corner0 z
				mVertices_.push_back(mScale_);    //zoom / w component of position

				auto qc1_pos = mPositions_[quadCorner1[POSITION_INDEX]]; //quad corner 1 _ positions
				mVertices_.push_back(qc1_pos[0]); //corner1 x
				mVertices_.push_back(qc1_pos[1]); //corner1 y
				mVertices_.push_back(qc1_pos[2]); //corner1 z
				mVertices_.push_back(mScale_);   //zoom

				auto qc2_pos = mPositions_[quadCorner2[POSITION_INDEX]]; //quad corner 2 _ positions
				mVertices_.push_back(qc2_pos[0]); //corner2 x
				mVertices_.push_back(qc2_pos[1]); //corner2 y
				mVertices_.push_back(qc2_pos[2]); //corner2 z
				mVertices_.push_back(mScale_);    //zoom / w component of position

				//Triangle 2
				mVertices_.push_back(qc2_pos[0]); //corner2 x
				mVertices_.push_back(qc2_pos[1]); //corner2 y
				mVertices_.push_back(qc2_pos[2]); //corner2 z
				mVertices_.push_back(mScale_);    //zoom / w component of position


				mVertices_.push_back(qc1_pos[0]); //corner1 x
				mVertices_.push_back(qc1_pos[1]); //corner1 y
				mVertices_.push_back(qc1_pos[2]); //corner1 z
				mVertices_.push_back(mScale_);    //zoom 

				auto qc3_pos = mPositions_[quadCorner3[POSITION_INDEX]]; //quad corner 3 _ positions
				mVertices_.push_back(qc3_pos[0]); //corner3 x
				mVertices_.push_back(qc3_pos[1]); //corner3 y
				mVertices_.push_back(qc3_pos[2]); //corner3 z
				mVertices_.push_back(mScale_);    //zoom / w component of position

			}



			else if (faceIter->hasTexCoord()) {
				if (!(faceIter->hasNormals())) { //Only texture coords, no normals

					//Triangle 1
					auto qc0_pos = mPositions_[quadCorner0[POSITION_INDEX]]; //quad corner 0 _ positions
					mVertices_.push_back(qc0_pos[0]); //corner0 x
					mVertices_.push_back(qc0_pos[1]); //corner0 y
					mVertices_.push_back(qc0_pos[2]); //corner0 z
					mVertices_.push_back(mScale_);    //zoom / w component of position
					auto qc0_tex = mTexCoords_[quadCorner0[TEXTURE_COORD_INDEX]]; //quad corner 0 _ texture coords
					mVertices_.push_back(qc0_tex[0]); //Corner 0 tex coord s
					mVertices_.push_back(qc0_tex[1]); //Corner 0 tex coord t


					auto qc1_pos = mPositions_[quadCorner1[POSITION_INDEX]]; //quad corner 1 _ positions
					mVertices_.push_back(qc1_pos[0]); //corner1 x
					mVertices_.push_back(qc1_pos[1]); //corner1 y
					mVertices_.push_back(qc1_pos[2]); //corner1 z
					mVertices_.push_back(mScale_);   //zoom
					auto qc1_tex = mTexCoords_[quadCorner1[TEXTURE_COORD_INDEX]]; //quad corner 1 _ texture coords
					mVertices_.push_back(qc1_tex[0]); //Corner 1 tex coord s
					mVertices_.push_back(qc1_tex[1]); //Corner 1 tex coord t


					auto qc2_pos = mPositions_[quadCorner2[POSITION_INDEX]]; //quad corner 2 _ positions
					mVertices_.push_back(qc2_pos[0]); //corner2 x
					mVertices_.push_back(qc2_pos[1]); //corner2 y
					mVertices_.push_back(qc2_pos[2]); //corner2 z
					mVertices_.push_back(mScale_);    //zoom / w component of position
					auto qc2_tex = mTexCoords_[quadCorner2[TEXTURE_COORD_INDEX]]; //quad corner 2 _ texture coords
					mVertices_.push_back(qc2_tex[0]); //Corner 2 tex coord s
					mVertices_.push_back(qc2_tex[1]); //Corner 2 tex coord t



					//Triangle 2
					mVertices_.push_back(qc2_pos[0]); //corner2 x
					mVertices_.push_back(qc2_pos[1]); //corner2 y
					mVertices_.push_back(qc2_pos[2]); //corner2 z
					mVertices_.push_back(mScale_);    //zoom / w component of position
					mVertices_.push_back(qc2_tex[0]); //Corner 2 tex coord s
					mVertices_.push_back(qc2_tex[1]); //Corner 2 tex coord t

					mVertices_.push_back(qc1_pos[0]);
					mVertices_.push_back(qc1_pos[1]);
					mVertices_.push_back(qc1_pos[2]);
					mVertices_.push_back(mScale_);
					mVertices_.push_back(qc1_tex[0]); //Corner 1 tex coord s
					mVertices_.push_back(qc1_tex[1]); //Corner 1 tex coord t

					auto qc3_pos = mPositions_[quadCorner3[POSITION_INDEX]]; //quad corner 3 _ positions
					mVertices_.push_back(qc3_pos[0]); //corner3 x
					mVertices_.push_back(qc3_pos[1]); //corner3 y
					mVertices_.push_back(qc3_pos[2]); //corner3 z
					mVertices_.push_back(mScale_);    //zoom / w component of position
					auto qc3_tex = mTexCoords_[quadCorner3[TEXTURE_COORD_INDEX]]; //quad corner 3 _ texture coords
					mVertices_.push_back(qc3_tex[0]); //Corner 3 tex coord s
					mVertices_.push_back(qc3_tex[1]); //Corner 3 tex coord t

				}



				else { //There are both normals and texture coords
					   //Triangle 1
					auto qc0_pos = mPositions_[quadCorner0[POSITION_INDEX]]; //quad corner 0 _ positions
					mVertices_.push_back(qc0_pos[0]); //corner0 x
					mVertices_.push_back(qc0_pos[1]); //corner0 y
					mVertices_.push_back(qc0_pos[2]); //corner0 z
					mVertices_.push_back(mScale_);    //zoom / w component of position
					auto qc0_tex = mTexCoords_[quadCorner0[TEXTURE_COORD_INDEX]]; //quad corner 0 _ texture coords
					mVertices_.push_back(qc0_tex[0]); //Corner 0 tex coord s
					mVertices_.push_back(qc0_tex[1]); //Corner 0 tex coord t
					auto qc0_nrml = mNormals_[quadCorner0[NORMAL_INDEX]]; //quad corner 0 _ normal
					mVertices_.push_back(qc0_nrml[0]); //Corner 0 normal x
					mVertices_.push_back(qc0_nrml[1]); //Corner 0 normal y
					mVertices_.push_back(qc0_nrml[2]); //Corner 0 normal z


					auto qc1_pos = mPositions_[quadCorner1[POSITION_INDEX]]; //quad corner 1 _ positions
					mVertices_.push_back(qc1_pos[0]); //corner1 x
					mVertices_.push_back(qc1_pos[1]); //corner1 y
					mVertices_.push_back(qc1_pos[2]); //corner1 z
					mVertices_.push_back(mScale_);   //zoom
					auto qc1_tex = mTexCoords_[quadCorner1[TEXTURE_COORD_INDEX]]; //quad corner 1 _ texture coords
					mVertices_.push_back(qc1_tex[0]); //Corner 1 tex coord s
					mVertices_.push_back(qc1_tex[1]); //Corner 1 tex coord t
					auto qc1_nrml = mNormals_[quadCorner1[NORMAL_INDEX]]; //quad corner 1 _ normal
					mVertices_.push_back(qc1_nrml[0]); //Corner 1 normal x
					mVertices_.push_back(qc1_nrml[1]); //Corner 1 normal y
					mVertices_.push_back(qc1_nrml[2]); //Corner 1 normal z

					auto qc2_pos = mPositions_[quadCorner2[POSITION_INDEX]]; //quad corner 2 _ positions
					mVertices_.push_back(qc2_pos[0]); //corner2 x
					mVertices_.push_back(qc2_pos[1]); //corner2 y
					mVertices_.push_back(qc2_pos[2]); //corner2 z
					mVertices_.push_back(mScale_);    //zoom / w component of position
					auto qc2_tex = mTexCoords_[quadCorner2[TEXTURE_COORD_INDEX]]; //quad corner 2 _ texture coords
					mVertices_.push_back(qc2_tex[0]); //Corner 2 tex coord s
					mVertices_.push_back(qc2_tex[1]); //Corner 2 tex coord t
					auto qc2_nrml = mNormals_[quadCorner2[NORMAL_INDEX]]; //quad corner 2 _ normal
					mVertices_.push_back(qc2_nrml[0]); //Corner 2 normal x
					mVertices_.push_back(qc2_nrml[1]); //Corner 2 normal y
					mVertices_.push_back(qc2_nrml[2]); //Corner 2 normal z


						//Triangle 2
					mVertices_.push_back(qc2_pos[0]); //corner2 x
					mVertices_.push_back(qc2_pos[1]); //corner2 y
					mVertices_.push_back(qc2_pos[2]); //corner2 z
					mVertices_.push_back(mScale_);    //zoom / w component of position
					mVertices_.push_back(qc2_tex[0]); //Corner 2 tex coord s
					mVertices_.push_back(qc2_tex[1]); //Corner 2 tex coord t
					mVertices_.push_back(qc2_nrml[0]); //Corner 2 normal x
					mVertices_.push_back(qc2_nrml[1]); //Corner 2 normal y
					mVertices_.push_back(qc2_nrml[2]); //Corner 2 normal z

					mVertices_.push_back(qc1_pos[0]); //corner1 x
					mVertices_.push_back(qc1_pos[1]); //corner1 y
					mVertices_.push_back(qc1_pos[2]); //corner1 z
					mVertices_.push_back(mScale_);   //zoom
					mVertices_.push_back(qc1_tex[0]); //Corner 1 tex coord s
					mVertices_.push_back(qc1_tex[1]); //Corner 1 tex coord t
					mVertices_.push_back(qc1_nrml[0]); //Corner 1 normal x
					mVertices_.push_back(qc1_nrml[1]); //Corner 1 normal y
					mVertices_.push_back(qc1_nrml[2]); //Corner 1 normal z

					auto qc3_pos = mPositions_[quadCorner3[POSITION_INDEX]]; //quad corner 3 _ positions
					mVertices_.push_back(qc3_pos[0]); //corner3 x
					mVertices_.push_back(qc3_pos[1]); //corner3 y
					mVertices_.push_back(qc3_pos[2]); //corner3 z
					mVertices_.push_back(mScale_);    //zoom / w component of position
					auto qc3_tex = mTexCoords_[quadCorner3[TEXTURE_COORD_INDEX]]; //quad corner 3 _ texture coords
					mVertices_.push_back(qc3_tex[0]); //Corner 3 tex coord s
					mVertices_.push_back(qc3_tex[1]); //Corner 3 tex coord t
					auto qc3_nrml = mNormals_[quadCorner3[NORMAL_INDEX]]; //quad corner 3 _ normal
					mVertices_.push_back(qc3_nrml[0]); //Corner 3 normal x
					mVertices_.push_back(qc3_nrml[1]); //Corner 3 normal y
					mVertices_.push_back(qc3_nrml[2]); //Corner 3 normal z

				}
			}
			else { //There must be normals but no texture coords
				   //Triangle 1
				auto qc0_pos = mPositions_[quadCorner0[POSITION_INDEX]]; //quad corner 0 _ positions
				mVertices_.push_back(qc0_pos[0]); //corner0 x
				mVertices_.push_back(qc0_pos[1]); //corner0 y
				mVertices_.push_back(qc0_pos[2]); //corner0 z
				mVertices_.push_back(mScale_);    //zoom / w component of position
				//auto qc0_tex = mTexCoords_[quadCorner0[TEXTURE_COORD_INDEX]]; //quad corner 0 _ texture coords
				//mVertices_.push_back(qc0_tex[0]); //Corner 0 tex coord s
				//mVertices_.push_back(qc0_tex[1]); //Corner 0 tex coord t
				auto qc0_nrml = mNormals_[quadCorner0[NORMAL_INDEX]]; //quad corner 0 _ normal
				mVertices_.push_back(qc0_nrml[0]); //Corner 0 normal x
				mVertices_.push_back(qc0_nrml[1]); //Corner 0 normal y
				mVertices_.push_back(qc0_nrml[2]); //Corner 0 normal z


				auto qc1_pos = mPositions_[quadCorner1[POSITION_INDEX]]; //quad corner 1 _ positions
				mVertices_.push_back(qc1_pos[0]); //corner1 x
				mVertices_.push_back(qc1_pos[1]); //corner1 y
				mVertices_.push_back(qc1_pos[2]); //corner1 z
				mVertices_.push_back(mScale_);   //zoom
				//auto qc1_tex = mTexCoords_[quadCorner1[TEXTURE_COORD_INDEX]]; //quad corner 1 _ texture coords
				//mVertices_.push_back(qc1_tex[0]); //Corner 1 tex coord s
				//mVertices_.push_back(qc1_tex[1]); //Corner 1 tex coord t
				auto qc1_nrml = mNormals_[quadCorner1[NORMAL_INDEX]]; //quad corner 1 _ normal
				mVertices_.push_back(qc1_nrml[0]); //Corner 1 normal x
				mVertices_.push_back(qc1_nrml[1]); //Corner 1 normal y
				mVertices_.push_back(qc1_nrml[2]); //Corner 1 normal z

				auto qc2_pos = mPositions_[quadCorner2[POSITION_INDEX]]; //quad corner 2 _ positions
				mVertices_.push_back(qc2_pos[0]); //corner2 x
				mVertices_.push_back(qc2_pos[1]); //corner2 y
				mVertices_.push_back(qc2_pos[2]); //corner2 z
				mVertices_.push_back(mScale_);    //zoom / w component of position
				//auto qc2_tex = mTexCoords_[quadCorner2[TEXTURE_COORD_INDEX]]; //quad corner 2 _ texture coords
				//mVertices_.push_back(qc2_tex[0]); //Corner 2 tex coord s
				//mVertices_.push_back(qc2_tex[1]); //Corner 2 tex coord t
				auto qc2_nrml = mNormals_[quadCorner2[NORMAL_INDEX]]; //quad corner 2 _ normal
				mVertices_.push_back(qc2_nrml[0]); //Corner 2 normal x
				mVertices_.push_back(qc2_nrml[1]); //Corner 2 normal y
				mVertices_.push_back(qc2_nrml[2]); //Corner 2 normal z


				//Triangle 2
				mVertices_.push_back(qc2_pos[0]); //corner2 x
				mVertices_.push_back(qc2_pos[1]); //corner2 y
				mVertices_.push_back(qc2_pos[2]); //corner2 z
				mVertices_.push_back(mScale_);    //zoom / w component of position
				//mVertices_.push_back(qc2_tex[0]); //Corner 2 tex coord s
				//mVertices_.push_back(qc2_tex[1]); //Corner 2 tex coord t
				mVertices_.push_back(qc2_nrml[0]); //Corner 2 normal x
				mVertices_.push_back(qc2_nrml[1]); //Corner 2 normal y
				mVertices_.push_back(qc2_nrml[2]); //Corner 2 normal z


				mVertices_.push_back(qc1_pos[0]); //corner1 x
				mVertices_.push_back(qc1_pos[1]); //corner1 y
				mVertices_.push_back(qc1_pos[2]); //corner1 z
				mVertices_.push_back(mScale_);
				//mVertices_.push_back(qc1_tex[0]); //Corner 1 tex coord s
				//mVertices_.push_back(qc1_tex[1]); //Corner 1 tex coord t
				mVertices_.push_back(qc1_nrml[0]); //Corner 1 normal x
				mVertices_.push_back(qc1_nrml[1]); //Corner 1 normal y
				mVertices_.push_back(qc1_nrml[2]); //Corner 1 normal z


				auto qc3_pos = mPositions_[quadCorner3[POSITION_INDEX]]; //quad corner 3 _ positions
				mVertices_.push_back(qc3_pos[0]); //corner3 x
				mVertices_.push_back(qc3_pos[1]); //corner3 y
				mVertices_.push_back(qc3_pos[2]); //corner3 z
				mVertices_.push_back(mScale_);    //zoom / w component of position
				//auto qc3_tex = mTexCoords_[quadCorner3[TEXTURE_COORD_INDEX]]; //quad corner 3 _ texture coords
				//mVertices_.push_back(qc3_tex[0]); //Corner 3 tex coord s
				//mVertices_.push_back(qc3_tex[1]); //Corner 3 tex coord t
				auto qc3_nrml = mNormals_[quadCorner3[NORMAL_INDEX]]; //quad corner 3 _ normal
				mVertices_.push_back(qc3_nrml[0]); //Corner 3 normal x
				mVertices_.push_back(qc3_nrml[1]); //Corner 3 normal y
				mVertices_.push_back(qc3_nrml[2]); //Corner 3 normal z

			}
		}
		//Else we have a triangular face
		else {
			auto offsets = faceIter->getTriangleFace(); //Get the triangle face (note 'offsets' is an array of arrays)
			auto corner0 = offsets[0]; //Get the 3 corners
			auto corner1 = offsets[1];
			auto corner2 = offsets[2];

			//If we just have to worry about position data since there was no texture or normal data:
			if (!(faceIter->hasTexCoord() || faceIter->hasNormals())) {
				//Triangle:
				auto c0_pos = mPositions_[corner0[POSITION_INDEX]]; //corner 0 _ positions
				mVertices_.push_back(c0_pos[0]); //corner0 x
				mVertices_.push_back(c0_pos[1]); //corner0 y
				mVertices_.push_back(c0_pos[2]); //corner0 z
				mVertices_.push_back(mScale_);    //zoom / w component of position

				auto c1_pos = mPositions_[corner1[POSITION_INDEX]]; //corner 1 _ positions
				mVertices_.push_back(c1_pos[0]); //corner1 x
				mVertices_.push_back(c1_pos[1]); //corner1 y
				mVertices_.push_back(c1_pos[2]); //corner1 z
				mVertices_.push_back(mScale_);   //zoom

				auto c2_pos = mPositions_[corner2[POSITION_INDEX]]; //corner 2 _ positions
				mVertices_.push_back(c2_pos[0]); //corner2 x
				mVertices_.push_back(c2_pos[1]); //corner2 y
				mVertices_.push_back(c2_pos[2]); //corner2 z
				mVertices_.push_back(mScale_);    //zoom / w component of position

			}

			else if (faceIter->hasTexCoord()) {
				if (!(faceIter->hasNormals())) { //Only texture coords, no normals
					//Triangle:
					auto c0_pos = mPositions_[corner0[POSITION_INDEX]]; //corner 0 _ positions
					mVertices_.push_back(c0_pos[0]); //corner0 x
					mVertices_.push_back(c0_pos[1]); //corner0 y
					mVertices_.push_back(c0_pos[2]); //corner0 z
					mVertices_.push_back(mScale_);    //zoom / w component of position
					auto c0_tex = mTexCoords_[corner0[TEXTURE_COORD_INDEX]]; //corner 0 _ texture coords
					mVertices_.push_back(c0_tex[0]); //Corner 0 tex coord s
					mVertices_.push_back(c0_tex[1]); //Corner 0 tex coord t

					auto c1_pos = mPositions_[corner1[POSITION_INDEX]]; //corner 1 _ positions
					mVertices_.push_back(c1_pos[0]); //corner1 x
					mVertices_.push_back(c1_pos[1]); //corner1 y
					mVertices_.push_back(c1_pos[2]); //corner1 z
					mVertices_.push_back(mScale_);   //zoom
					auto c1_tex = mTexCoords_[corner1[TEXTURE_COORD_INDEX]]; //corner 1 _ texture coords
					mVertices_.push_back(c1_tex[0]); //Corner 1 tex coord s
					mVertices_.push_back(c1_tex[1]); //Corner 1 tex coord t

					auto c2_pos = mPositions_[corner2[POSITION_INDEX]]; //corner 2 _ positions
					mVertices_.push_back(c2_pos[0]); //corner2 x
					mVertices_.push_back(c2_pos[1]); //corner2 y
					mVertices_.push_back(c2_pos[2]); //corner2 z
					mVertices_.push_back(mScale_);    //zoom / w component of position
					auto c2_tex = mTexCoords_[corner2[TEXTURE_COORD_INDEX]]; //corner 2 _ texture coords
					mVertices_.push_back(c2_tex[0]); //Corner 2 tex coord s
					mVertices_.push_back(c2_tex[1]); //Corner 2 tex coord t
				}

				else { //Else we have both Texture and Normal data to deal with
					   //Triangle:
					auto c0_pos = mPositions_[corner0[POSITION_INDEX]]; //corner 0 _ positions
					mVertices_.push_back(c0_pos[0]); //corner0 x
					mVertices_.push_back(c0_pos[1]); //corner0 y
					mVertices_.push_back(c0_pos[2]); //corner0 z
					mVertices_.push_back(mScale_);    //zoom / w component of position
					auto c0_tex = mTexCoords_[corner0[TEXTURE_COORD_INDEX]]; //corner 0 _ texture coords
					mVertices_.push_back(c0_tex[0]); //Corner 0 tex coord s
					mVertices_.push_back(c0_tex[1]); //Corner 0 tex coord t
					auto c0_nrml = mNormals_[corner0[NORMAL_INDEX]]; // corner 0 _ normal
					mVertices_.push_back(c0_nrml[0]); //Corner 0 normal x
					mVertices_.push_back(c0_nrml[1]); //Corner 0 normal y
					mVertices_.push_back(c0_nrml[2]); //Corner 0 normal z


					auto c1_pos = mPositions_[corner1[POSITION_INDEX]]; //corner 1 _ positions
					mVertices_.push_back(c1_pos[0]); //corner1 x
					mVertices_.push_back(c1_pos[1]); //corner1 y
					mVertices_.push_back(c1_pos[2]); //corner1 z
					mVertices_.push_back(mScale_);   //zoom
					auto c1_tex = mTexCoords_[corner1[TEXTURE_COORD_INDEX]]; //corner 1 _ texture coords
					mVertices_.push_back(c1_tex[0]); //Corner 1 tex coord s
					mVertices_.push_back(c1_tex[1]); //Corner 1 tex coord t
					auto c1_nrml = mNormals_[corner1[NORMAL_INDEX]]; // corner 1 _ normal
					mVertices_.push_back(c1_nrml[0]); //Corner 1 normal x
					mVertices_.push_back(c1_nrml[1]); //Corner 1 normal y
					mVertices_.push_back(c1_nrml[2]); //Corner 1 normal z


					auto c2_pos = mPositions_[corner2[POSITION_INDEX]]; //corner 2 _ positions
					mVertices_.push_back(c2_pos[0]); //corner2 x
					mVertices_.push_back(c2_pos[1]); //corner2 y
					mVertices_.push_back(c2_pos[2]); //corner2 z
					mVertices_.push_back(mScale_);    //zoom / w component of position
					auto c2_tex = mTexCoords_[corner2[TEXTURE_COORD_INDEX]]; //corner 2 _ texture coords
					mVertices_.push_back(c2_tex[0]); //Corner 2 tex coord s
					mVertices_.push_back(c2_tex[1]); //Corner 2 tex coord t
					auto c2_nrml = mNormals_[corner2[NORMAL_INDEX]]; // corner 2 _ normal
					mVertices_.push_back(c2_nrml[0]); //Corner 2 normal x
					mVertices_.push_back(c2_nrml[1]); //Corner 2 normal y
					mVertices_.push_back(c2_nrml[2]); //Corner 2 normal z
				}
			}

			//Else there is just Position and normal data to worry about 
			else {
				//Triangle:
				auto c0_pos = mPositions_[corner0[POSITION_INDEX]]; //corner 0 _ positions
				mVertices_.push_back(c0_pos[0]); //corner0 x
				mVertices_.push_back(c0_pos[1]); //corner0 y
				mVertices_.push_back(c0_pos[2]); //corner0 z
				mVertices_.push_back(mScale_);    //zoom / w component of position
				//auto c0_tex = mTexCoords_[corner0[TEXTURE_COORD_INDEX]]; //corner 0 _ texture coords
				//mVertices_.push_back(c0_tex[0]); //Corner 0 tex coord s
				//mVertices_.push_back(c0_tex[1]); //Corner 0 tex coord t
				auto c0_nrml = mNormals_[corner0[NORMAL_INDEX]]; // corner 0 _ normal
				mVertices_.push_back(c0_nrml[0]); //Corner 0 normal x
				mVertices_.push_back(c0_nrml[1]); //Corner 0 normal y
				mVertices_.push_back(c0_nrml[2]); //Corner 0 normal z


				auto c1_pos = mPositions_[corner1[POSITION_INDEX]]; //corner 1 _ positions
				mVertices_.push_back(c1_pos[0]); //corner1 x
				mVertices_.push_back(c1_pos[1]); //corner1 y
				mVertices_.push_back(c1_pos[2]); //corner1 z
				mVertices_.push_back(mScale_);   //zoom
				//auto c1_tex = mTexCoords_[corner1[TEXTURE_COORD_INDEX]]; //corner 1 _ texture coords
				//mVertices_.push_back(c1_tex[0]); //Corner 1 tex coord s
				//mVertices_.push_back(c1_tex[1]); //Corner 1 tex coord t
				auto c1_nrml = mNormals_[corner1[NORMAL_INDEX]]; // corner 1 _ normal
				mVertices_.push_back(c1_nrml[0]); //Corner 1 normal x
				mVertices_.push_back(c1_nrml[1]); //Corner 1 normal y
				mVertices_.push_back(c1_nrml[2]); //Corner 1 normal z


				auto c2_pos = mPositions_[corner2[POSITION_INDEX]]; //corner 2 _ positions
				mVertices_.push_back(c2_pos[0]); //corner2 x
				mVertices_.push_back(c2_pos[1]); //corner2 y
				mVertices_.push_back(c2_pos[2]); //corner2 z
				mVertices_.push_back(mScale_);    //zoom / w component of position
				//auto c2_tex = mTexCoords_[corner2[TEXTURE_COORD_INDEX]]; //corner 2 _ texture coords
				//mVertices_.push_back(c2_tex[0]); //Corner 2 tex coord s
				//mVertices_.push_back(c2_tex[1]); //Corner 2 tex coord t
				auto c2_nrml = mNormals_[corner2[NORMAL_INDEX]]; // corner 2 _ normal
				mVertices_.push_back(c2_nrml[0]); //Corner 2 normal x
				mVertices_.push_back(c2_nrml[1]); //Corner 2 normal y
				mVertices_.push_back(c2_nrml[2]); //Corner 2 normal z
			}
		}
	}
}

void QuickObj::loadLineIntoVertex(const char * line, std::vector<Vertex>& verts) {
	float values[3];
	int vertComponent = 0;
	const char * strIter = line;

	while ((*strIter != '\n') && (*strIter != '\0')) {
		if (isNumber(strIter) || (*strIter == '-')) {
			if (vertComponent > 2) {
				fprintf(ERRLOG, "\nERROR! Read too many values!\n");
				return;
			}
			else {
				char * temp; //see http://www.cplusplus.com/reference/cstdlib/strtof/
				values[vertComponent++] = strtof(strIter, &temp);
				strIter = temp;
			}
		}
		else if (*strIter == ' ') {
			strIter++;
		}
		else {
			fprintf(ERRLOG, "\nERROR! Unrecognized data: %s\n", strIter);
		}
	}

	switch (vertComponent) {
	case 0:
		[[__fallthrough]]
	default:
		fprintf(ERRLOG, "\nERROR! No (or too much) data was loaded for line %s\n", line);
		break;
	case 1:
		fprintf(WRNLOG, "\nWarning! Only 1 data value was read from line %sVerify this is correct plz!\n", line);
		verts.push_back(Vertex(values[0]));
		break;
	case 2:
		verts.push_back(Vertex(values[0], values[1]));
		break;
	case 3:
		verts.push_back(Vertex(values[0], values[1], values[2]));
		break;
	case 4:
		fprintf(ERRLOG, "\nERROR! TOO many data values were read for line %s\n", line);
		break;
	}
}