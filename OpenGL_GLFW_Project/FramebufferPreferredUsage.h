
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
#include <set>
#include <string>
#include "Timepoint.h"
#include "GlobalIncludes.h"



#define INCLUDE_NAME

//Here are the 2 most important settings that are entirely the purpose behind this class.
//This is because they directly matter towards the performance of reading data from the
//default framebuffer.
typedef struct ColorReadPreference {
    GLenum preferredFormat;  //e.g. 
    GLenum preferredType;    //e.g.
#ifdef INCLUDE_NAME
    std::string preferredFormatName;
    std::string preferredTypeName;
#endif //INCLUDE_NAMES
} PreferredPixelReadParam;





//Outlines the current sampling behavior enabled on this buffer
typedef struct FramebufferSampling {
    int samples;
    int sampleBuffers;
} CurrentSamplingConfiguration;




//GLFW suggests in its documentation to retrieve the following
//properties of the default framebuffer using the function 
// 'glGetFramebufferAttachmentParameteriv()'. (See the GLFW
//link above under references)
typedef struct DefaultFramebufferState {

    //So it's a long story but for the default framebuffer the available 
    //attachments are referred to under the following GLenums:
    //        GL_FRONT_LEFT, GL_FRONT_RIGHT, GL_BACK_LEFT,
    //             GL_BACK_RIGHT, GL_DEPTH or GL_STENCIL,
    //Arrg it is confusing. Basically all of GLFW's color is being
    //kept in the BACK_LEFT buffer, so we are very curious about that one.
    //Let's try to see what we can find out about all of them though. Let's
    //represent each one with the following struct
    typedef struct DefaultFBAttachment {
        
        GLint targetID;
        enum class Target { FRONT_LEFT, FRONT_RIGHT, BACK_LEFT,
            BACK_RIGHT, DEPTH, STENCIL , INVALID};
        Target target;
        GLint objectTypeID;
        enum class ObjectType { NONE, FRAMEBUFFER_DEFAULT,
            TEXTURE, RENDERBUFFER , OTHER};
        ObjectType objType;
        //If ObjectType is NONE, then none of the these remaining values will
        //be set.
        //Not all of these will be active at the same time
        GLint redSize, greenSize, blueSize, alphaSize, depthSize, stencilSize;
        GLint componentTypeID;
        enum class ComponentType { FLOAT, INT, UINT, SNORM, UNORM }; //Types 2-5 are all integer types which only are used with color attachments
        ComponentType componentType;
        GLint colorEncodingID;
        enum class ColorEncoding { LINEAR, SRGB };
        ColorEncoding colorEncoding;
        DefaultFBAttachment(GLenum targetAttachment) : targetID(targetAttachment),
                                             target(Target::INVALID),
                                             objectTypeID(0),
                                             objType(ObjectType::NONE),
                                             redSize(0), 
                                             greenSize(0),
                                             blueSize(0),
                                             alphaSize(0),
                                             depthSize(0),
                                             stencilSize(0),
                                             componentTypeID(0),
                                             componentType(ComponentType::FLOAT),
                                             colorEncodingID(0),
                                             colorEncoding(ColorEncoding::LINEAR) {
            switch (targetAttachment) {
            default: 
                break;
            case (GL_FRONT_LEFT):
                target = Target::FRONT_LEFT;
                break;
            case (GL_FRONT_RIGHT):
                target = Target::FRONT_RIGHT;
                break;
            case (GL_BACK_LEFT):
                target = Target::BACK_LEFT;
                break;
            case (GL_BACK_RIGHT):
                target = Target::BACK_RIGHT;
                break;
            case (GL_DEPTH):
                target = Target::DEPTH;
                break;
            case (GL_STENCIL):
                target = Target::STENCIL;
                break;
            }
            
            //Find out our object type
            glGetNamedFramebufferAttachmentParameteriv(0u, targetAttachment,
                GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &objectTypeID);
            if (objectTypeID == GL_NONE)
                return;

            switch (objectTypeID) {
            default:
                objType = ObjectType::OTHER;
                break;
            case GL_TEXTURE:
                objType = ObjectType::TEXTURE;
                break;
            case GL_RENDERBUFFER:
                objType = ObjectType::RENDERBUFFER;
                break;
            case GL_FRAMEBUFFER_DEFAULT:
                objType = ObjectType::FRAMEBUFFER_DEFAULT;
                break;
            }


            //Since we know we are a valid attachment object type, might as well fill in
            //the rest of our members
            glGetNamedFramebufferAttachmentParameteriv(0u, targetID,
                GL_FRAMEBUFFER_ATTACHMENT_RED_SIZE, &redSize);
            glGetNamedFramebufferAttachmentParameteriv(0u, targetID,
                GL_FRAMEBUFFER_ATTACHMENT_GREEN_SIZE, &greenSize);
            glGetNamedFramebufferAttachmentParameteriv(0u, targetID,
                GL_FRAMEBUFFER_ATTACHMENT_BLUE_SIZE, &blueSize);
            glGetNamedFramebufferAttachmentParameteriv(0u, targetID,
                GL_FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE, &alphaSize);
            glGetNamedFramebufferAttachmentParameteriv(0u, targetID,
                GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE, &depthSize);
            glGetNamedFramebufferAttachmentParameteriv(0u, targetID,
                GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE, &stencilSize);


            glGetNamedFramebufferAttachmentParameteriv(0u, targetID,
                GL_FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE, &componentTypeID);
            switch (componentTypeID) {
            default:
                fprintf(MSGLOG, "\nUnrecognized component type: %#X\n", componentTypeID);
                break;
            case (GL_FLOAT):
                componentType = ComponentType::FLOAT;
                break;
            case (GL_INT):
                componentType = ComponentType::INT;
                break;
            case (GL_UNSIGNED_INT):
                componentType = ComponentType::UINT;
                break;
            case (GL_SIGNED_NORMALIZED):
                componentType = ComponentType::SNORM;
                break;
            case (GL_UNSIGNED_NORMALIZED):
                componentType = ComponentType::UNORM;
                break;
            }

            glGetNamedFramebufferAttachmentParameteriv(0u, targetID,
                GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING, &colorEncodingID);
            if (colorEncodingID == GL_SRGB)
                colorEncoding = ColorEncoding::SRGB;


        }


    } DefFBAttachment;


    DefaultFramebufferState() : frontLeft { GL_FRONT_LEFT }, 
                                frontRight { GL_FRONT_RIGHT }, 
                                backLeft { GL_BACK_LEFT },
                                backRight { GL_BACK_RIGHT },
                                depth { GL_DEPTH },
                                stencil { GL_STENCIL }         {                        

    }
    DefFBAttachment frontLeft; 
    DefFBAttachment frontRight;
    DefFBAttachment backLeft;
    DefFBAttachment backRight;
    DefFBAttachment depth;
    DefFBAttachment stencil;
   
    
} DefaultFBState;






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

    //This will only be available if this object is a Default Framebuffer
    std::optional<DefaultFBState> mState_;

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


//
// File:                  ImplementationPreferences.h
//
// Class:                 ImplementationPreferences
//
// Description:           For performing data transfers to and from 
//                        GPU to CPU and vice-versa, each implementation/
//                        platform will have its own unique preferences
//                        for how to best arrange and represent data. 
//                        Since this information is invariant once it is
//                        queried, this type is implemented with usage as
//                        a global reference object in mind. This object 
//                        simply needs to be constructed on the thread with
//                        the primary OpenGL context after it and all the 
//                        OpenGL functions have had a chance to load.
//                                                                        
                                                  





//#endif //IMPLEMENTATION_PREFERENCES_H_





#if 0



//  Based off the following
//      
//       https://www.khronos.org/opengl/wiki/GLAPI/glGetInternalformat
//      https://gist.github.com/rdb/83b1d952e3808f100465
//      https://www.khronos.org/opengl/wiki/Common_Mistakes#Slow_pixel_transfer_performance
//     --  OpenGL Programming Guide 9e Page 280

#pragma once

#ifndef REPORT_DETAILS_ON_GL_IMPLEMENTATION_H_
#define REPORT_DETAILS_ON_GL_IMPLEMENTATION_H_


#include <string>
#include <vector>
#include <sstream>
#include "GraphicsLanguage.h"


//Function Prototypes
std::string getType(GLenum type) noexcept;
std::string internalFormatEnumToString(GLenum format) noexcept;


//
//enum BaseInternalFormats {
//    red = GL_RED,
//    rg = GL_RG,
//    rgb = GL_RGB,
//    rgba = GL_RGBA,
//};
//

namespace {
    const char* indent = "     ";
    const char* sectionDelimiter = "      +-----------------+---------------------------+--------------------+\n";

    const char* unknownFormatName = "UNKNOWN";
}


std::string getGLImplentationReportHeader() noexcept {
    std::ostringstream glReportHeader;

    glReportHeader << "\n"
        << indent << indent << indent << "           ________________________       \n"
        << indent << indent << indent << "          /                        \\      \n"
        << indent << indent << indent << "     --- ( GL Implementation Report ) --- \n"
        << indent << indent << indent << "          \\________________________/      \n";


    //General Information
    const GLubyte* name = glGetString(GL_VENDOR);
    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* version = glGetString(GL_VERSION);

    glReportHeader << ""
        << indent << indent << indent << indent << "Vendor:    " << indent << "   " << name << "\n"
        << indent << indent << indent << indent << "Renderer:  " << indent << "   " << renderer << "\n"
        << indent << indent << indent << indent << "Version:   " << indent << "   " << version << "\n";

    return glReportHeader.str();
}


//I don't think I understand the concepts fully behind formats, types and texture characteristics
std::string getImplementationsPreferencesForFormat(GLenum format) noexcept {
    char buffer[28] = { '\0' };
    GLint preferredInternalFormat = 0;
    GLint preferredInternalType = 0;
    std::ostringstream chart;
    std::string formatName = internalFormatEnumToString(format);
    if (formatName == unknownFormatName) {
        chart << "INVALID ENUM FORMAT: (GLenum) 0x" << std::hex << format << "\n";
        return chart.str();
    }

    chart << "         INTERNAL FORMAT:  " << formatName << "      Target: Texture2D\n";
    chart << "      +=================+===========================+====================+\n";
    chart << "      |    OPERATION    | Preferred Internal Format |   Preferred Type   |\n";
    chart << "      +=================+===========================+====================+\n";

    chart << "      |     UPLOAD      |";


    //Get the 'UPLOAD' preferred format and type
    glGetInternalformativ(GL_TEXTURE_2D, format, GL_TEXTURE_IMAGE_FORMAT, 1, &preferredInternalFormat);
    glGetInternalformativ(GL_TEXTURE_2D, format, GL_TEXTURE_IMAGE_TYPE, 1, &preferredInternalType);


    //I've been getting a lot of 'unknown' or 'unrecognized type' results from these functions so I have
    //decided to call both enum-to-string functions for each type just to see if I can increase my chances 
    //of getting a recognized enum

    chart << "\n\ntypeFunc gives:  " << getType(preferredInternalFormat);
    chart << "\nformatFunc gives:  " << internalFormatEnumToString(preferredInternalFormat); //Call both enum-to-string for the format
    chart << "\n\ntypeFunc gives:  " << getType(preferredInternalType);
    chart << "\nformatFunc gives:  " << internalFormatEnumToString(preferredInternalType); //call both enum-to-string for the type
    chart << "\n\n";



    chart << sectionDelimiter;
    chart << "      |      READ       |";


    glGetInternalformativ(GL_TEXTURE_2D, format, GL_READ_PIXELS_FORMAT, 1, &preferredInternalFormat);
    glGetInternalformativ(GL_TEXTURE_2D, format, GL_READ_PIXELS_TYPE, 1, &preferredInternalType);

    chart << "\n\ntypeFunc gives:  " << getType(preferredInternalFormat);
    chart << "\nformatFunc gives:  " << internalFormatEnumToString(preferredInternalFormat);
    chart << "\n\ntypeFunc gives:  " << getType(preferredInternalType);
    chart << "\nformatFunc gives:  " << internalFormatEnumToString(preferredInternalType);
    chart << "\n\n";



    chart << sectionDelimiter;
    chart << "      |   Get Texture   |";

    glGetInternalformativ(GL_TEXTURE_2D, format, GL_GET_TEXTURE_IMAGE_FORMAT, 1, &preferredInternalFormat);
    glGetInternalformativ(GL_TEXTURE_2D, format, GL_GET_TEXTURE_IMAGE_TYPE, 1, &preferredInternalType);

    chart << "\n\ntypeFunc gives:  " << getType(preferredInternalFormat);
    chart << "\nformatFunc gives:  " << internalFormatEnumToString(preferredInternalFormat);
    chart << "\n\ntypeFunc gives:  " << getType(preferredInternalType);
    chart << "\nformatFunc gives:  " << internalFormatEnumToString(preferredInternalType);
    chart << "\n\n";

    chart << sectionDelimiter;

    chart << "\n\n";
    return chart.str();
}



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




#endif //REPORT_DETAILS_ON_GL_IMPLEMENTATION_H_






#endif //0