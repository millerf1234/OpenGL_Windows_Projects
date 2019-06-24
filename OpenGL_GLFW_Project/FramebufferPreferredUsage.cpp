


#include "FramebufferPreferredUsage.h"




namespace {
    const std::string creationMsgPt1 = "Constructing Preferred Usage Object For Framebuffer ";
}


//Prototypes for Implementation-only functions 
std::string internalFormatEnumToString(GLenum format) noexcept;
std::string getType(GLenum type) noexcept;



FramebufferPreferredUsage::FramebufferPreferredUsage(GLuint framebufferName) :
                                        mIsDefaultFramebuffer_(0u == framebufferName),
                                        mName_(framebufferName),
                                        mTCreation_(creationMsgPt1 + std::to_string(framebufferName)),
                                        mTReady_(nullptr) {
    constexpr GLint NOT_SET = -3; //Going to play it safe and go with -3 in-case -1 means something
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
    glGetNamedFramebufferParameteriv(mName_, GL_IMPLEMENTATION_COLOR_READ_TYPE, &queryResult);
    assert(queryResult != NOT_SET);
    mSampling_.sampleBuffers = queryResult;
}

FramebufferPreferredUsage::~FramebufferPreferredUsage() noexcept {

}


PreferredPixelReadParam FramebufferPreferredUsage::getPreferredReadPixelsParameters() const noexcept {
    return mFramebufferPreferences_;
}


GLenum FramebufferPreferredUsage::preferredColorReadFormat() const noexcept {
    return mFramebufferPreferences_.preferredFormat;
}

GLenum FramebufferPreferredUsage::preferredColorReadType() const noexcept {
    return mFramebufferPreferences_.preferredType;
}








static constexpr const char* unknownFormatName = "UNKNOWN FORMAT";

std::string getType(GLenum type) noexcept {
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
    }
    return typeName;
}





std::string internalFormatEnumToString(GLenum format) noexcept {
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