


#include "ScreenCapture.h"



ScreenCapture::ScreenCapture(const GLFWwindow* context,
                             ReadPixelsSourceTarget t,
                             ScreenCaptureImplementationStrategy s) : mContext_(context),
                                                                      mTarget_(t),
                                                                      mStrategy_(s) {



}


ScreenCapture::~ScreenCapture() noexcept {

}



const struct ScreenCapture::ScreenshotOutcome& ScreenCapture::status() const noexcept {
    return mStatus_;
}


std::vector<Timepoint> ScreenCapture::timepoints() const noexcept {
    return mTimepoints_;
}