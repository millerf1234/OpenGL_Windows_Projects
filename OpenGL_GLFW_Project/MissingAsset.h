//File:           MissingAsset.h
//Programmer:     Forrest Miller
//Date:           November 1, 2018     (I am adding this WAY too late into the projects development. Oh well...)
//
//Description:    Class that represents a missing asset. Chances are instances of
//                this object type will be created and reported if an asset file is
//                unlocatable because its name changed. 

#pragma once

#ifndef MISSING_ASSET_H_
#define MISSING_ASSET_H_

#include "LoggingMessageTargets.h"
#include <string>



class MissingAsset {
public:
	MissingAsset();
	MissingAsset(const char * tag);
	MissingAsset(const std::string& tag);
	~MissingAsset();

	//Gets the name of the missing asset
	const char * name() const;

private:
	std::string mAssetTag_; //Chances are this is a filepath, but could be anything 

};



#endif //MISSING_ASSET_H_

