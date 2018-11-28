//Ideas:
//This class represents a texture to be used within OpenGL.
//Very simple currently, not a whole lot of image formats supported
//and not designed for maximum streamlined efficiency.
// or....
// Could have this class just be a wrapper for the data (and the ImageFileLoader) and
// have all of the OpenGL textures created and managed by 1 Object

//Currently only supports loading JPEG and PNG files

//New idea for structure:
//  Have Texture classes be able to create themselves within
//  the OpenGL context, and then also have the texture pool class
//  available for creating multiple textures at once. If created in
//  the pool, then all of the interaction between them and other 
//  code is done through the pool.

//Created by Forrest Miller on 8/17/2018

//Update on 8/19/2018    Check out texture storage... 

#pragma once

#ifndef TEXTURE_H_
#define TEXTURE_H_

#include "ImageFileLoader.h"
#include "ProjectConstants.h"
#include "ProjectParameters.h"


enum class TextureType {
	TEXTURE_1D,
	TEXTURE_2D,
	TEXTURE_3D,
	TEXTURE_1D_ARRAY,
	TEXTURE_2D_ARRAY,
	TEXTURE_RECTANGLE,
	TEXTURE_CUBE_MAP,
	TEXTURE_CUBE_MAP_ARRAY,
	TEXTURE_BUFFER,
	TEXTURE_2D_MULTISAMPLE,
	TEXTURE_2D_MULTISAMPLE_ARRAY,
};

enum class TextureWrapParameter {
	REPEAT,
	MIRRORED_REPEAT,
	CLAMP_TO_BORDER,
	CLAMP_TO_EDGE,
};

enum class TextureFilterParameter { 
	LINEAR,
	NEAREST,   //NOTE THAT THE MIPMAP OPTIONS WILL ONLY WORK IF MIPMAPS ARE AVAILABLE
	NEAREST_MIPMAP_NEAREST,
	LINEAR_MIPMAP_NEAREST,
	NEAREST_MIPMAP_LINEAR,
	LINEAR_MIPMAP_LINEAR,
	//Bi-linear?  Tri-linear?  
};

class Texture {
public:
	//Creates a GL Texture object within the context for itself unless
	//the parameter generateTextureWithinGLContext is false. 
	Texture(TextureType type, bool generateTextureWithinGLContext = true); 
	Texture(TextureType type, const char * filepath, bool generateMipmap = true);

	///Copying is not allowed:
	Texture(const Texture&) = delete;
	Texture& operator=(const Texture&) = delete;
	
	///Moving is allowed though:
	Texture(Texture&&);
	Texture& operator=(Texture&&);

	~Texture();

	//The function loadFromFile() will return if an image has already been loaded
	//and used by this object
	void loadFromFile(const char *);


	//Texture parameter tweaking
	TextureType getTextureType() const { return mTextureType; }
	void changeWrapParameter_S(TextureWrapParameter wrapS);
	TextureWrapParameter getWrapParameter_S() const { return mWrapParameter_S; }
	void changeWrapParameter_T(TextureWrapParameter wrapT);
	TextureWrapParameter getWrapParameter_T() const { return mWrapParameter_T; }
	void changeWrapParameter_R(TextureWrapParameter wrapR);
	TextureWrapParameter getWrapParameter_R() const { return mWrapParameter_R; }


private: 
	//////////////
	//  FIELDS  //
	//////////////

	//////////////////////////////////
	//		   Object State		    //
	//////////////////////////////////
	bool mValid;
	bool mPartOfPool;
	GLuint mID;
	const char * mFilepath;

	///////////////////////////////////
	//  Texture-specific parameters  //
	///////////////////////////////////

	TextureType mTextureType; 
	TextureWrapParameter mWrapParameter_S;
	TextureWrapParameter mWrapParameter_T; //For 2-dimensional textures
	TextureWrapParameter mWrapParameter_R; //For 3-dimensional textures



	float mLODBias; //Will be 0.0f initially




	bool mHasBackgroundColor;
	glm::vec4 mBackgroundColor;
};


#endif //TEXTURE_H_
