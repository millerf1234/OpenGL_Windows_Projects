//Implementation for the ImageFileLoader class. 
//This file is the only source file that should use stb_image.h. 
//Any other source or header file that needs stb_image.h must go
//through this class. 
//
// Implementation started by Forrest Miller on 8/17/2018
//
//

#include "ImageFileLoader.h"

//stb_image stuff
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

static constexpr int NOT_SET = -1; //Used for image property data before they
//									  get written to by stb_image 

namespace ImageLoadingInterface {

	/*ImageFileLoader::ImageFileLoader() {
		initialize();
	}*/

	ImageFileLoader::ImageFileLoader(const char * imageFilepath) {
		initialize();
	}
	
	ImageFileLoader::~ImageFileLoader() {

	}

	//void ImageFileLoader::loadFile(const char * imageFilepath) {
	//}

	[[nodiscard]] uniquePtrToImageData ImageFileLoader::getImageData() const {
		uniquePtrToImageData imageData = std::make_unique<std::vector<unsigned char>> ();

	}

	void ImageFileLoader::initialize() {
		mWidth = mHeight = NOT_SET;
		mComponents = NOT_SET;
		mFilepath = "\0";
		stbi_data = nullptr;
		mValidFilepath = false;
		mError = false;

	}

}  //namespace ImageLoadingInterface


