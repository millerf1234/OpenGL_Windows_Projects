


#include "FramebufferPreferredUsage.h"




namespace {
    const std::string creationMsgPt1 = "Constructing Framebuffer Preferences Object for FB #";
}


//Prototypes for Implementation-only functions 
std::string getNameOfGLEnum(GLenum hexEnum) noexcept;
std::string internalFormatEnumToString(GLenum format) noexcept;
std::string getType(GLenum type) noexcept;
std::string checkAgainstOtherValues(GLenum hexEnum) noexcept;



FramebufferPreferredUsage::FramebufferPreferredUsage(GLuint framebufferName) :
                                                   mName_(framebufferName),
                                                   mIsDefaultFramebuffer_(0u == framebufferName),
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


#ifdef INCLUDE_NAME
    mFramebufferPreferences_.preferredFormatName = 
        getNameOfGLEnum(mFramebufferPreferences_.preferredFormat);
    
    mFramebufferPreferences_.preferredTypeName = 
        getNameOfGLEnum(mFramebufferPreferences_.preferredType);
  //  mFramebufferPreferences_.preferredFormatName =
  //      internalFormatEnumToString(mFramebufferPreferences_.preferredFormat);
  //  mFramebufferPreferences_.preferredTypeName =
  //      getType(mFramebufferPreferences_.preferredType);
#endif 


    try {
        mTReady_ = std::make_unique<Timepoint>("Finished Querying All Basic Preferences for Framebuffer #" + std::to_string(mName_));
        //Check to see if we are a default framebuffer. If we are, release the newly 
        //created timepoint because we got some more initialization to get through.
        if (mIsDefaultFramebuffer_) {
            mTReady_.release();
            mTReady_ = nullptr;
        }
    }
    catch (const std::bad_alloc& e) {
        fprintf(ERRLOG, "\nIt looks like a bad allocation just occurred!\nMsg: %s\n",
            e.what());
    }


    if (mIsDefaultFramebuffer_)
        mState_.emplace(DefaultFBState());
    else
        mState_ = std::nullopt;


    //For a default framebuffer, we now follow the GLFW documentation's
    //guidance and learn about our default Framebuffer.
    if (mIsDefaultFramebuffer_) {
        assert(mState_.has_value());
        
        auto defFBState = mState_.value();

        try {
            mTReady_ = std::make_unique<Timepoint>("Finished Querying All Available Attachments Of This Default Framebuffer #" + std::to_string(mName_));
        } catch (const std::bad_alloc& e) {
            fprintf(ERRLOG, "\nBad Alloc! %s\n", e.what());
            std::exit(EXIT_FAILURE);
        }
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
    return internalFormatEnumToString(mFramebufferPreferences_.preferredFormat);
}


GLenum FramebufferPreferredUsage::preferredColorReadType() const noexcept {
    return mFramebufferPreferences_.preferredType;
}

std::string FramebufferPreferredUsage::nameOfPreferredColorReadType() const noexcept {
    return getType(mFramebufferPreferences_.preferredType);
}








static constexpr const char* unknownFormatName = "UNKNOWN FORMAT";

std::string getNameOfGLEnum(GLenum hexEnum) noexcept {
    std::string name = getType(hexEnum);
    if (name != std::string("UNRECOGNIZED TYPE"))
        return name;
    
    name = internalFormatEnumToString(hexEnum);
    if (name != unknownFormatName)
        return name;

    return name;
}

std::string getType(GLenum type) noexcept {
    fprintf(MSGLOG, "\n\n\n"
        "      DEBUG ~~~> The Preferred glReadPixels() type by\n"
        "                  this implementations has value %#X\n\n\n",
        type);
    std::string typeName;
    switch (type) {
    default:
        typeName = "UNRECOGNIZED TYPE";
        break;
    case (GL_RED):
        typeName = "GL_RED";
        break;
    case (GL_RG):
        typeName = "GL_RG";
        break;
    case (GL_RGB):
        typeName = "GL_RGB";
        break;
    case (GL_BGR):
        typeName = "GL_BGR";
        break;
    case (GL_RGBA):
        typeName = "GL_RGBA";
        break;
    case (GL_BGRA):
        typeName = "GL_BGRA";
        break;
    case (GL_RED_INTEGER):
        typeName = "GL_RED_INTEGER";
        break;
    case (GL_RG_INTEGER):
        typeName = "GL_RG_INTEGER";
        break;
    case (GL_RGB_INTEGER):
        typeName = "GL_RGB_INTEGER";
        break;
    case (GL_BGR_INTEGER):
        typeName = "GL_BGR_INTEGER";
        break;
    case (GL_RGBA_INTEGER):
        typeName = "GL_RGBA_INTEGER";
        break;
    case (GL_BGRA_INTEGER):
        typeName = "GL_BGRA_INTEGER";
        break;
    case (GL_STENCIL_INDEX):
        typeName = "GL_STENCIL_INDEX";
        break;
    case (GL_DEPTH_COMPONENT):
        typeName = "GL_DEPTH_COMPONENT";
        break;
    case (GL_DEPTH_STENCIL):
        typeName = "GL_DEPTH_STENCIL";
        break;
        
        // There are Also Data Types Which Might Be Considered Same Thing

    case(GL_BYTE):             //0x1400
        typeName = "GL_UNSIGNED_BYTE";
        break;
    case(GL_UNSIGNED_BYTE):    //0x1401
        typeName = "GL_UNSIGNED_BYTE";
        break;
    case(GL_SHORT):            //0x1402
        typeName = "GL_SHORT";
        break;
    case(GL_UNSIGNED_SHORT):   //0x1403
        typeName = "GL_UNSIGNED_SHORT";
        break;
    case(GL_FLOAT):            //0x1406
        typeName = "GL_FLOAT";
        break;
    case(GL_FIXED):            //0x140C
        typeName = "GL_FIXED";
        break;
    }
    
    return typeName;
}





std::string internalFormatEnumToString(GLenum format) noexcept {
    fprintf(MSGLOG, "\n\n\n"
        "      DEBUG ~~~> The Preferred glReadPixels() format by\n"
        "                  this implementations has value %#X\n\n\n",
        format);
    std::string name;
    switch (format) {
    default:
        name = unknownFormatName;
        break;
    case GL_R8:
        name = "GL_R8";
        break;
    case GL_R8_SNORM:
        name = "GL_R8_SNORM";
        break;
    case GL_R16:
        name = "GL_R16";
        break;
    case GL_R16_SNORM:
        name = "GL_R16_SNORM";
        break;
    case GL_R3_G3_B2:
        name = "GL_R3_G3_B2";
        break;
    case GL_RGB4:
        name = "GL_RGB4";
        break;
    case GL_RGB5:
        name = "GL_RGB5";
        break;
    case GL_RGB565:
        name = "GL_RGB565";
        break;
    case GL_RGB8:
        name = "GL_RGB8";
        break;
    case GL_RGB8_SNORM:
        name = "GL_RGB8_SNORM";
        break;
    case GL_RGB10:
        name = "GL_RGB10";
        break;
    case GL_RGB12:
        name = "GL_RGB12";
        break;
    case GL_RGB16:
        name = "GL_RGB16";
        break;
    case GL_RGB16_SNORM:
        name = "GL_RGB16_SNORM";
        break;
    case GL_RGBA2:
        name = "GL_RGBA2";
        break;
    case GL_RGBA4:
        name = "GL_RGBA4";
        break;
    case GL_RGB5_A1:
        name = "GL_RGB5_A1";
        break;
    case GL_RGBA8:
        name = "GL_RGBA8";
        break;
    case GL_RGBA8_SNORM:
        name = "GL_RGBA8_SNORM";
        break;
    case GL_RGB10_A2:
        name = "GL_RGB10_A2";
        break;
    case GL_RGB10_A2UI:
        name = "GL_RGB10_A2UI";
        break;
    case GL_RGBA12:
        name = "GL_RGBA12";
        break;
    case GL_RGBA16:
        name = "GL_RGBA16";
        break;
    case GL_RGBA16_SNORM:
        name = "GL_RGBA16_SNORM";
        break;
    case GL_SRGB8:
        name = "GL_SRGB8";
        break;
    case GL_SRGB8_ALPHA8:
        name = "GL_SRGB8_ALPHA8";
        break;
    case GL_R16F:
        name = "GL_R16F";
        break;
    case GL_RG16F:
        name = "GL_RG16F";
        break;
    case GL_RGB16F:
        name = "GL_RGB16F";
        break;
    case GL_RGBA16F:
        name = "GL_RGBA16F";
        break;
    case GL_R32F:
        name = "GL_R32F";
        break;
    case GL_RG32F:
        name = "GL_RG32F";
        break;
    case GL_RGB32F:
        name = "GL_RGB32F";
        break;
    case GL_RGBA32F:
        name = "GL_RGBA32F";
        break;
    case GL_R11F_G11F_B10F:
        name = "GL_R11F_G11F_B10F";
        break;
    case GL_RGB9_E5:
        name = "GL_RGB9_E5";
        break;
    case GL_R8I:
        name = "GL_R8I";
        break;
    case GL_R8UI:
        name = "GL_R8UI";
        break;
    case GL_R16I:
        name = "GL_R16I";
        break;
    case GL_R16UI:
        name = "GL_R16UI";
        break;
    case GL_R32I:
        name = "GL_R32I";
        break;
    case GL_R32UI:
        name = "GL_R32UI";
        break;
    case GL_RG8I:
        name = "GL_RG8I";
        break;
    case GL_RG8UI:
        name = "GL_RG8UI";
        break;
    case GL_RG16I:
        name = "GL_RG16I";
        break;
    case GL_RG16UI:
        name = "GL_RG16UI";
        break;
    case GL_RG32I:
        name = "GL_RG32I";
        break;
    case GL_RG32UI:
        name = "GL_RG32UI";
        break;
    case GL_RGB8I:
        name = "GL_RGB8I";
        break;
    case GL_RGB8UI:
        name = "GL_RGB8UI";
        break;
    case GL_RGB16I:
        name = "GL_RGB16I";
        break;
    case GL_RGB16UI:
        name = "GL_RGB16UI";
        break;
    case GL_RGB32I:
        name = "GL_RGB32I";
        break;
    case GL_RGB32UI:
        name = "GL_RGB32UI";
        break;
    case GL_RGBA8I:
        name = "GL_RGBA8I";
        break;
    case GL_RGBA8UI:
        name = "GL_RGBA8UI";
        break;
    case GL_RGBA16I:
        name = "GL_RGBA16I";
        break;
    case GL_RGBA16UI:
        name = "GL_RGBA16UI";
        break;
    case GL_RGBA32I:
        name = "GL_RGBA32I";
        break;
    case GL_RGBA32UI:
        name = "GL_RGBA32UI";
        break;
    }
    return name;
}

#ifndef GL_PALETTE4_RGB8_OES
#define GL_PALETTE4_RGB8_OES 0x8B90
#endif 
std::string checkAgainstOtherValues(GLenum hexEnum) noexcept {
    std::string name = unknownFormatName;
    switch (hexEnum) {
    default:
        break;
    case(GL_PALETTE4_RGB8_OES):
        name = "GL_PALETTE4_RGB8_OES";
        break;
    }

    return name;
}

//More Glenum Values 

/*
// PixelInternalFormat 
#define GL_PALETTE4_RGB8_OES              0x8B90
#define GL_PALETTE4_RGBA8_OES             0x8B91
#define GL_PALETTE4_R5_G6_B5_OES          0x8B92
#define GL_PALETTE4_RGBA4_OES             0x8B93
#define GL_PALETTE4_RGB5_A1_OES           0x8B94
#define GL_PALETTE8_RGB8_OES              0x8B95
#define GL_PALETTE8_RGBA8_OES             0x8B96
#define GL_PALETTE8_R5_G6_B5_OES          0x8B97
#define GL_PALETTE8_RGBA4_OES             0x8B98
#define GL_PALETTE8_RGB5_A1_OES           0x8B99


*/