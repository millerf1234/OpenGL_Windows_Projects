
//
//  Basically this class just goes to town with the function
//  glGetNamedFramebufferParameteriv() to reveal a whole lot of 
//  information about the default framebuffer.
//  
//  Reference: https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glGetFramebufferParameter.xhtml
//
//  [more]                                                                       
//  References:             https://www.khronos.org/opengl/wiki/Pixel_Transfer
//                         https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glGetInternalformat.xhtml
//                     
//  [GLFW Link]         https://www.glfw.org/docs/latest/window_guide.html#window_attribs_fb
//
//     Calling the following function with the first parameter set to 0 reveals 
//       void glGetNamedFramebufferParameteriv(GLuint framebuffer,
//                                             GLenum pname,
//                                             GLint* param);
//
//     Must be constructed on the thread that has an active GL context and is
//     in charge of the default OS-provided framebuffer. After construction however,
//     there shouldn't be any problem using this object on other threads. It has no
//     internal protection against race conditions. 
//
//    REQUIRES OPENGL 4.5 OR NEWER TO FUNCTION ON DEFAULT FRAMEBUFFERS
//


#pragma once

#ifndef FRAMEBUFFER_PREFERED_USAGE_H_
#define FRAMEBUFFER_PREFERED_USAGE_H_

#include <optional>
#include <string>
#include "Timepoint.h"
#include "GlobalIncludes.h"

#include "DefaultFramebufferState.h"


class FramebufferPreferredUsage {
public:
    //Will default to the default framebuffer if no parameter is provided
    FramebufferPreferredUsage(GLuint framebufferName = 0u);

    ~FramebufferPreferredUsage() noexcept;

    FramebufferPreferredUsage(FramebufferPreferredUsage&& other) noexcept = default;

    bool isDefaultFramebuffer() const noexcept { return mIsDefaultFramebuffer_; }
    bool doubleBuffered() const noexcept { return mDoubleBuffered_; }



    /////////////////////////////////////////////////////////////////////////////
    ///  Here are the preferences that matter for performing screen captures  ///
    /////////////////////////////////////////////////////////////////////////////
    PreferredPixelReadParam getPreferredReadPixelsParameters() const noexcept;

    GLenum preferredColorReadFormat() const noexcept;  //e.g. 
    //Uses an internal lookup table to try to report the name of this GLenum.
    //Known to fail...
    std::string nameOfPreferredColorReadFormat() const noexcept;


    GLenum preferredColorReadType() const noexcept;    //e.g.
    //Uses an internal lookup table to try to report the name of this GLenum.
    //Known to fail...
    std::string nameOfPreferredColorReadType() const noexcept; 




    //Back to the less directly interesting settings...

    CurrentSamplingConfiguration getCurrentSamplingConfiguration() const noexcept { return mSampling_; }
    bool hasStereoBufferingEnabled() const noexcept { return mStereoBufferingEnabled_; }



#ifdef READY_TO_IMPLEMENT_CLIENT_FRAMEBUFFER_OBJECTS
    typedef struct ClientFramebufferProperties {
        int defaultWidth; //Corresponds to GL_FRAMEBUFFER_DEFAULT_WIDTH
        int defaultHeight; //Corresponds to GL_FRAMEBUFFER_DEFAULT_HEIGHT
        int defaultLayers; //Corresponds to GL_FRAMEBUFFER_DEFAULT_LAYERS
        int defaultSamples; //Corresponds to GL_FRAMEBUFFER_DEFAULT_SAMPLES
        bool defaultFixedSampleLocations; //Corresponds to GL_FRAMEBUFFER_DEFAULT_FIXED_SAMPLE_LOCATIONS
    } ClientFramebufferDetails;
    const std::optional<ClientFramebufferDetails> clientFBDetails;
#endif

private:
    const GLuint mName_;
    const Timepoint mTCreation_;
    std::unique_ptr<Timepoint> mTReady_;

    const bool mIsDefaultFramebuffer_;
    bool mDoubleBuffered_;
    PreferredPixelReadParam mFramebufferPreferences_;
    CurrentSamplingConfiguration mSampling_;
    bool mStereoBufferingEnabled_;
    int mMSAA_SAMPLES_;

    //This will only be available if this object is a Default Framebuffer
    std::optional<DefaultFramebufferState> mState_;

};

#endif //FRAMEBUFFER_PREFERED_USAGE_H_


//From OpenGL Documentation:
/*
GL_DOUBLEBUFFER
param returns a boolean value indicating whether double buffering is supported for the framebuffer object.

GL_IMPLEMENTATION_COLOR_READ_FORMAT
param returns a GLenum value indicating the preferred pixel data format for the framebuffer object. See glReadPixels.

GL_IMPLEMENTATION_COLOR_READ_TYPE
param returns a GLenum value indicating the implementation's preferred pixel data type for the framebuffer object. See glReadPixels.

GL_SAMPLES
param returns an integer value indicating the coverage mask size for the framebuffer object. See glSampleCoverage.

GL_SAMPLE_BUFFERS
param returns an integer value indicating the number of sample buffers associated with the framebuffer object. See glSampleCoverage.

GL_STEREO
param returns a boolean value indicating whether stereo buffers (left and right) are supported for the framebuffer object.
*/







#endif //REPORT_DETAILS_ON_GL_IMPLEMENTATION_H_


