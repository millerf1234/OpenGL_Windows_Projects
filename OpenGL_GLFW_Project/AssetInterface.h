
//This file just contains a virtual abstract base class called AssetInterface that provides an 
//interface for asset files. This may potentially lead to a more portable asset-loading capability
//once multiple asset types are introduced. (At the time I am writing this, I haven't even completly
//implemented Wavefront obj/mtl assets yet)

//Programmer:  Forrest Miller
//Date:        October 15, 2018

#pragma once

#include "ProjectSetup.h"

class AssetInterface {
public:
	//Constructor
	AssetInterface();
	virtual ~AssetInterface();  //See my note's file on Virtual Destructors

	//No Copying is allowed for AssetInterfaces 
	AssetInterface(const AssetInterface&) = delete;
	AssetInterface(AssetInterface&&) = delete;
	AssetInterface& operator=(const AssetInterface&) = delete;
	AssetInterface& operator=(AssetInterface&&) = delete;

	//////////////////////////
	//  Virtual Interface  ///
	//////////////////////////

	//Returns the size of the loaded asset data
	virtual size_t size() const /* = 0; */ { return 0; }
	//Returns the amount of storage space required to store the asset
	virtual GLsizei requiredStorageSpace() const /* = 0 */ { return 0; }
	//Loads the asset into application memory. Most likely this will involve parsing a file.
	//Does nothing if the asset has already been loaded.
	virtual bool load() /* = 0; */ { return false; }


	////////////////////////////////////////
	//  Non-Virtual Interface Functions  ///
	////////////////////////////////////////

	//Returns true if the Asset is valid (i.e. no issues when loading the asset).
	bool valid() const { return mValid_; }
	
	bool loaded() const { return mLoaded_; }
	bool isLoaded() const { return mLoaded_; }
	explicit operator bool() const { return mLoaded_; }


protected:
	bool mLoaded_;
	bool mValid_;

};

