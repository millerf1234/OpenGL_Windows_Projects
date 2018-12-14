//File:                          VideoMode.h
//
//Description:                   Based off the GLFW struct GLFWvidmode, this file defines a class used to 
//                               track information about a display's available video modes.
//
//Programmer:                    Forrest Miller
//Date:                          December 2018

#pragma once

#ifndef VIDEO_MODE_H_
#define VIDEO_MODE_H_

#include "LoggingMessageTargets.h"
#include "ProjectSetup.h"
#include "ProjectConstants.h"   //Defines the constant used to convert millimeters to inches

namespace GLFrameworkInternal {

	//This class is based off the GLFW defined type GLFWvidmode. It includes some extra 
	//capabilities from the basic GLFWvidmode struct.
	class VideoMode final {
	public:
		VideoMode() = delete;
		VideoMode(const GLFWvidmode& vid, int physicalWidthMM, int physicalHeightMM);

		~VideoMode() { ; }   //Desctructor is trivial

		VideoMode(const VideoMode&) = default;
		VideoMode(VideoMode&&) = default;
		VideoMode& operator=(const VideoMode&) = default;
		VideoMode& operator=(VideoMode&&) = default;

		//Formats this object's data into a 2-line string meant for printing to a console or log screen
		//                                          [untested, format may need editing]
		std::string toString() const;

		//Compares this object with another VideoMode object for equality. All
		//fields (aside from physical screen dimensions) must match exactly for equality
		bool operator==(const VideoMode&) const;
		//Compares this object with a GLFWvidmode struct for equality
		bool operator==(const GLFWvidmode&) const;
		//Compares this object with another VideoMode object for inequality. All
		//fields (aside from physical screen dimensions) must match exactly for equality
		bool operator!=(const VideoMode&) const;
		//Compares this object with a GLFWvidmode struct for inequality
		bool operator!=(const GLFWvidmode&) const;

		//This class has a whole lot of getters

		//Returns the monitor's width (in screen coordinates) for this videomode
		int getWidth() const; 
		//Returns the monitor's height (in screen coordinates) for this videomode
		int getHeight() const;

		//Returns the monitors [approximate] physical height (in millimeters)
		int getPhysicalHeightMilliMeters() const;
		//Returns the monitors [approximate] physical width (in millimeters)
		int getPhysicalWidthMilliMeters() const;
		//Returns the monitors [approximate] physical height (in inches)
		double getPhysicalHeightInches() const;
		//Returns the monitors [approximate] physical width (in inches)
		double getPhysicalWidthInches() const;
		//Returns the monitors [approximate] physical diameter (in millimeters)
		double getPhysicalDisplaySizeMillimeters() const;
		//Returns the monitors [approximate] physical diameter (in inches)
		double getPhysicalDisplaySizeInches() const;
		//Measures the DPI (measured based off monitor's height)
		double getDPI_Height() const;
		//Measures the DPI (measured based off monitor's height)
		double getDPI_Width() const;
		//Measures the DPI (measured based off monitor's height)
		double getDPI_WidthHeightAverage() const;

		//Returns the refresh rate for this videomode
		int getRefreshRate() const;

		//Returns the red bit depth for this videomode
		int getRedBitDepth() const;
		//Returns the red bit depth for this videomode
		int getGreenBitDepth() const;
		//Returns the red bit depth for this videomode
		int getBlueBitDepth() const;

	private:
		int mWidth_, mHeight_; //Measured in screen coordinates
		int mPhysicalWidthMM_, mPhysicalHeightMM_; //measured in millimeters, not guarenteed to be accurate.
		int mRefreshRate_;
		int mRedBits_, mGreenBits_, mBlueBits_;
	};


} //namespace GLFrameworkInternal

#endif //VIDEO_MODE_H_