//This class is in charge of loading image files. This 
//allows texture files to be loaded.
//Currently only .jpg and .png files are supported...
//
// This class distinguishes between filepath errors and other
// types of error, so that a distinction exists between issues
// caused by bad files vs issues caused internally within this
// class. (actually, not really...)
//
//	IMPORTANT! This class will generate a bogus 100x100 3-component image of one color if it 
//				is unable to read from the file. 
//
// (This is basically just a wrapper class for stb_image)
//
// Created by Forrest Miller on August 11, 2018
// More work done on August 17, 2018   (in fact I got a working version completed this day)


#pragma once

#ifndef IMAGE_FILE_LOADER_H_
#define IMAGE_FILE_LOADER_H_

#include <memory>
#include <vector>
#include <stdio.h>

#include <stdexcept>

#include "LoggingMessageTargets.h"

namespace ImageLoadingInterface {

	enum class fileFormat{JPEG, PNG, UNSUPPORTED};

	typedef std::unique_ptr<std::vector<unsigned char>> uniquePtrToImageData;

	class ImageFileLoader {
	public:
		ImageFileLoader() = delete;
		ImageFileLoader(const char * imageFilepath);
		~ImageFileLoader();

		ImageFileLoader(const ImageFileLoader&) = delete;
		ImageFileLoader& operator=(const ImageFileLoader&) = delete;


		//Public Functions
		[[nodiscard]] uniquePtrToImageData getImageData() const;
		
		//Getters
		int getWidth() const { return mWidth; }
		int w() const { return mWidth; }
		int getHeight() const { return mHeight; }
		int h() const { return mHeight; }
		int getComponents() const { return mComponents; }
		bool hasAlpha() const { return (mComponents == 4); } //r, g, b, alpha 
		int getDataSize() const { return mWidth * mHeight * ((0) < 3 ? 3 : mComponents ); } //Hmm, this line doesn't make sense to me
		const char * getFilepath() const { return mFilepath; }
		bool validFilepath() const { return mValidFilepath; }
		bool error() const { return mError; }

	private:
		int mWidth, mHeight, mComponents;
		const char * mFilepath;
		
		bool mValidFilepath;        //Will be set based on the most recently loaded file
		bool mError;                //Will be true if this object encounters an internal error

		bool mBogusData;            //Will become true if made-up data is used as a safe substitute for
		//							//being asked to load an invalid/unreable file

		//I need the extra level of pointers here for this so that I can construct this object properly.
		std::unique_ptr<std::unique_ptr<unsigned char, void(*)(void*)>> stbi_data;

		//Private helper functions
		void initialize(); 
		void load(); //Requires for this object to have mFilepath set
		void createBogusButReasonableDataToReturnOnFileReadError();
	};

} //namespace ImageLoadingInterface




#endif IMAGE_FILE_LOADER_H_