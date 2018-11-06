//Update: So it turns out that (as of early November 2018) the class WavefrontObj in
//        the file "WavefrontObj.h" is only partially implemented and isn't yet 
//        in a usable state. This file sets up a simpler ObjLoader, that doesn't
//        have as many fancy bells and whistles but it can get the data from the 
//        obj file into an interlaced form for OpenGL. As such, this class is the 
//        one that most other code will be using.
//
//        Implementation Details:
//             This class relies heavily on several classes beneath it to do the brunt of
//             the work. This class uses an instance of the AsciiAsset class to aquire and 
//             process the file from a filepath. Also used are the classes Line and Face,
//             which are each stored in their own vector and are used to store Line and Face 
//             data from the Obj file. Note that the actual parsing of the data is done by these
//             two classes. Final assembly of the data is handled by this class. 

//I am getting the sense that I do not have the time I would like to write
//the '.obj' wrapper class I would like, so this is a quick and dirty implementation
//just to get something working. No objects or groups will be marked, and only data 
//already found in the file will be generated. No fancy functionality, just construct 
//with a filepath and get the loaded mesh data from the object.
//No material support either

#pragma once

#ifndef QUCIK_OBJ_H_
#define QUICK_OBJ_H_

#include "Line.h"           //Used internally by class
#include "Face.h"           //Used internally by class
#include "AsciiAsset.h"     //Used internally by class
#include "Vertex.h"         //Used to store data


static constexpr const size_t POSITION_INDEX = 0;
static constexpr const size_t TEXTURE_COORD_INDEX = 1;
static constexpr const size_t NORMAL_INDEX = 2;

class QuickObj {
public:
	QuickObj() = delete;
	//Construction is loading of file
	QuickObj(std::string filepath, float scale = 1.0f);
	~QuickObj();

	QuickObj(const QuickObj&) = delete;
	QuickObj(QuickObj&&) = delete;
	QuickObj& operator=(const QuickObj&) = delete;
	QuickObj& operator=(QuickObj&&) = delete;


	bool hasTexCoords() const { return mHasTexCoords_; }
	bool hasNormals() const { return mHasNormals_; }

	//The vertices will be public information for fast/easy access. Quick and dirty
	std::vector<float> mVertices_; //No draw elements or anything for now, only draw arrays
	std::vector<float> mLineEndpoints_; 

private:
	float mScale_; 
	bool mHasTexCoords_, mHasNormals_;

	std::unique_ptr<AssetLoadingInternal::AsciiAsset> mFile_;
	std::vector<AssetLoadingInternal::Face> mFaces_;
	std::vector<AssetLoadingInternal::Line> mLines_;
	
	std::vector<Vertex> mPositions_;
	std::vector<Vertex> mTexCoords_;
	std::vector<Vertex> mNormals_;

	void parseFile();
	//void preparseFile();
	void loadLineIntoVertex(const char * line, std::vector<Vertex>& verts);
	void constructVerticesFromParsedData();

	//Helper functions (borrowed from subclasses 'Face' and 'Line'):
	//Checks to see if a string character is between '0' (i.e. 48u in ASCII) and '9' (i.e. 57u in ASCII) (i.e. if it's a number)
	bool isNumber(const char * c) const { return ((*c >= static_cast<char>(48u)) && (*c <= static_cast<char>(57u))); }
	//Checks to see if a chracter is the number '0'  (to make sure a face that might look like "10/0/3" is marked invalid)
	bool isZero(const char * c) const { return (*c == static_cast<char>(48u)); }

	inline void eatWhitespace(const char * c) {
		while (*c == ' ') {
			c++;
		}
	}

	


};

#endif //QUICK_OBJ_H_