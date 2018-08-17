//This class is in charge of loading image files. This 
//allows texture files to be loaded.
//Currently only .jpg and .png files are supported...
//
// This class distinguishes between filepath errors and other
// types of error, so that a distinction exists between issues
// caused by bad files vs issues caused internally within this
// class.
//
// (This is basically just a wrapper class for stb_image)
//
// Created by Forrest Miller on August 11, 2018
// More work done on August 17, 2018
//
//
//  I have decided to do things differently than what I wrote here in "Thoughts"
//  Thoughts: So this class can exist in several different states. 
//			First, it can be created with no parameters to put it
//		    in a ready-to-load-file state. 
//          Second, it can be created with a filepath parameter to
//			get it to load the provided file. 
//			

#pragma once

#ifndef IMAGE_FILE_LOADER_H_
#define IMAGE_FILE_LOADER_H_

#include <memory>
#include <vector>
#include <stdio.h>

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
		//void loadFile(const char * imageFilepath);
		
		//Getters
		int getWidth() const { return mWidth; }
		int w() const { return mWidth; }
		int getHeight() const { return mHeight; }
		int h() const { return mHeight; }
		int getComponents() const { return mComponents; }
		bool hasAlpha() const { return (mComponents == 4); } //r, g, b, alpha 
		int getDataSize() const { return mWidth * mHeight * ((0) < STBI_rgb ? STBI_rgb : mComponents ); } //Hmm, this line doesn't make sense to me
		const char * getFilepath() const { return mFilepath; }
		bool validFilepath() const { return mValidFilepath; }
		bool error() const { return mError; }

	private:
		int mWidth, mHeight, mComponents;
		const char * mFilepath;
		//std::unique_ptr<std::vector<unsigned char>> mImageData;
		
		bool mValidFilepath;        //Will be set based on the most recently loaded file
		bool mError;                //Will be true if this object encounters an internal error
		//bool mHasLoadedImageData; //Will be true when this object has loaded an image.

		std::unique_ptr<unsigned char, void(*)(void*)> stbi_data;

		//Private helper functions
		void initialize(); 
	};

} //namespace ImageLoadingInterface




#endif IMAGE_FILE_LOADER_H_