


#include "FramebufferPreferredUsage.h"
#include "OpenGLEnumToString.h"



namespace {
    const std::string creationMsgPt1 = "Beginning Analysis Of Preferences Of FB #";
}



FramebufferPreferredUsage::FramebufferPreferredUsage(GLuint framebufferName) :
                                                   mName_(framebufferName),
                                                   mIsDefaultFramebuffer_(0u==framebufferName),
                                                   mTCreation_(creationMsgPt1 + 
                                                              std::to_string(framebufferName)),
                                                   mTReady_(nullptr) {

    //Need a Default value to reset to between queries to prevent cross-interference. 
    //Best to play it safe and go with an obscure value such as '-37' because the 
    //documentation for these query functions is massive and confusing and I have no
    //idea what to expect.
    constexpr GLint NOT_SET = -37; 

    assert(glfwGetCurrentContext());
    GLint queryResult = NOT_SET;
    //Query the framebuffer to see if it is double buffered
    glGetNamedFramebufferParameteriv(mName_, GL_DOUBLEBUFFER, &queryResult);
    assert(queryResult != NOT_SET);
    if (queryResult == GL_TRUE)
        mDoubleBuffered_ = true;
    else if (queryResult == GL_FALSE)
        mDoubleBuffered_ = false;
    else {
        fprintf(ERRLOG, "\nglGetNamedFramebufferParameteriv() returned an unexpected value!");
        assert(false);
    }

    //Repeat for the additional members
    queryResult = NOT_SET;
    glGetNamedFramebufferParameteriv(mName_, GL_IMPLEMENTATION_COLOR_READ_FORMAT, &queryResult);
    assert(queryResult != NOT_SET);
    mFramebufferPreferences_.preferredFormat = static_cast<GLenum>(queryResult);

    queryResult = NOT_SET;
    glGetNamedFramebufferParameteriv(mName_, GL_IMPLEMENTATION_COLOR_READ_TYPE, &queryResult);
    assert(queryResult != NOT_SET);
    mFramebufferPreferences_.preferredType = static_cast<GLenum>(queryResult);

    queryResult = NOT_SET;
    glGetNamedFramebufferParameteriv(mName_, GL_SAMPLES, &queryResult);
    assert(queryResult != NOT_SET);
    mSampling_.samples = queryResult;
   
    queryResult = NOT_SET;
    glGetNamedFramebufferParameteriv(mName_, GL_SAMPLE_BUFFERS, &queryResult);
    assert(queryResult != NOT_SET);
    mSampling_.sampleBuffers = queryResult;

    queryResult = NOT_SET;
    glGetNamedFramebufferParameteriv(mName_, GL_STEREO, &queryResult);
    assert(queryResult != NOT_SET);
    if (queryResult == GL_TRUE)
        mStereoBufferingEnabled_ = true;
    else if (queryResult == GL_FALSE)
        mStereoBufferingEnabled_ = false;
    else {
        fprintf(ERRLOG, "\nglGetNamedFramebufferParameteriv() returned an unexpected value!");
        assert(false);
    }

    //Get MSAA
    queryResult = NOT_SET;
    glGetIntegerv(GL_SAMPLES, &queryResult);
    assert(queryResult != NOT_SET);
    mMSAA_SAMPLES_ = queryResult;


#ifdef INCLUDE_NAME
    mFramebufferPreferences_.preferredFormatName = 
        convertGLEnumToString(mFramebufferPreferences_.preferredFormat);
    
    mFramebufferPreferences_.preferredTypeName = 
        convertGLEnumToString(mFramebufferPreferences_.preferredType);
  //  mFramebufferPreferences_.preferredFormatName =
  //      internalFormatEnumToString(mFramebufferPreferences_.preferredFormat);
  //  mFramebufferPreferences_.preferredTypeName =
  //      getType(mFramebufferPreferences_.preferredType);
#endif 


        mTReady_ = std::make_unique<Timepoint>("Analysis of Preferences for Framebuffer #" + std::to_string(mName_) + " Complete");
        //Check to see if we are a default framebuffer. If we are, release the newly 
        //created Timepoint because we got some more initialization to get through.
        if (mIsDefaultFramebuffer_) {
            mTReady_.release();
            mTReady_ = nullptr;
        }

    if (mIsDefaultFramebuffer_)
        mState_.emplace(DefaultFramebufferState());
    else
        mState_ = std::nullopt;


    //For a default framebuffer, we now follow the GLFW documentation's
    //guidance and learn about our default Framebuffer.
    if (mIsDefaultFramebuffer_) {
        assert(mState_.has_value());
        auto defFBState = mState_.value();
        mTReady_ = std::make_unique<Timepoint>("Acquired Attachments Of Default Framebuffer #" + std::to_string(mName_));
    }
}

FramebufferPreferredUsage::~FramebufferPreferredUsage() noexcept {

}


PreferredPixelReadParam FramebufferPreferredUsage::getPreferredReadPixelsParameters() const noexcept {
    return mFramebufferPreferences_;
}


GLenum FramebufferPreferredUsage::preferredColorReadFormat() const noexcept {
    return mFramebufferPreferences_.preferredFormat;
}

std::string FramebufferPreferredUsage::nameOfPreferredColorReadFormat() const noexcept {
    return convertGLEnumToString(mFramebufferPreferences_.preferredFormat);
}


GLenum FramebufferPreferredUsage::preferredColorReadType() const noexcept {
    return mFramebufferPreferences_.preferredType;
}

std::string FramebufferPreferredUsage::nameOfPreferredColorReadType() const noexcept {
    return convertGLEnumToString(mFramebufferPreferences_.preferredType);
}






