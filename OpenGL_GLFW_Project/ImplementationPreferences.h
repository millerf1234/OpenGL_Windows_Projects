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
//                                                                         
// References:             https://www.khronos.org/opengl/wiki/Pixel_Transfer
//                         https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glGetInternalformat.xhtml
//                                                                       
// Programmer:            Forrest Miller                                 
//                                                                      
// Date:                  June 23, 2019
//
//


#pragma once

#ifndef IMPLEMENTATION_PREFERENCES_H_
#define IMPLEMENTATION_PREFERENCES_H_


#include "GlobalIncludes.h"

class ImplementationPreferences {
public:
    //Only Call From The Main Render Thread With A Valid Context
    //Or Be Prepared For The Exception This Will Throw
    ImplementationPreferences() {
        //glGetInternalformati64v(GL_RENDERBUFFER, )
    }


};







#endif //IMPLEMENTATION_PREFERENCES_H_





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