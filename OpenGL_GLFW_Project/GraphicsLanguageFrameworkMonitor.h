//File:          GraphicsLanguageFrameworkMonitor.h
//
// Description:    todo...
//
//  NOTE:        GLFW (as of version 3.3) does not provide a function for querrying if a monitor is connected
//               or disconnected beyond the assignable callback function. Thus it is up to any code using this
//               object to ensure that this object is notified by that callback when the monitor gets disconnected.
//
// Todo:        Add option to create a custom gamma ramp
//
// Programmer:   Forrest Miller
// Date:         December 2018

#pragma once

#ifndef GRAPHICS_LANGUAGE_FRAMEWORK_MONITOR_H_
#define GRAPHICS_LANGUAGE_FRAMEWORK_MONITOR_H_

#include <vector>
#include <memory>

#include "LoggingMessageTargets.h"
#include "ProjectSetup.h"
#include "VideoMode.h"


namespace GLFrameworkInternal {

	class GraphicsLanguageFrameworkMonitor final {
	public:
		GraphicsLanguageFrameworkMonitor() = delete;
		//Only construct objects of this type with values that were returned from glfwGetMonitors(). 
		//Once assigned it's GLFWmonitor* from the provided parameter, this value can not be changed.
		//Failure to assign a valid GLFWmonitor* will cause undefined behavior from this class.
		GraphicsLanguageFrameworkMonitor(GLFWmonitor *);
		~GraphicsLanguageFrameworkMonitor();

		GraphicsLanguageFrameworkMonitor(const GraphicsLanguageFrameworkMonitor&) = delete;
		GraphicsLanguageFrameworkMonitor(GraphicsLanguageFrameworkMonitor&&);
		GraphicsLanguageFrameworkMonitor& operator=(const GraphicsLanguageFrameworkMonitor&) = delete;
		GraphicsLanguageFrameworkMonitor& operator=(GraphicsLanguageFrameworkMonitor&&);

		//Returns a const pointer to const GLFWmonitor representing this monitor's handle. 
		const GLFWmonitor* const getMonitorHandle() const { return mHandle_; }

		//Marks the monitor as disconnected
		void disconnect() { mIsConnected_ = false; }

		//Returns the connection status of this monitor. 
		bool getIsConnected() const { return mIsConnected_; }

		//After a monitor-disconnection event by a monitor other than this one, there is a chance
		//the virtual position of this monitor object will be updated. Thus, this function can be
		//used to update this monitor's virtual screen position.
		void updateVirtualPosition();

		//Returns the Virtual Desktop screen X coordinate used for this monitor's viewport. 
		//Measured from the top left corner of monitor. Coordiante will be unique for each monitor.
		int getVirtualPositionX() const {
			return mVirtualXPos_;
		}

		//Returns the Virtual Desktop screen Y coordinate used for this monitor's viewport. 
		//Measured from the top left corner of monitor. Coordiante will be unique for each monitor.
		int getVirtualPositionY() const {
			return mVirtualYPos_;
		}

		//In case the videoMode of this monitor is changed, it is recommended that the content scale
		//values stored be updated. This function is how those values get updated.
		void updateContentScale();

		//Returns the stored content scale for this monitor. Note that changing the monitor's video mode 
		//will not automatically update this object's stored content scale, so any changes to a monitor's
		//videomode should be followed with a call to this object's member function  'updateContentScale()'
		float getContentScaleX() const {
			return mContentScaleX_;
		}

		//Returns the stored content scale for this monitor. Note that changing the monitor's video mode 
		//will not automatically update this object's stored content scale, so any changes to a monitor's
		//videomode should be followed with a call to this object's member function  'updateContentScale()'
		float getContentScaleY() const {
			return mContentScaleY_;
		}

		//Returns the number of video modes available for this monitor
		size_t getNumberOfAvailableVideoModes() const;
		//Returns the primary video mode for this monitor
		VideoMode getPrimaryVideoMode() const;
		//Returns the videomode located at the specified index from this monitors detected available
		//videomodes. Indexing beyond the end of the array will return the highest-indexed video mode. 
		VideoMode getSpecificVideoMode(size_t index) const;
		//Returns a vector containing all available video modes.
		std::vector<VideoMode> getAvailableVideoModes() const;

		//Returns true if this monitor's handle is nullptr and false otherwise
		bool handleNotNull() const { return (mHandle_ == nullptr); }

		//Should always return false in well-formed code
		bool wasMoved() const { return mWasMoved_; }

		//Compares GLFWmonitor* handles for equality. Note that if both monitors have nullptr as a handle, they will
		//compare as equal.
		bool operator==(const GraphicsLanguageFrameworkMonitor& that) const { return (mHandle_ == that.mHandle_); }
		//Compares GLFWmonitor* handles for inequality. Note that if both monitors have nullptr as a handle, they will
		//compare as equal.
		bool operator!=(const GraphicsLanguageFrameworkMonitor& that) const { return (mHandle_ != that.mHandle_); }

		//Compares this object's monitor handle with the provided monitor handle for equality. If this object's 
		//handle is nullptr and second parameter is nullptr, then equality comparison will return true.
		bool operator==(const GLFWmonitor* that) const { return (mHandle_ == that); }
		//Compares this object's monitor handle with the provided monitor handle for equality. If this object's 
		//handle is nullptr and second parameter is nullptr, then inequality comparison will return false.
		bool operator!=(const GLFWmonitor* that) const { return (mHandle_ != that); }

		//Returns the gamma ramp for this monitor. 
		GLFWgammaramp getGammaRamp() const {
			return mGammaRamp_;
		}
		//Has GLFW generate a new 256-element gamma-ramp from the specified exponent parameter. Please
		//ensure that gamma is a positive finite value. Note that the parameter 'gamma' is used as an
		//exponent when calculating the new ramp. The example code for this uses a value of 1.0f when
		//setting a gamma ramp.
		void setCustomGamma(float gamma);

	private:

		//Each GLFWmonitor object is assigned a unique handle of type GLFWmonitor* that will remain valid until the
		//monitor is disconnected or GLFW is uninitialized. Thus each object of this type can be identified by its handle
		const GLFWmonitor* mHandle_;

		//Object tracking fields
		bool mWasMoved_; //This is just to provide extra safety for invalidating an object after it has been moved
		bool mIsConnected_; 	
		bool mIsPrimary_; //GLFW will determine if a monitor is primary or not, a primary monitor typically is one that contains UI/Desktop
		
		//Monitor-specific data/specifications
		size_t mAvailableVideoModes_;
		std::vector<VideoMode> mVideoModes_;
		std::unique_ptr<VideoMode> mCurrentVideoMode_;

		//The virtual desktop position (measured in screen coordinates) is unique for each monitor.
		int mVirtualXPos_, mVirtualYPos_;
		//The content scale is the ratio between the currently in-use DPI of the monitor and the default DPI. 
		//These values will be useful for properly sizing text and other UI elements.
		float mContentScaleX_, mContentScaleY_;

		GLFWgammaramp mGammaRamp_;
		
		std::string mName_;
		


		//Private helper functions:
		void initialize(); //Gives initial values to this object's member fields
		bool checkIfIsPrimaryMonitor(); //Compares monitor handle to handle returned by glfwGetPrimaryMonitor()
		void getMonitorProperties(); //Gets monitor properties from GLFW
		

		//The following functions are called within getMonitorProperties() 
		void getMonitorName();
		void getVideoModes();
		void getMonitorPhysicalSize(int& w, int& h);
		

	};


} //namespace GLFrameworkInternal


#endif //GRAPHICS_LANGUAGE_FRAMEWORK_MONITOR_H_
