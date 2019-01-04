//File:    GraphicsLanguageFrameworkMonitor.cpp
//    See header file for more detail

#include "GraphicsLanguageFrameworkMonitor.h"

namespace GLFrameworkInternal {

	void GraphicsLanguageFrameworkMonitor::initialize() {
		mWasMoved_ = false;
		mIsConnected_ = true;
		mIsPrimary_ = false;
		mAvailableVideoModes_ = 0u;
		mPrimaryVideoMode_ = nullptr;
		mVirtualXPos_ = 0;
		mVirtualYPos_ = 0;
		mContentScaleX_ = 1.0f;
		mContentScaleY_ = 1.0f;
		mName_ = "[UNASSIGNED_MONITOR_NAME]";
	}

	//Since a monitor requires a unique GLFWmonitor* to serve as its identifier (essentially it acts as a Primary Key),
	//it doesn't make sense to have a default constructor. Thus the default constructor is disabled (deleted).
	//GraphicsLanguageFrameworkMonitor::GraphicsLanguageFrameworkMonitor() {
	//	initialize();
	//}

	GraphicsLanguageFrameworkMonitor::GraphicsLanguageFrameworkMonitor(GLFWmonitor * handle) {

		initialize();
		if (!handle) {
			fprintf(ERRLOG, "\nERROR! A GraphicsLanguageFrameworkMonitor object's constructor was called with nullptr!\n"
				"This is a very bad thing that happened! Please review code and ensure that only valid\n"
				"Monitor handles ever get passed to this constructor!\n");
			mIsConnected_ = false;
		} 
		else {
			mHandle_ = handle;
		}

		if (checkIfIsPrimaryMonitor()) {
			mIsPrimary_ = true;
		}
		aquireMonitorDetails();
	}

	GraphicsLanguageFrameworkMonitor::~GraphicsLanguageFrameworkMonitor() {
		//for debug
		fprintf(MSGLOG, "\n\nDEBUG NOTIFICATION:: Detected that the monitor at address %p has been disconnected!\n", mHandle_);
	}

	GraphicsLanguageFrameworkMonitor::GraphicsLanguageFrameworkMonitor(GraphicsLanguageFrameworkMonitor&& that) {
		mHandle_ = that.mHandle_;
		mIsConnected_ = that.mIsConnected_;
		mIsPrimary_ = that.mIsPrimary_;
		mAvailableVideoModes_ = that.mAvailableVideoModes_;
		mVideoModes_.swap(that.mVideoModes_);
		mPrimaryVideoMode_ = std::move(that.mPrimaryVideoMode_);
		that.mPrimaryVideoMode_ = nullptr; //This is probably unnecessary...
		
		mVirtualXPos_ = that.mVirtualXPos_;
		mVirtualYPos_ = that.mVirtualYPos_;

		mContentScaleX_ = that.mContentScaleX_;
		mContentScaleY_ = that.mContentScaleY_;

		mGammaRamp_ = that.mGammaRamp_;

		mName_ = that.mName_;
		mWasMoved_ = false;
		that.mWasMoved_ = true;
	}

	GraphicsLanguageFrameworkMonitor& GraphicsLanguageFrameworkMonitor::operator=(GraphicsLanguageFrameworkMonitor&& that) {
		if (this != &that) {
			mHandle_ = that.mHandle_;
			mIsConnected_ = that.mIsConnected_;
			mIsPrimary_ = that.mIsPrimary_;
			mAvailableVideoModes_ = that.mAvailableVideoModes_;
			mVideoModes_.swap(that.mVideoModes_);
			mPrimaryVideoMode_ = std::move(that.mPrimaryVideoMode_);
			that.mPrimaryVideoMode_ = nullptr; //This is probably unnecessary...

			mVirtualXPos_ = that.mVirtualXPos_;
			mVirtualYPos_ = that.mVirtualYPos_;

			mContentScaleX_ = that.mContentScaleX_;
			mContentScaleY_ = that.mContentScaleY_;

			mGammaRamp_ = that.mGammaRamp_;

			mName_ = that.mName_;
			mWasMoved_ = false;
			that.mWasMoved_ = true;
		}
		return *this;
	}


	void GraphicsLanguageFrameworkMonitor::updateVirtualPosition() {
		glfwGetMonitorPos(const_cast<GLFWmonitor*>(mHandle_), &mVirtualXPos_, &mVirtualYPos_);
	}

	void GraphicsLanguageFrameworkMonitor::updateContentScale() {
		glfwGetMonitorContentScale(const_cast<GLFWmonitor*>(mHandle_), &mContentScaleX_, &mContentScaleY_);
	}
	
	size_t GraphicsLanguageFrameworkMonitor::getNumberOfAvailableVideoModes() const {
		return mAvailableVideoModes_;
	}
	
	VideoMode GraphicsLanguageFrameworkMonitor::getPrimaryVideoMode() const {
		if (mPrimaryVideoMode_) {
			return *mPrimaryVideoMode_;
		}
		else {
			fprintf(WRNLOG, "\nWARNING! For some reason there was no primary video mode set for monitor %s(0x%p)!\n", mName_.c_str(), mHandle_);
			return getSpecificVideoMode(0u); //This isn't the best solution here, but it also isn't the worst...
		}
	}

	VideoMode GraphicsLanguageFrameworkMonitor::getSpecificVideoMode(size_t index) const {
		if (mAvailableVideoModes_ == 0u) {
			fprintf(WRNLOG, "\nWARNING! A video mode at index %u was requested for monitor %s(handle=0x%p),\n"
				"    but no valid video mode exists at this index. A \"bogus\" video mode will generated as a placeholder!\n", index,
				mName_.c_str(), mHandle_);
			//Create and return a bogus videomode
			GLFWvidmode bogus;
			bogus.height = 1;
			bogus.width = 1;
			bogus.redBits = bogus.greenBits = bogus.blueBits = 0;
			bogus.refreshRate = 0;
			return std::move(VideoMode(bogus, 1, 1));
		}
		else if (index >= mAvailableVideoModes_) {
			return *(mVideoModes_.crend()); //This returns a copy of last element in vector of available video modes
		}
		else { //Else this function was called with a valid parameter, so return the requested video mode
			return mVideoModes_[index];
		}
	}

	std::vector<VideoMode> GraphicsLanguageFrameworkMonitor::getAvailableVideoModes() const {
		return mVideoModes_;
	}

	void GraphicsLanguageFrameworkMonitor::setCustomGamma(float gamma) {
		fprintf(MSGLOG, "\nSetting Custom Gamma Ramp of %f for monitor %s(handle=0x%p)!\n",
			gamma, mName_.c_str(), mHandle_);
		glfwSetGamma(const_cast<GLFWmonitor*>(mHandle_), gamma);
	}
	
	bool GraphicsLanguageFrameworkMonitor::checkIfIsPrimaryMonitor() {
		return (*this == glfwGetPrimaryMonitor()); //Uses overloaded 'operator==' with GLFWmonitor* as second parameter 
	}


	void GraphicsLanguageFrameworkMonitor::aquireMonitorDetails() {
		getMonitorName();
		getVideoModes();
		updateVirtualPosition();
		updateContentScale();
	}

	void GraphicsLanguageFrameworkMonitor::getMonitorName() {
		const char * name = glfwGetMonitorName(const_cast<GLFWmonitor*>(mHandle_));
		if ( (name != NULL) && (*name != '\0') ) {
			mName_ = name;
		}
	}

	void GraphicsLanguageFrameworkMonitor::getVideoModes() {
		int availableVideoModes = 0;
		const GLFWvidmode * arrayOfVidModes = glfwGetVideoModes(const_cast<GLFWmonitor*>(mHandle_), &availableVideoModes);
		if ( (arrayOfVidModes == nullptr) || (availableVideoModes <= 0)) {
			fprintf(ERRLOG, "\nError: Unable to obtain video modes for monitor %s(0x%p)\n", mName_.c_str(), mHandle_);
			return;
		}

		//else
		mAvailableVideoModes_ = static_cast<size_t>(availableVideoModes);
		int physicalSizeX = 1;
		int physicalSizeY = 1;
		getMonitorPhysicalSize(physicalSizeX, physicalSizeY);
		for (size_t i = 0; i < mAvailableVideoModes_; i++) {
			mVideoModes_.emplace_back(arrayOfVidModes[i], physicalSizeX, physicalSizeY);
		}

		//Last of all, get the primary video mode
		const GLFWvidmode* primary = glfwGetVideoMode(const_cast<GLFWmonitor*>(mHandle_));
		mPrimaryVideoMode_ = std::make_unique<VideoMode>(*primary, physicalSizeX, physicalSizeY);
	}

	void GraphicsLanguageFrameworkMonitor::getMonitorPhysicalSize(int& w, int& h) {
		glfwGetMonitorPhysicalSize(const_cast<GLFWmonitor*>(mHandle_), &w, &h);
	}

} //namespace GLFrameworkInternal
