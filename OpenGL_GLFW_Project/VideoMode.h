//File:                          VideoMode.h
//
//Description:                   Built around the GLFW struct GLFWvidmode, this class provides a wrapper 
//                               for tracking the properties of a single available video mode that is
//                               supported by a connected monitor. 
//
//                               Each  VideoMode of a monitor contains the following properties:
//                                      - Width and Height in screen coordinates
//                                      - Width and Height in physical size (millimeters or inches)
//                                      - Refresh Rate   (in Hz)
//                                      - Red bit depth, Green bit depth, Blue bit depth  *         
//
//
//
// Note on Bit Depths:   GLFW 3.3 documentation mentions bit depths can be modified through hints? This 
//                               has not been tested
//
//Programmer:                    Forrest Miller
//Date:                          December 2018
//
//Modifications:                 January 2, 2019  -- Added comparison operators '>' and '<' 

#pragma once

#ifndef VIDEO_MODE_H_
#define VIDEO_MODE_H_

#include "LoggingMessageTargets.h"
#include "ProjectSetup.h"
#include "ProjectConstants.h"   //Defines the constant used to convert millimeters to inches (for DPI calculation)

namespace GLFrameworkInternal {

	//This class is based off the GLFW defined type GLFWvidmode. It includes some extra 
	//capabilities from the basic GLFWvidmode struct.
	class VideoMode final {
	public:
		VideoMode() = delete;
		VideoMode(const GLFWvidmode& vid, int physicalWidthMM, int physicalHeightMM);

		~VideoMode() = default;

		VideoMode(const VideoMode&) = default;
		VideoMode(VideoMode&&) = default;
		VideoMode& operator=(const VideoMode&) = default;
		VideoMode& operator=(VideoMode&&) = default;

		//Formats this object's data into a 2-line string meant for printing to a console or log screen
		//                                          [untested, format may need editing]
		std::string toString() const;


		//////////////////////////////////////////
		//////     Comparison Operators     //////
		//////////////////////////////////////////

		//Assigns an well-defined ordering to the set of all VideoModes. Expression evaluation 
		//proceeds by performing the following comparisons, with each evaluation after the first
		//only being reached after the event of a tie from the previous:
		//   1) Compare Widths                         
		//   2) Compare Heights
		//   3) Compare Refresh Rates
		//   4) Compare sum of Red, Green and Blue bit depths
		//   5) Compare Green bit depths
		//   6) Compare Blue bit depths
		//   7) Compare Red bit depths
		//In the event that both of the VideoModes are equivalent, 'false' will be returned 
		//by default
		bool operator<(const VideoMode&) const;

		//Assigns an well-defined ordering to the set of all VideoModes. Expression evaluation 
		//proceeds by performing the following comparisons, with each evaluation after the first
		//only being reached after the event of a tie from the previous comparison:
		//   1) Compare Widths                         
		//   2) Compare Heights
		//   3) Compare Refresh Rates
		//   4) Compare sum of Red, Green and Blue bit depths
		//   5) Compare Green bit depths
		//   6) Compare Blue bit depths
		//   7) Compare Red bit depths
		//In the event that both of the VideoModes are equivalent, 'false' will be returned 
		//by default
		bool operator>(const VideoMode&) const;
		

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

		///////////////////////////////////////////////////////////
		////////         Stored Member Data Access        /////////
		///////////////////////////////////////////////////////////

		//Returns the monitor's width (in screen coordinates) for this videomode.
		//Note that screen coordinates are not guarenteed to match the monitor's 
		//pixels/resolution.
		int getWidth() const; 
		//Returns the monitor's height (in screen coordinates) for this videomode.
		//Note that screen coordinates are not guarenteed to match the monitor's 
		//pixels/resolution.
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