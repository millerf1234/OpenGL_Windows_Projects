//
//  Basically this class just goes to town with the function
//  glGetNamedFramebufferParameteriv() to reveal a whole lot of 
//  information about the default framebuffer.
//  
//  Reference: https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glGetFramebufferParameter.xhtml
//
//     Calling the following function with the first parameter set to 0 reveals 
//       void glGetNamedFramebufferParameteriv(GLuint framebuffer,
//                                             GLenum pname,
//                                             GLint* param);
//
//    REQUIRES OPENGL 4.5 OR NEWER TO FUNCTION ON DEFAULT FRAMEBUFFERS
//
#pragma once

#ifndef FRAMEBUFFER_PREFERED_USAGE_H_
#define FRAMEBUFFER_PREFERED_USAGE_H_

#include <optional>

#include "GlobalIncludes.h"
#include "RestrictedOperationViolation.h"


class FramebufferPreferedUsage {
public:
    //Will throw a RestrictedOperationViolation exception if not called 
    //from a thread with a valid OpenGL context.
    FramebufferPreferedUsage(GLuint framebufferName = 0u);

    ~FramebufferPreferedUsage() noexcept;



    const bool isDefaultFramebuffer;
    bool doubleBuffered;
    //Here are the ones that matter for performing screen captures
    //GLenum implementationReadColorFormat;
    //GLenum 

#ifdef READY_TO_IMPLEMENT_CLIENT_FRAMEBUFFER_OBJECTS
    typedef struct ClientFramebufferProperties {
        int defaultWidth; //Corresponds to GL_FRAMEBUFFER_DEFAULT_WIDTH
        int defaultHeight; //Corresponds to GL_FRAMEBUFFER_DEFAULT_HEIGHT
        int defaultLayers; //Corresponds to GL_FRAMEBUFFER_DEFAULT_LAYERS
        int defaultSamples; //Corresponds to GL_FRAMEBUFFER_DEFAULT_SAMPLES
        bool defaultFixedSampleLocations; //Corresponds to GL_FRAMEBUFFER_DEFAULT_FIXED_SAMPLE_LOCATIONS
    } ClientFramebufferDetails;

    std::optional<ClientFramebufferDetails> clientFBDetails;
#endif

private:
    const GLuint mName_;
};

#endif //FRAMEBUFFER_PREFERED_USAGE_H_



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