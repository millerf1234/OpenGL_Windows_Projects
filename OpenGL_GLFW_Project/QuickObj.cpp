//File:               QuickObj.cpp
//
//  See Header file for more details
//
//  Known Issues:      Currently a face must be either 3 or 4 vertices. Any more than this 
//                     will not parse correctly. I have tried to implement a work-around class
//                     for n-gons, but I have been unable to complete this class due to needing
//                     to pay my rent rather than writing the code to fix the issue. 
//                   
//                    In a file with multiple objects, it is possible for some to have texture coordinates/normals
//                     and others to not. The way this class is currently implemented did not account for this possibility,
//                     and at this point there does not seem to me to be an easy way to remedy this.
//
// Programmer:          Forrest Miller
// Date:                November 2018


#include "QuickObj.h"

#include "QuickObj_NGonParser.h"

namespace { //An anonymous namespace is used to prevent these constants from polluting the global namespace
    static constexpr const size_t POSITION_INDEX = 0u;
    static constexpr const size_t TEXTURE_COORD_INDEX = 1u;
    static constexpr const size_t NORMAL_INDEX = 2u;

    static constexpr const size_t POSITION_COMPONENTS = 4u;
    static constexpr const size_t TEXTURE_COORDINATE_COMPONENTS = 2u;
    static constexpr const size_t NORMAL_COMPONENTS = 3u;

    static constexpr const size_t POSITION_VERTEX_SIZE = POSITION_COMPONENTS;
    static constexpr const size_t POSITION_TEXCOORD_VERTEX_SIZE = POSITION_COMPONENTS + TEXTURE_COORDINATE_COMPONENTS;
    static constexpr const size_t POSITION_NORMAL_VERTEX_SIZE = POSITION_COMPONENTS + NORMAL_COMPONENTS;
    static constexpr const size_t POSITION_TEXCOORD_NORMAL_VERTEX_SIZE = POSITION_COMPONENTS + TEXTURE_COORDINATE_COMPONENTS + NORMAL_COMPONENTS;

    static constexpr const size_t VERTICES_IN_A_TRIANGLE = 3u; 
    static constexpr const size_t TRIANGLES_IN_A_QUAD = 2u;

    //It turns out that the triangles within a Quad have a 0-1-3-2 ordering to them. Thus the following 
    //constants are defined to attempt to avert additional confusion within the already-pretty-hairy 
    //parsing logic
    static constexpr const size_t QUAD_CORNER_0_OFFSETS = 0u;
    static constexpr const size_t QUAD_CORNER_1_OFFSETS = 1u;
    static constexpr const size_t QUAD_CORNER_3_OFFSETS = 3u; //Is 3u to accommodate triangle winding order 
    static constexpr const size_t QUAD_CORNER_2_OFFSETS = 2u; //Is 2u to accommodate triangle winding order
}



QuickObj::QuickObj(std::string filepath, float scale, bool generateMissingComponents) {
    mError_ = false;
    mScale_ = scale;
    mHasTexCoords_ = false;
    mHasNormals_ = false;
    //Load the file as an AsciiAsset object
    mFile_ = std::make_unique<AssetLoadingInternal::AsciiAsset>(filepath);

    if (mFile_->getStoredTextLength() > 1u) { //Was 0u
        //preparseFile(); //This is unnecessary 
        parseFile();
    }
    else {
        fprintf(ERRLOG, "\nERROR acquiring file: %s!\n", filepath.c_str());
        mError_ = true;
        return;
    }

    if (generateMissingComponents) {
        if (mVertices_.size() > 0u) { //Only generate components if some data has been loaded
            addMissingComponents(true, 0.0f, 1.0f);
        }
    }

    if (mLines_.size() > 0u)
        addParsedLinePrimitivesToEndOfMeshData();

    mVertices_.shrink_to_fit();
}


//The current implementation here is not the most efficient, since essentially it follows the exact same steps as 
//the non-texCoord-Normal-generating constructor before filling in the missing data. A better implementation would
//fill in the missing data as it goes.
QuickObj::QuickObj(const std::string filepath, float scale, bool generateMissingComponents, bool randomizeTextureCoords, float s, float t) {
    mError_ = false;
    mScale_ = scale;
    mHasTexCoords_ = false;
    mHasNormals_ = false;
    mFile_ = std::make_unique<AssetLoadingInternal::AsciiAsset>(filepath);

    if (mFile_->getStoredTextLength() > 0u) {
        //preparseFile(); //This is unnecessary 
        parseFile();
    }
    else {
        fprintf(ERRLOG, "\nERROR acquiring the file: \"%s\"\n", filepath.c_str());
        mError_ = true;
        return;
    }
    if (generateMissingComponents) {
        if (mVertices_.size() > 0u) { //Only generate components if some data has been loaded
            addMissingComponents(randomizeTextureCoords, s, t);
        }
    }
    
    if (mLines_.size() > 0u)
        addParsedLinePrimitivesToEndOfMeshData();

    mVertices_.shrink_to_fit();
}


//QuickObj::~QuickObj() {
//
//}





//This is a very quick and dirty implementation. Not clean
void QuickObj::parseFile() {
    //TODO  Calculate required space for each vector and reserve said space ahead of time
    /*size_t tempFilesize = mFile_->getNumberOfLines();
    mPositions_.reserve(tempFilesize);
    mNormals_.reserve(tempFilesize);
    mTexCoords_.reserve(tempFilesize);
    */

    //////////////////////////////////////////////
    //TODO  finish this idea
    //To test task-based computing, I am going to handle ngons as a separate task from parsing the 
    //rest of the file. Any parsed ngon vertex data will then be appended onto the end of any loaded 
    //data at the end of this function.
    //auto ngons = findNGons(*mFile_);
    //fprintf(MSGLOG, "\nTesting NGON Finder... Found %u NGons!\n\n", ngons.size());
    ////////////////////////////////////

    //In case a line starting with "vp" is parsed (representing freeform geometry data),
    //we only need to print out a warning about parser not supporting freeform geometry 
    //one time. 
    bool freeformGeometryWarningMessageFlag = false; 

    size_t fileSize = mFile_->getNumberOfLines();
    for (size_t i = 0; i < fileSize; i++) {
        std::string line = mFile_->getLine(i);
        const char * lineIter = line.c_str();

        switch (*lineIter) {
        case '\n':
            continue; //If it's an empty line, skip it
        case 'o':
            continue; //Skip object tags
        case 'g':
            continue; //Skip group tags
        case 's': 
            continue; //Skip smoothing group tags
        case '#':
            //Skip comments for now...
            continue;
        case 'u':
            //Line probably is a "usemtl ____"  instruction
            //No material using for now
            break;
        case 'v':
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
                if (!freeformGeometryWarningMessageFlag) {
                    fprintf(MSGLOG, "\n\nWhoah! Freeform geometry data encountered! This is quite unexpected!\n"
                        "It looks like you are trying to load an Advanced OBJ file.\n"
                        "Unfortunately freeform geometry is not supported at this time...\n"
                        "\t  [All freeform geometry in this file will be skipped]\n");
                    freeformGeometryWarningMessageFlag = true;
                }
                continue;
            }
            else {
                fprintf(ERRLOG, "\nERROR parsing line %s\n", line.c_str());
            }
            break;
        case 'f': [[fallthrough]]; //If not c++17 then just comment out the '[[fallthrough]]' statement
        case 'F':
            mFaces_.emplace_back(AssetLoadingInternal::Face{ lineIter, true });
            break;
        case 'l': [[fallthrough]]; //If not c++17 then just comment out the '[[fallthrough]]' statement
        case 'L':
            mLines_.emplace_back(AssetLoadingInternal::Line(lineIter, true));
            break;
        case 'm':
            if (*(lineIter + 1u) == 'g') { //If line starts with "mg", then
                //It's a merging group, which we will skip
                fprintf(MSGLOG, "Skipping Merging Group: %s", line.c_str());
            }
            else {
                //It's the name of a material, which currently are not implemented
                fprintf(MSGLOG, "Skipping Material: %s", line.c_str());
            }
            break;
        
        default:
            fprintf(MSGLOG, "\nUnable to parse line %s\n", line.c_str());
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

    static constexpr const size_t SPACE_PER_QUAD_FACE =
        POSITION_TEXCOORD_NORMAL_VERTEX_SIZE * VERTICES_IN_A_TRIANGLE * TRIANGLES_IN_A_QUAD;
    static constexpr const size_t SPACE_PER_TRIANGLE_FACE =
        POSITION_TEXCOORD_NORMAL_VERTEX_SIZE * VERTICES_IN_A_TRIANGLE;
    static constexpr const size_t SPACE_PER_LINE =
        POSITION_TEXCOORD_NORMAL_VERTEX_SIZE * 3u; //I cheat and convert lines into triangles, hence the factor of 3u instead of 2u

    int triangleFaces = 0;
    int quadFaces = 0;
    const size_t linePrimitives = mLines_.size();
    for (auto faceIter = mFaces_.begin(); faceIter != mFaces_.end(); faceIter++) {
        if (faceIter->isQuad()) {
            quadFaces++;
        }
        else {
            triangleFaces++;
        }
    }

    const size_t spaceToReserve =
        (quadFaces * SPACE_PER_QUAD_FACE) +
        (triangleFaces * SPACE_PER_TRIANGLE_FACE) +
        (linePrimitives * SPACE_PER_LINE);

    mVertices_.reserve(spaceToReserve);

    fprintf(MSGLOG, "\n*** Model Statistics ***\nPrimitive Counts:  Lines: %u\tTriangles: %d\tQuads: %d\n",
        linePrimitives, triangleFaces, quadFaces);
    fprintf(MSGLOG, "Parsed  Positions: %d\tTexCoords: %d\tNormals: %d\n", mPositions_.size(),
        mTexCoords_.size(), mNormals_.size());


    //Construct each face from the parsed data into the mVertices_ vector, with positions/textureCoords/Normals interlaced
    for (auto faceIter = mFaces_.begin(); faceIter != mFaces_.end(); faceIter++) {
        if (faceIter->isQuad()) { //Quads will be triangulated here:
            auto offsets = faceIter->getQuadFace(); //Get the quad face (note variable 'offsets' is an array of arrays)
            auto quadCorner0 = offsets[QUAD_CORNER_0_OFFSETS]; //Get the quad's 4 corners
            auto quadCorner1 = offsets[QUAD_CORNER_1_OFFSETS];
            auto quadCorner2 = offsets[QUAD_CORNER_3_OFFSETS];
            auto quadCorner3 = offsets[QUAD_CORNER_2_OFFSETS];

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
                    mVertices_.push_back(qc0_tex[0]);      //Corner 0 tex coord s
                    mVertices_.push_back(1.0f-qc0_tex[1]); //Corner 0 tex coord t


                    auto qc1_pos = mPositions_[quadCorner1[POSITION_INDEX]]; //quad corner 1 _ positions
                    mVertices_.push_back(qc1_pos[0]); //corner1 x
                    mVertices_.push_back(qc1_pos[1]); //corner1 y
                    mVertices_.push_back(qc1_pos[2]); //corner1 z
                    mVertices_.push_back(mScale_);   //zoom
                    auto qc1_tex = mTexCoords_[quadCorner1[TEXTURE_COORD_INDEX]]; //quad corner 1 _ texture coords
                    mVertices_.push_back(qc1_tex[0]);      //Corner 1 tex coord s
                    mVertices_.push_back(1.0f-qc1_tex[1]); //Corner 1 tex coord t


                    auto qc2_pos = mPositions_[quadCorner2[POSITION_INDEX]]; //quad corner 2 _ positions
                    mVertices_.push_back(qc2_pos[0]); //corner2 x
                    mVertices_.push_back(qc2_pos[1]); //corner2 y
                    mVertices_.push_back(qc2_pos[2]); //corner2 z
                    mVertices_.push_back(mScale_);    //zoom / w component of position
                    auto qc2_tex = mTexCoords_[quadCorner2[TEXTURE_COORD_INDEX]]; //quad corner 2 _ texture coords
                    mVertices_.push_back(qc2_tex[0]);      //Corner 2 tex coord s
                    mVertices_.push_back(1.0f-qc2_tex[1]); //Corner 2 tex coord t



                    //Triangle 2
                    mVertices_.push_back(qc2_pos[0]); //corner2 x
                    mVertices_.push_back(qc2_pos[1]); //corner2 y
                    mVertices_.push_back(qc2_pos[2]); //corner2 z
                    mVertices_.push_back(mScale_);    //zoom / w component of position
                    mVertices_.push_back(qc2_tex[0]);      //Corner 2 tex coord s
                    mVertices_.push_back(1.0f-qc2_tex[1]); //Corner 2 tex coord t

                    mVertices_.push_back(qc1_pos[0]); //corner1 x
                    mVertices_.push_back(qc1_pos[1]); //corner1 y
                    mVertices_.push_back(qc1_pos[2]); //corner1 z
                    mVertices_.push_back(mScale_);
                    mVertices_.push_back(qc1_tex[0]);      //Corner 1 tex coord s
                    mVertices_.push_back(1.0f-qc1_tex[1]); //Corner 1 tex coord t

                    auto qc3_pos = mPositions_[quadCorner3[POSITION_INDEX]]; //quad corner 3 _ positions
                    mVertices_.push_back(qc3_pos[0]); //corner3 x
                    mVertices_.push_back(qc3_pos[1]); //corner3 y
                    mVertices_.push_back(qc3_pos[2]); //corner3 z
                    mVertices_.push_back(mScale_);    //zoom / w component of position
                    auto qc3_tex = mTexCoords_[quadCorner3[TEXTURE_COORD_INDEX]]; //quad corner 3 _ texture coords
                    mVertices_.push_back(qc3_tex[0]);      //Corner 3 tex coord s
                    mVertices_.push_back(1.0f-qc3_tex[1]); //Corner 3 tex coord t

                }



                else { //There are both normals and texture coords
                       //Triangle 1
                    auto qc0_pos = mPositions_[quadCorner0[POSITION_INDEX]]; //quad corner 0 _ positions
                    mVertices_.push_back(qc0_pos[0]); //corner0 x
                    mVertices_.push_back(qc0_pos[1]); //corner0 y
                    mVertices_.push_back(qc0_pos[2]); //corner0 z
                    mVertices_.push_back(mScale_);    //zoom / w component of position
                    auto qc0_tex = mTexCoords_[quadCorner0[TEXTURE_COORD_INDEX]]; //quad corner 0 _ texture coords
                    mVertices_.push_back(qc0_tex[0]);      //Corner 0 tex coord s
                    mVertices_.push_back(1.0f-qc0_tex[1]); //Corner 0 tex coord t
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
                    mVertices_.push_back(qc1_tex[0]);      //Corner 1 tex coord s
                    mVertices_.push_back(1.0f-qc1_tex[1]); //Corner 1 tex coord t
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
                    mVertices_.push_back(qc2_tex[0]);      //Corner 2 tex coord s
                    mVertices_.push_back(1.0f-qc2_tex[1]); //Corner 2 tex coord t
                    auto qc2_nrml = mNormals_[quadCorner2[NORMAL_INDEX]]; //quad corner 2 _ normal
                    mVertices_.push_back(qc2_nrml[0]); //Corner 2 normal x
                    mVertices_.push_back(qc2_nrml[1]); //Corner 2 normal y
                    mVertices_.push_back(qc2_nrml[2]); //Corner 2 normal z


                        //Triangle 2
                    mVertices_.push_back(qc2_pos[0]); //corner2 x
                    mVertices_.push_back(qc2_pos[1]); //corner2 y
                    mVertices_.push_back(qc2_pos[2]); //corner2 z
                    mVertices_.push_back(mScale_);    //zoom / w component of position
                    mVertices_.push_back(qc2_tex[0]);      //Corner 2 tex coord s
                    mVertices_.push_back(1.0f-qc2_tex[1]); //Corner 2 tex coord t
                    mVertices_.push_back(qc2_nrml[0]); //Corner 2 normal x
                    mVertices_.push_back(qc2_nrml[1]); //Corner 2 normal y
                    mVertices_.push_back(qc2_nrml[2]); //Corner 2 normal z

                    mVertices_.push_back(qc1_pos[0]); //corner1 x
                    mVertices_.push_back(qc1_pos[1]); //corner1 y
                    mVertices_.push_back(qc1_pos[2]); //corner1 z
                    mVertices_.push_back(mScale_);   //zoom
                    mVertices_.push_back(qc1_tex[0]);      //Corner 1 tex coord s
                    mVertices_.push_back(1.0f-qc1_tex[1]); //Corner 1 tex coord t
                    mVertices_.push_back(qc1_nrml[0]); //Corner 1 normal x
                    mVertices_.push_back(qc1_nrml[1]); //Corner 1 normal y
                    mVertices_.push_back(qc1_nrml[2]); //Corner 1 normal z

                    auto qc3_pos = mPositions_[quadCorner3[POSITION_INDEX]]; //quad corner 3 _ positions
                    mVertices_.push_back(qc3_pos[0]); //corner3 x
                    mVertices_.push_back(qc3_pos[1]); //corner3 y
                    mVertices_.push_back(qc3_pos[2]); //corner3 z
                    mVertices_.push_back(mScale_);    //zoom / w component of position
                    auto qc3_tex = mTexCoords_[quadCorner3[TEXTURE_COORD_INDEX]]; //quad corner 3 _ texture coords
                    mVertices_.push_back(qc3_tex[0]);      //Corner 3 tex coord s
                    mVertices_.push_back(1.0f-qc3_tex[1]); //Corner 3 tex coord t
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
                auto qc0_nrml = mNormals_[quadCorner0[NORMAL_INDEX]]; //quad corner 0 _ normal
                mVertices_.push_back(qc0_nrml[0]); //Corner 0 normal x
                mVertices_.push_back(qc0_nrml[1]); //Corner 0 normal y
                mVertices_.push_back(qc0_nrml[2]); //Corner 0 normal z


                auto qc1_pos = mPositions_[quadCorner1[POSITION_INDEX]]; //quad corner 1 _ positions
                mVertices_.push_back(qc1_pos[0]); //corner1 x
                mVertices_.push_back(qc1_pos[1]); //corner1 y
                mVertices_.push_back(qc1_pos[2]); //corner1 z
                mVertices_.push_back(mScale_);   //zoom
                auto qc1_nrml = mNormals_[quadCorner1[NORMAL_INDEX]]; //quad corner 1 _ normal
                mVertices_.push_back(qc1_nrml[0]); //Corner 1 normal x
                mVertices_.push_back(qc1_nrml[1]); //Corner 1 normal y
                mVertices_.push_back(qc1_nrml[2]); //Corner 1 normal z

                auto qc2_pos = mPositions_[quadCorner2[POSITION_INDEX]]; //quad corner 2 _ positions
                mVertices_.push_back(qc2_pos[0]); //corner2 x
                mVertices_.push_back(qc2_pos[1]); //corner2 y
                mVertices_.push_back(qc2_pos[2]); //corner2 z
                mVertices_.push_back(mScale_);    //zoom / w component of position
                auto qc2_nrml = mNormals_[quadCorner2[NORMAL_INDEX]]; //quad corner 2 _ normal
                mVertices_.push_back(qc2_nrml[0]); //Corner 2 normal x
                mVertices_.push_back(qc2_nrml[1]); //Corner 2 normal y
                mVertices_.push_back(qc2_nrml[2]); //Corner 2 normal z


                //Triangle 2
                mVertices_.push_back(qc2_pos[0]); //corner2 x
                mVertices_.push_back(qc2_pos[1]); //corner2 y
                mVertices_.push_back(qc2_pos[2]); //corner2 z
                mVertices_.push_back(mScale_);    //zoom / w component of position
                mVertices_.push_back(qc2_nrml[0]); //Corner 2 normal x
                mVertices_.push_back(qc2_nrml[1]); //Corner 2 normal y
                mVertices_.push_back(qc2_nrml[2]); //Corner 2 normal z


                mVertices_.push_back(qc1_pos[0]); //corner1 x
                mVertices_.push_back(qc1_pos[1]); //corner1 y
                mVertices_.push_back(qc1_pos[2]); //corner1 z
                mVertices_.push_back(mScale_);
                mVertices_.push_back(qc1_nrml[0]); //Corner 1 normal x
                mVertices_.push_back(qc1_nrml[1]); //Corner 1 normal y
                mVertices_.push_back(qc1_nrml[2]); //Corner 1 normal z


                auto qc3_pos = mPositions_[quadCorner3[POSITION_INDEX]]; //quad corner 3 _ positions
                mVertices_.push_back(qc3_pos[0]); //corner3 x
                mVertices_.push_back(qc3_pos[1]); //corner3 y
                mVertices_.push_back(qc3_pos[2]); //corner3 z
                mVertices_.push_back(mScale_);    //zoom / w component of position
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
                    mVertices_.push_back(c0_tex[0]);      //Corner 0 tex coord s
                    mVertices_.push_back(1.0f-c0_tex[1]); //Corner 0 tex coord t

                    auto c1_pos = mPositions_[corner1[POSITION_INDEX]]; //corner 1 _ positions
                    mVertices_.push_back(c1_pos[0]); //corner1 x
                    mVertices_.push_back(c1_pos[1]); //corner1 y
                    mVertices_.push_back(c1_pos[2]); //corner1 z
                    mVertices_.push_back(mScale_);   //zoom
                    auto c1_tex = mTexCoords_[corner1[TEXTURE_COORD_INDEX]]; //corner 1 _ texture coords
                    mVertices_.push_back(c1_tex[0]);      //Corner 1 tex coord s
                    mVertices_.push_back(1.0f-c1_tex[1]); //Corner 1 tex coord t

                    auto c2_pos = mPositions_[corner2[POSITION_INDEX]]; //corner 2 _ positions
                    mVertices_.push_back(c2_pos[0]); //corner2 x
                    mVertices_.push_back(c2_pos[1]); //corner2 y
                    mVertices_.push_back(c2_pos[2]); //corner2 z
                    mVertices_.push_back(mScale_);    //zoom / w component of position
                    auto c2_tex = mTexCoords_[corner2[TEXTURE_COORD_INDEX]]; //corner 2 _ texture coords
                    mVertices_.push_back(c2_tex[0]);      //Corner 2 tex coord s
                    mVertices_.push_back(1.0f-c2_tex[1]); //Corner 2 tex coord t
                }

                else { //Else we have both Texture and Normal data to deal with
                       //Triangle:
                       auto c0_pos = mPositions_[corner0[POSITION_INDEX]]; //corner 0 _ positions
                       mVertices_.push_back(c0_pos[0]); //corner0 x
                       mVertices_.push_back(c0_pos[1]); //corner0 y
                       mVertices_.push_back(c0_pos[2]); //corner0 z
                       mVertices_.push_back(mScale_);    //zoom / w component of position
                       auto c0_tex = mTexCoords_[corner0[TEXTURE_COORD_INDEX]]; //corner 0 _ texture coords
                       mVertices_.push_back(c0_tex[0]);      //Corner 0 tex coord s
                       mVertices_.push_back(1.0f-c0_tex[1]); //Corner 0 tex coord t
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
                       mVertices_.push_back(c1_tex[0]);      //Corner 1 tex coord s
                       mVertices_.push_back(1.0f-c1_tex[1]); //Corner 1 tex coord t
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
                       mVertices_.push_back(c2_tex[0]);      //Corner 2 tex coord s
                       mVertices_.push_back(1.0f-c2_tex[1]); //Corner 2 tex coord t
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

//This function handles parsing vertex data by inserting the numerical values it reads into a 'Vertex' object 
//and then adding that object to the end of 
void QuickObj::loadLineIntoVertex(const char * line, std::vector<Vertex>& verts) {
    float values[3u];
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
        fprintf(ERRLOG, "\nERROR! No data was loaded for the line %s", line);
        fprintf(ERRLOG, "\nTo keep the vertex ordering intact, a dummy vertex will be substituted!\n");
        verts.emplace_back(Vertex(0.0f));
        break;
    case 1:
        fprintf(WRNLOG, "\nWarning! Only 1 data value was read from line %s", line);
        verts.emplace_back(Vertex(values[0])); //emplace_back might be faster than push_back
        break;
    case 2:
        //verts.push_back(Vertex(values[0], values[1]));
        verts.emplace_back(Vertex(values[0], values[1]));
        break;
    case 3:
        //verts.push_back(Vertex(values[0], values[1], values[2]));
        verts.emplace_back(Vertex(values[0], values[1], values[2]));
        break;
    default:
        fprintf(ERRLOG, "\nERROR! Too many data values were read for a vertex. Only the first 3 values will be used!\n");
        verts.emplace_back(Vertex(values[0], values[1], values[2]));
        break;
    }
}


void QuickObj::addMissingComponents(bool randomizeTextureCoords, float s, float t) {
    
    if (mHasTexCoords_ && mHasNormals_) {
        return; //There is nothing missing, so return
    }

    size_t loadedDataSize = mVertices_.size(); //Used to validate loaded data to prevent issues when generating new data
    
    if (mHasTexCoords_) {
        if (!verifyVertexComponents(loadedDataSize, POSITION_COMPONENTS + TEXTURE_COORDINATE_COMPONENTS)) {
            mError_ = true;
            fprintf(ERRLOG, "\nError occurred during the loading of model from file: %s\n"
                "The number of values (%u) loaded from the file does not match the expected\n"
                "vertex size (6-components per vertex [4 position + 2 tex])!\n", mFile_->getFilepath().c_str(), loadedDataSize);
            return;
        }
        generateMissingNormals();
    }
    else if (mHasNormals_) {
        if (!verifyVertexComponents(loadedDataSize, POSITION_COMPONENTS + NORMAL_COMPONENTS)) {
            mError_ = true;
            fprintf(ERRLOG, "\nError occurred during the loading of model from file: %s\n"
                "The number of values (%u) loaded from the file does not match the expected\n"
                "vertex size (7-components expected per vertex [4 position + 3 normal])!\n", mFile_->getFilepath().c_str(), loadedDataSize);
            return;
        }
        generateMissingTextureCoords(randomizeTextureCoords, s, t);
    }
    else { //File was missing both normals and texture components
        if (!verifyVertexComponents(loadedDataSize, POSITION_COMPONENTS)) {
            mError_ = true;
            fprintf(ERRLOG, "\nError occurred during the loading of model from file: %s\n"
                "The number of values (%u) loaded from the file does not match the expected\n"
                "vertex size (4-components expected per vertex [4 position])!\n", mFile_->getFilepath().c_str(), loadedDataSize);
            return;
        }
        generateMissingTextureCoordsAndNormals(randomizeTextureCoords, s, t);
    }

    fprintf(MSGLOG, "Missing data was generated for the model loaded from file: %s\n", mFile_->getFilepath().c_str());
    fprintf(MSGLOG, "   [%u values were loaded from file, %u values were generated, brining total size to %u]\n",
        loadedDataSize, mVertices_.size() - loadedDataSize, mVertices_.size());
}


//The idea behind this function is to iterate through the loaded data and fill in the missing components
void QuickObj::generateMissingTextureCoords(bool randomizeTextureCoords, float s, float t) {

    int currentVertexComponent = -1; //Will be incremented to 0 in first iteration
    std::vector<float> verticesWithTexCoords; //Data is to be filled into this vector which will then swap with mVertices_
    verticesWithTexCoords.reserve( (mVertices_.size()/POSITION_TEXCOORD_VERTEX_SIZE) * POSITION_TEXCOORD_NORMAL_VERTEX_SIZE); //Reserve the required space 

    auto vertsEnd = mVertices_.end(); //Iterator to end of vertices
    auto vertsBegin = mVertices_.begin(); //Iterator to start of vertices
    
    if (!randomizeTextureCoords) {
        for (auto componentIter = vertsBegin; componentIter != vertsEnd; componentIter++) {
            currentVertexComponent = ((currentVertexComponent + 1u) % POSITION_NORMAL_VERTEX_SIZE);
            if (currentVertexComponent == 3) {
                //Add the fourth position component (scale) to vector
                verticesWithTexCoords.push_back(*componentIter);
                //Add the s texture coord to the vector
                verticesWithTexCoords.push_back(s);
                //Add the t texture coord to the vector
                verticesWithTexCoords.push_back(t);
            }
            else {
                verticesWithTexCoords.push_back(*componentIter);
            }
        }
    }
    else {
        for (auto componentIter = vertsBegin; componentIter != vertsEnd; componentIter++) {
            currentVertexComponent = ((currentVertexComponent + 1u) % POSITION_NORMAL_VERTEX_SIZE);
            if (currentVertexComponent == 3) {
                //Add the fourth position component (scale) to vector
                verticesWithTexCoords.push_back(*componentIter);
                //Add the random s texture coord to the vector
                verticesWithTexCoords.push_back(MathFunc::getRandomInRangef(0.0f, 1.0f));
                //Add the random t texture coord to the vector
                verticesWithTexCoords.push_back(MathFunc::getRandomInRangef(0.0f, 1.0f));
            }
            else {
                verticesWithTexCoords.push_back(*componentIter);
            }
        }
    }
    verticesWithTexCoords.swap(mVertices_);
    mHasTexCoords_ = true;
}


void QuickObj::generateMissingNormals() {
    if (!verifyVertexComponents(mVertices_.size(), POSITION_TEXCOORD_VERTEX_SIZE * VERTICES_IN_A_TRIANGLE)) {
        fprintf(ERRLOG, "\nError! Unable to generate triangle normals for model from file: \"%s\"!\n"
            "The data was loaded fine (positions and textures) but the number of values loaded does not match\n"
            "the number needed for a whole number of triangles (%u values were loaded,\n"
            "this number must be divisible by 18 for proper normal generation!)\n\n", mFile_->getFilepath().c_str(), mVertices_.size());
        mError_ = true;
        return;
    }

    glm::vec3 v0, v1, v2, computedNormal;

    std::vector<float> verticesWithNormals;
    verticesWithNormals.reserve((mVertices_.size() / POSITION_TEXCOORD_VERTEX_SIZE) * POSITION_TEXCOORD_NORMAL_VERTEX_SIZE); //Reserve the required space 


    //Count the number of triangles for the object
    size_t numberOfTriangles = (mVertices_.size() / (POSITION_TEXCOORD_VERTEX_SIZE * VERTICES_IN_A_TRIANGLE));

    //Loop through the object's data triangle by triangle
    for (size_t i = 0u; i < numberOfTriangles; i++) {
        auto triangleStart = (mVertices_.begin() + (i * (POSITION_TEXCOORD_VERTEX_SIZE * VERTICES_IN_A_TRIANGLE)));

        v0 = glm::vec3(*(triangleStart), *(triangleStart + 1u), *(triangleStart + 2u));  //skip w, s, t (located at indices 3-5)
        v1 = glm::vec3(*(triangleStart + 6u), *(triangleStart + 7u), *(triangleStart + 8u)); //skip w, s, t (located at indices 9-11)
        v2 = glm::vec3(*(triangleStart + 12u), *(triangleStart + 13u), *(triangleStart + 14u)); //skip w, s, t (located at indices 15-17)

        computedNormal = MeshFunc::computeNormalizedVertexNormalsForTriangle(v0, v1, v2);

        for (size_t i = 0u; i < VERTICES_IN_A_TRIANGLE; i++) { //For each of the 3 vertices of the triangle
            //Copy over the existing Position and TexCoord data  
            verticesWithNormals.push_back(*(triangleStart + (i *  POSITION_TEXCOORD_VERTEX_SIZE)));            //x
            verticesWithNormals.push_back(*(triangleStart + ((i *  POSITION_TEXCOORD_VERTEX_SIZE) + 1u)));     //y
            verticesWithNormals.push_back(*(triangleStart + ((i *  POSITION_TEXCOORD_VERTEX_SIZE) + 2u)));     //z
            verticesWithNormals.push_back(*(triangleStart + ((i *  POSITION_TEXCOORD_VERTEX_SIZE) + 3u)));     //w (aka 'scale')
            verticesWithNormals.push_back(*(triangleStart + ((i *  POSITION_TEXCOORD_VERTEX_SIZE) + 4u)));     //s
            verticesWithNormals.push_back(*(triangleStart + ((i *  POSITION_TEXCOORD_VERTEX_SIZE) + 5u)));     //t
            //Add the 3 components of the computed normal		
            verticesWithNormals.push_back(computedNormal.x);       
            verticesWithNormals.push_back(computedNormal.y);
            verticesWithNormals.push_back(computedNormal.z);
        }
    }

    verticesWithNormals.swap(mVertices_); 
    mHasNormals_ = true;
}


void QuickObj::generateMissingTextureCoordsAndNormals(bool randomizeTextureCoords, float s, float t) {
    if (!verifyVertexComponents(mVertices_.size(), POSITION_COMPONENTS * VERTICES_IN_A_TRIANGLE)) {
        fprintf(ERRLOG, "\nError! Unable to generate triangle normals for model from file: \"%s\"!\n"
            "The data was loaded fine (positions and textures) but the number of values loaded does not match\n"
            "the number needed for a whole number of triangles (%u values were loaded,\n"
            "this number must be divisible by 18 for proper normal generation!)\n\n", mFile_->getFilepath().c_str(), mVertices_.size());
        mError_ = true;
        return;
    }


    glm::vec3 v0, v1, v2, computedNormal;

    std::vector<float> verticesWithTexCoordAndNormals;
    verticesWithTexCoordAndNormals.reserve((mVertices_.size() / POSITION_COMPONENTS) * VERTICES_IN_A_TRIANGLE); //Reserve the required space 


    //Count the number of triangles for the object              //4 position-components per vertex * 3 Vertices per triangle => 12 position-components per triangle
    size_t numberOfTriangles = (mVertices_.size() / (POSITION_COMPONENTS * VERTICES_IN_A_TRIANGLE)); 

    if (!randomizeTextureCoords ) {
        //Loop through the object's data triangle by triangle
        for (size_t i = 0u; i < numberOfTriangles; i++) {
            auto triangleStart = (mVertices_.begin() + (i * (POSITION_COMPONENTS * VERTICES_IN_A_TRIANGLE)));

            v0 = glm::vec3(*(triangleStart), *(triangleStart + 1u), *(triangleStart + 2u));  //skip w (located at index 3)
            v1 = glm::vec3(*(triangleStart + 4u), *(triangleStart + 5u), *(triangleStart + 6u)); //skip w (located at index 7)
            v2 = glm::vec3(*(triangleStart + 8u), *(triangleStart + 9u), *(triangleStart + 10u)); //skip w (located at index 11)

            computedNormal = MeshFunc::computeNormalizedVertexNormalsForTriangle(v0, v1, v2);
            for (size_t j = 0u; j < VERTICES_IN_A_TRIANGLE; j++) { //For each of the 3 vertices of the triangle
                //Copy over the existing Position data 
                verticesWithTexCoordAndNormals.push_back(*(triangleStart + (j *  POSITION_VERTEX_SIZE)));            //x
                verticesWithTexCoordAndNormals.push_back(*(triangleStart + ((j *  POSITION_VERTEX_SIZE) + 1u)));     //y
                verticesWithTexCoordAndNormals.push_back(*(triangleStart + ((j *  POSITION_VERTEX_SIZE) + 2u)));     //z
                verticesWithTexCoordAndNormals.push_back(*(triangleStart + ((j *  POSITION_VERTEX_SIZE) + 3u)));     //w (aka 'scale')
                //Add the 2 texture coordinates
                verticesWithTexCoordAndNormals.push_back(s);                                                                  //s
                verticesWithTexCoordAndNormals.push_back(t);                                                                  //t
                //Add the 3-components of the computed normal
                verticesWithTexCoordAndNormals.push_back(computedNormal.x);                                                   //normal.x
                verticesWithTexCoordAndNormals.push_back(computedNormal.y);                                                   //normal.y
                verticesWithTexCoordAndNormals.push_back(computedNormal.z);                                                   //normal.z
            }
        }
    } 

    else {  //Do the same as above, except instead generate random texture coordinates
        //Loop through the object's data triangle by triangle
        for (size_t i = 0u; i < numberOfTriangles; i++) {
            auto triangleStart = (mVertices_.begin() + (i * (POSITION_COMPONENTS * VERTICES_IN_A_TRIANGLE)));

            v0 = glm::vec3(*(triangleStart), *(triangleStart + 1u), *(triangleStart + 2u));  //skip w (located at index 3)
            v1 = glm::vec3(*(triangleStart + 4u), *(triangleStart + 5u), *(triangleStart + 6u)); //skip w (located at index 7)
            v2 = glm::vec3(*(triangleStart + 8u), *(triangleStart + 9u), *(triangleStart + 10u)); //skip w (located at index 11)

            computedNormal = MeshFunc::computeNormalizedVertexNormalsForTriangle(v0, v1, v2);
            for (size_t i = 0u; i < VERTICES_IN_A_TRIANGLE; i++) { //For each of the 3 vertices of the triangle
                //Copy over the existing Position data 
                verticesWithTexCoordAndNormals.push_back(*(triangleStart + (i *  POSITION_VERTEX_SIZE)));            //x
                verticesWithTexCoordAndNormals.push_back(*(triangleStart + ((i *  POSITION_VERTEX_SIZE) + 1u)));     //y
                verticesWithTexCoordAndNormals.push_back(*(triangleStart + ((i *  POSITION_VERTEX_SIZE) + 2u)));     //z
                verticesWithTexCoordAndNormals.push_back(*(triangleStart + ((i *  POSITION_VERTEX_SIZE) + 3u)));     //w (aka 'scale')
                //Generate random values for then add the 2 texture coordinates
                verticesWithTexCoordAndNormals.push_back(MathFunc::getRandomInRangef(0.0f, 1.0f));                            //Randomized s
                verticesWithTexCoordAndNormals.push_back(MathFunc::getRandomInRangef(0.0f, 1.0f));                            //Randomized t
                //Add the 3-components of the computed normal
                verticesWithTexCoordAndNormals.push_back(computedNormal.x);                                                   //normal.x
                verticesWithTexCoordAndNormals.push_back(computedNormal.y);                                                   //normal.y
                verticesWithTexCoordAndNormals.push_back(computedNormal.z);                                                   //normal.z
            }
        }
    }

    verticesWithTexCoordAndNormals.swap(mVertices_);
    mHasNormals_ = true;
    mHasTexCoords_ = true;
}


void QuickObj::addParsedLinePrimitivesToEndOfMeshData() {

    size_t expectedVertexSize = 4u;
    if (mHasTexCoords_)
        expectedVertexSize += 2u;
    if (mHasNormals_)
        expectedVertexSize += 3u;


    const size_t MAX_POS_INDEX = mPositions_.size();

    //For each parsed line primitive
    for (auto lineIter = mLines_.cbegin(); lineIter != mLines_.cend(); lineIter++) {
        if (lineIter->dataValid()) { //Only add if data is valid
            const auto endpoints = lineIter->get();
            if ((endpoints[0] < MAX_POS_INDEX) && (endpoints[1] < MAX_POS_INDEX)) {
                Vertex p0 = mPositions_[endpoints[0]];
                Vertex p1 = mPositions_[endpoints[1]];

                addLineEndpointToVertexData(p0, expectedVertexSize);
                addLineEndpointToVertexData(p1, expectedVertexSize);
                addLineEndpointToVertexData(p0, expectedVertexSize);

            }
        }
    }
}

void QuickObj::addLineEndpointToVertexData(Vertex p, size_t expectedVertexComponents) noexcept {

    for (size_t i = 0u; i < expectedVertexComponents; i++) {
        if (i == OFFSET_TO_X)
            mVertices_.push_back(p.xVal());
        else if (i == OFFSET_TO_Y)
            mVertices_.push_back(p.yVal());
        else if (i == OFFSET_TO_Z)
            mVertices_.push_back(p.zVal());
        else if (i == OFFSET_TO_W)
            mVertices_.push_back(p.wVal());
        else if (i == OFFSET_TO_NRML_X)
            mVertices_.push_back(1.0f);
        else
            mVertices_.push_back(0.0f);
    }
}


