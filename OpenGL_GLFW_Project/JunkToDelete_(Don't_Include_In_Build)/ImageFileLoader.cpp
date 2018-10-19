//Implementation for the ImageFileLoader class. 
//This file is the only source file that should use stb_image.h. 
//Any other source or header file that needs stb_image.h must go
//through this class. 
//
// Implementation by Forrest Miller on 8/17/2018
//
//

#include "ImageFileLoader.h"

//stb_image stuff
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

static constexpr int NOT_SET = -1; //Used for image property data before they
//									  get written to by stb_image 

namespace ImageLoadingInterface {

	ImageFileLoader::ImageFileLoader(const char * imageFilepath) {
		initialize();
		//Make sure filepath isn't null
		if (imageFilepath == nullptr) {
			mError = true;
			mValidFilepath = false;
			createBogusButReasonableDataToReturnOnFileReadError();
		}
		//else
		mFilepath = imageFilepath;
		load();
	}
	
	ImageFileLoader::~ImageFileLoader() {

	}

	[[nodiscard]] uniquePtrToImageData ImageFileLoader::getImageData() const {
		uniquePtrToImageData imageData = std::make_unique<std::vector<unsigned char>>();
		
		//Check to see if safe to return loaded data 
		if (mBogusData) {
			fprintf(WRNLOG, "Warning! Attempting to load bogus image data from "
				"file:\n\t%s\n", mFilepath);
			fprintf(WRNLOG, "Since the data is bogus, it is fine to replace it with\n"
				"reasonable bogus...\n");
		}
		else if (!mValidFilepath || mError) { //This should never happen
			fprintf(ERRLOG, "\nThe Bogus data safety net failed for some reason!\nprogram will crash...");
			throw new std::logic_error("Bad Failsafe in Image Loader!\n");
		}

		int imageDataSize = getDataSize();
		imageData->reserve(imageDataSize);

		//Here is where the bogus data is generated for returning...
		if (mBogusData) {
			//Create the bogus data
			for (int i = 0; i < imageDataSize; i++) {
				(*imageData)[i] = '\xfe'; //some bogus value
			}
		}
		else {
			std::memcpy(imageData->data(), stbi_data->get(), imageDataSize); //Copy the data
		}
		return std::move(imageData);
	}

	void ImageFileLoader::initialize() {
		mWidth = mHeight = NOT_SET;
		mComponents = NOT_SET;
		mFilepath = "\0";
		stbi_data = nullptr;
		mValidFilepath = false;
		mError = false;
		mBogusData = false;

	}

	void ImageFileLoader::load() {
		//Create the pointer to the pointer to the STBI_data
		stbi_data = std::make_unique <std::unique_ptr<unsigned char, void(*)(void*)>> (
			stbi_load(mFilepath, &mWidth, &mHeight, &mComponents, STBI_rgb), stbi_image_free);
		
		if ((*stbi_data) == nullptr) {
			fprintf(WRNLOG, "Warning! An issue occured while loading image file: %s\n", mFilepath);
			mValidFilepath = false;
			createBogusButReasonableDataToReturnOnFileReadError();
			return;
		}
		mValidFilepath = true;
	}

	void ImageFileLoader::createBogusButReasonableDataToReturnOnFileReadError() {
		mBogusData = true;
		mComponents = 3;
		mWidth = 100;
		mHeight = 100;
	}

}  //namespace ImageLoadingInterface


