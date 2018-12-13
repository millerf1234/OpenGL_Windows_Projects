//  DISCLAIMER:    This class exists a few steps below the usual quality and thought I usually expect from my code. 
//                 There are quite a few poor design choices I made in the name of getting this up and running quickly [hence the name].
//                 There are parts of this class that are not fully implemented. Also, the algorithm it uses is slow and performs multiple 
//                 unnecessary copies. A better model-loading class system is in the pipeline once some more urgent pieces are put into place.
//                 Ideally one day there will be a class that loads models, which is run through a class that can load multiple models into the same
//                 buffer, that is then sent to the GPU using a better method than the current old-school glBuuferData()  [Preferably something
//                 involving mapping a buffer]
//
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
//No material support either (as of for now)
//
//This class is not safe! It can easily be put into an invalid state through improper use. 


#pragma once

#ifndef QUCIK_OBJ_H_
#define QUICK_OBJ_H_

#include "Line.h"           //Used internally by class
#include "Face.h"           //Used internally by class
#include "AsciiAsset.h"     //Used internally by class
#include "Vertex.h"         //Used to store data

#include "MathFunctions.h"  //For random number generation
#include "MeshFunctions.h"  //For generating normals 


class QuickObj final{
public:
	//////////////////////////
	////   Constructors   ////
	//////////////////////////
	//A filepath to an '.obj' file is required for object construction
	QuickObj() = delete;
	//Tries to parse and load the '.obj' resource file into this object's memory. Object data is loaded
	//as-is, with scale being placed as the 'w' component to each loaded position. Use member functions 
	//hasTexCoords() and hasNormals() to determine what was loaded from the file. Each loaded vertex will
	//consist of 4 position components plus 2 texture components (if texture coordinates were part of the loaded 
	//data) and 3 normal components (if normals were loaded). These values are all interlaced. 
	QuickObj(std::string filepath, float scale = 1.0f);
	//Tries to parse and load the '.obj' resource file into this object's memory. If the object is missing either normal and/or 
	//texture-coordinate data, appropriate data will be loaded in their place. Normal's are computed on a triangle-by-triangle basis. 
	//Texture coordinates can either be randomly assigned from within the closed interval [0.0, 1.0] or all assigned the same value as 
	//determined by the parameters s and t. 
	QuickObj(std::string filepath, float scale, bool generateMissingComponents, bool randomizeTextureCoords = true, float s = 0.5f, float t = 0.5f);


	//////////////////////////
	////    Destructor    ////
	//////////////////////////
	~QuickObj();

	QuickObj(const QuickObj&) = delete;
	QuickObj(QuickObj&&) = delete;
	QuickObj& operator=(const QuickObj&) = delete;
	QuickObj& operator=(QuickObj&&) = delete;


	bool hasTexCoords() const { return mHasTexCoords_; }
	bool hasNormals() const { return mHasNormals_; }

	bool error() const { return mError_; }

	//Returns the scale [the 'w' component of each vertice's position] of the model.
	float getScale() const { return mScale_; }

	//The vertices will be public information for fast/easy access. Quick and dirty.
	//Note that vertices should not be modified by external code (unless you really know 
	//what you are doing). The reason these are not encapsulated within the class is to 
	//provide a quick way to access the data with a pointer for OpenGL's C API. 
	std::vector<float> mVertices_; 
	std::vector<float> mLineEndpoints_; 

private:
	bool mError_;
	float mScale_; 
	bool mHasTexCoords_, mHasNormals_;

	std::unique_ptr<AssetLoadingInternal::AsciiAsset> mFile_;
	std::vector<AssetLoadingInternal::Face> mFaces_;
	std::vector<AssetLoadingInternal::Line> mLines_;
	
	std::vector<Vertex> mPositions_;
	std::vector<Vertex> mTexCoords_;
	std::vector<Vertex> mNormals_;
	//This fourth type is probably very rare and won't ever be encountered...
	//std::vector<Vertex> mFreeformGeometryCoords_; //If line is of format "vp u v w\n", then it is freeform geometry.

	void parseFile();
	//void preparseFile();
	void loadLineIntoVertex(const char * line, std::vector<Vertex>& verts);
	void constructVerticesFromParsedData();

	//Helper functions (borrowed from subclasses 'Face' and 'Line'):
	//Checks to see if a string character is between '0' (i.e. 48u in ASCII) and '9' (i.e. 57u in ASCII) (i.e. if it's a number)
	inline bool isNumber(const char * c) const { return ((*c >= static_cast<char>(48u)) && (*c <= static_cast<char>(57u))); }
	//Checks to see if a chracter is the number '0'  (to make sure a face that might look like "10/0/3" is marked invalid)
	inline bool isZero(const char * c) const { return (*c == static_cast<char>(48u)); }

	inline void eatWhitespace(const char * c) {
		while (*c == ' ') {
			c++; 
		}
	}

	//Checks to make sure the number of data points in the vector of vertex data is divisible by
	//the expected vertex size
	inline bool verifyVertexComponents(size_t verticesSize, size_t expectedComponents) const {
		return ((verticesSize % expectedComponents) == 0u);
	}

	/////////////////////////////////////////////////////////////////////
	// Functions for missing component (normal/textCoord) generation 
	/////////////////////////////////////////////////////////////////////
	
	//Call this function only after determining that either Normals and/or TexCoords are missing
	void addMissingComponents(bool randomizeTextureCoords, float s, float t);
	
	//Call this function only once it has been verified that 4-positions and 3-normals
	//exist for each vertex in mVertices_.
	void generateMissingTextureCoords(bool randomizeTextureCoords, float s, float t);

	//Call this function only once it has been verified that 4-positions and 2-textureCoordinates
	//exist for each vertex in mVertices_.
	void generateMissingNormals();

	//Call this function only once it has been verified that only 4-positions
	//exist for each vertex in mVertices_.
	void generateMissingTextureCoordsAndNormals(bool randomizeTextureCoords, float s, float t);
};

#endif //QUICK_OBJ_H_