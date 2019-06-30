//
//  This is really just a placeholder class for until I can get a more realized texture
//  object implementation into place. This type will be pretty hardcoded to serve as a
//   suitable texture for acting as a color attachment for a FrameBuffer.
//
//
//


#pragma once

#ifndef BASIC_COLOR_ATTACHMENT_TEXTURE_H_
#define BASIC_COLOR_ATTACHMENT_TEXTURE_H_

#include <string>

#include "GlobalIncludes.h"


class BasicColorAttachmentTexture {
public:
    //Everything will be public information
    BasicColorAttachmentTexture(GLsizei levels, GLenum format, GLsizei width, GLsizei height);
    ~BasicColorAttachmentTexture() noexcept;
    GLuint ID;
    GLsizei levels;
    GLenum format;
    std::string formatName;
    GLint width, height;

    bool isValid() const noexcept {
        return glIsTexture(ID);
    }

    BasicColorAttachmentTexture(BasicColorAttachmentTexture&& that) noexcept {
        ID = that.ID;
        levels = that.levels;
        format = that.format;
        formatName = std::move(that.formatName);
        width = that.width;
        height = that.height;
        that.ID = 0u; //Prevents destructor from deleting this Texture
    }

    BasicColorAttachmentTexture& operator=(BasicColorAttachmentTexture&& that) noexcept {
        if (this != &that) {
            ID = that.ID;
            levels = that.levels;
            format = that.format;
            formatName = std::move(that.formatName);
            width = that.width;
            height = that.height;
            that.ID = 0u; //Prevents destructor from deleting this Texture
        }
        return *this;
    }

};


#endif //BASIC_COLOR_ATTACHMENT_H_
