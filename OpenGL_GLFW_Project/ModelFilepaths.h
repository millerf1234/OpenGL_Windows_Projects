//This is just a header file containing the file paths for obj files to be loaded 
//by this app.
//This class also implements a helper class to tie filepaths together
//
//
//
// Forrest Miller
// July-November 2018

#pragma once

#ifndef MODEL_FILEPATHS_H_
#define MODEL_FILEPATHS_H_

//-----------------------------------------------------------------------------------
// NOTE:  This whole ModelFilepath class implementation I did here feels sloppy and 
//		  really is not a great design. It could do with being rewritten.
//		  (especially the union part, which is completly unnecessary). 
//        
//		  Scroll down to the bottom of this file to find the actual data filepaths
//-----------------------------------------------------------------------------------

#define PRIVATE_MEMBER_FIELDS private
#define PRIVATE_MEMBER_FUNCTIONS private

//A typedef is used to distinguish between filepath types.
namespace {  //(using an anonymous namespace restricts the scope of these typedefs to just this document)
	typedef const char * objFilepath;
	typedef const char * mtlFilepath;
	typedef const char * jpgFilepath;
	typedef const char * pngFilepath;

	typedef union ImageFP {
		jpgFilepath jpgFP;
		pngFilepath pngFP;
	} ImageFP;
}

//A class to represent a set of resource filepaths for an obj file
class ModelFilepath final {
public:
	ModelFilepath() = delete;
	ModelFilepath(objFilepath objFile) {
		initialize();
		mObjFilepath = objFile;
		mHasObj = true;
	}
	ModelFilepath(objFilepath objFile, mtlFilepath mtlFile) {
		initialize();
		mObjFilepath = objFile;
		mHasObj = true;
		mMtlFilepath = mtlFile;
		mHasMtl = true;
	}
	ModelFilepath(objFilepath objFile, mtlFilepath mtlFile, jpgFilepath jpgFile) {
		initialize();
		mObjFilepath = objFile;
		mHasObj = true;
		mMtlFilepath = mtlFile;
		mHasMtl = true;
		mImageFP.jpgFP = jpgFile;
		mHasTexture = true;
	}
	
	bool hasObj() const { return mHasObj; }
	bool hasMtl() const { return mHasMtl; }
	bool hasTex() const { return mHasTexture; }
	const char * getObjFP() const { return mObjFilepath; }
	const char * getMtlFP() const { return mMtlFilepath; }
	const char * getTexFP() const { return mImageFP.jpgFP; }

PRIVATE_MEMBER_FIELDS:
	objFilepath mObjFilepath;
	mtlFilepath mMtlFilepath;
	ImageFP mImageFP;
	
	bool mHasObj;
	bool mHasMtl;
	bool mHasTexture;

PRIVATE_MEMBER_FUNCTIONS:
	void initialize() {
		mObjFilepath = nullptr;
		mMtlFilepath = nullptr;
		mImageFP.jpgFP = nullptr;
		mHasObj = mHasMtl = mHasTexture = false;
	}
	//bool verifyFilepathExtensionObj(objFilepath fp);
	//bool verifyFilepathExtensionMtl(mtlFilepath fp);
};

#undef PRIVATE_MEMBER_FIELDS
#undef PRIVATE_MEMBER_FUNCTIONS 



//Models 
static const ModelFilepath obj_BeveledCube("obj/BeveledCube.obj", "obj/BeveledCube.mtl", "obj/BeveledCube.png");
static const ModelFilepath obj_Engine("obj/Engine2_Textured.obj", "obj/Engine2_Textured.mtl", "obj/RocketEngine_UV.png");







#endif //MODEL_FILEPATHS_H_