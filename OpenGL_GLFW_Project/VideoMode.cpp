//File:                         VideoMode.h
//
//Description:                 Class that holds the information pertaining to one of a monitors available VideoModes. 
//                             Is based off the GLFW type GLFWvidmode. See header file for more detail.
//
//Programmer:                  Forrest Miller
//Date:                        December 14, 2018

#include "VideoMode.h"

#include <sstream>

namespace GLFrameworkInternal {

	VideoMode::VideoMode(const GLFWvidmode& vid, int physicalWidthMM, int physicalHeightMM) {
		mPhysicalHeightMM_ = physicalHeightMM;
		mPhysicalWidthMM_ = physicalWidthMM;
		mWidth_ = vid.width;
		mHeight_ = vid.height;
		mRedBits_ = vid.redBits;
		mGreenBits_ = vid.greenBits;
		mBlueBits_ = vid.blueBits;
		mRefreshRate_ = vid.refreshRate;
	}

	std::string VideoMode::toString() const {
		std::stringstream vidMode;
		vidMode << "DPI(height)=" << getDPI_Height() << ", DPI(width)=" << getDPI_Width() << ", DPI(avg)=" << getDPI_WidthHeightAverage();
		vidMode	<< "  Refresh Rate: " << mRefreshRate_ << " hz,\t  [in screen coord] h: " << mHeight_ << ", w: ";
		vidMode << mWidth_ << "\n\tColor Bit Depth:  R=" << mRedBits_ << " bits, G=" << mGreenBits_ << " bits, B=";
		vidMode << mBlueBits_ << " bits";
		return vidMode.str();
	}

	bool VideoMode::operator==(const VideoMode& that) const {
		if (this == &that) { return true; }
		
		//else
		if (mRefreshRate_ == that.mRefreshRate_) {
			if (mWidth_ == that.mWidth_) {
				if (mHeight_ == that.mHeight_) {
					if ((mRedBits_ == that.mRedBits_) && (mGreenBits_ == that.mGreenBits_) && (mBlueBits_ == that.mBlueBits_)) {
						return true;
					}

				}
			}
		}
		return false;
	}
	
	bool VideoMode::operator==(const GLFWvidmode& that) const {
		if (mWidth_ == that.width) {
			if (mHeight_ == that.height) {
				if (mRefreshRate_ == that.refreshRate) {
					if ((mRedBits_ == that.redBits) && (mGreenBits_ == that.greenBits) && (mBlueBits_ == that.blueBits)) {
						return true;
					}
				}
			}
		}
		return false;
	}
	
	bool VideoMode::operator!=(const VideoMode& that) const {
		if (this == &that) { return false; }

		//else
		if (mRefreshRate_ == that.mRefreshRate_) {
			if (mWidth_ == that.mWidth_) {
				if (mHeight_ == that.mHeight_) {
					if ((mRedBits_ == that.mRedBits_) && (mGreenBits_ == that.mGreenBits_) && (mBlueBits_ == that.mBlueBits_)) {
						return false;
					}

				}
			}
		}
		return true;
	}

	bool VideoMode::operator!=(const GLFWvidmode& that) const {
		if (mWidth_ == that.width) {
			if (mHeight_ == that.height) {
				if (mRefreshRate_ == that.refreshRate) {
					if ((mRedBits_ == that.redBits) && (mGreenBits_ == that.greenBits) && (mBlueBits_ == that.blueBits)) {
						return false;
					}
				}
			}
		}
		return true;
	}

	
	int VideoMode::getWidth() const {
		return mWidth_;
	}
	
	int VideoMode::getHeight() const {
		return mHeight_;
	}

	int VideoMode::getPhysicalHeightMilliMeters() const {
		return mPhysicalHeightMM_;
	}
	
	int VideoMode::getPhysicalWidthMilliMeters() const {
		return mPhysicalWidthMM_;
	}
	
	double VideoMode::getPhysicalHeightInches() const {
		return (static_cast<double>(mHeight_) / MILLIMETERS_PER_INCH);
	}
	
	double VideoMode::getPhysicalWidthInches() const {
		return (static_cast<double>(mWidth_) / MILLIMETERS_PER_INCH);
	}
	
	double VideoMode::getPhysicalDisplaySizeMillimeters() const {
		double heightSquared = pow(static_cast<double>(mHeight_), 2.0);
		double widthSquared = pow(static_cast<double>(mWidth_), 2.0);
		return sqrt(heightSquared + widthSquared);
	}
	
	double VideoMode::getPhysicalDisplaySizeInches() const {
		double heightSquared = pow(getPhysicalHeightInches(), 2.0);
		double widthSquared = pow(getPhysicalWidthInches(), 2.0);
		return sqrt(heightSquared + widthSquared);
	}

	double VideoMode::getDPI_Height() const {
		double heightInches = getPhysicalHeightInches();
		if (heightInches == 0) {
			return 0;
		}
		else {
			return (static_cast<double>(mHeight_) / getPhysicalHeightInches());
		}	
	}

	double VideoMode::getDPI_Width() const {
		double widthInches = getPhysicalWidthInches();
		if (widthInches == 0) {
			return 0;
		}
		else {
			return (static_cast<double>(mWidth_) / getPhysicalWidthInches());
		}
	}

	double VideoMode::getDPI_WidthHeightAverage() const {
		return ((getDPI_Height() + getDPI_Width()) / 2.0 );
	}

	int VideoMode::getRefreshRate() const {
		return mRefreshRate_;
	}

	
	int VideoMode::getRedBitDepth() const {
		return mRedBits_;
	}
	
	int VideoMode::getGreenBitDepth() const {
		return mGreenBits_;
	}
	
	int VideoMode::getBlueBitDepth() const {
		return mBlueBits_;
	}


} //namespace GLFrameworkInternal
