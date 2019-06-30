
#include "BasicColorAttachmentTexture.h"

#include "OpenGLEnumToString.h"  //Used to stringify the format name

//Based Off OpenGL Super Bible Example on Page 393
BasicColorAttachmentTexture::BasicColorAttachmentTexture(GLsizei levels, GLenum format, GLsizei width, GLsizei height) : levels(levels), format(format), width(width), height(height) {
    
    assert(levels > 0);
    assert(width > 0);
    assert(height > 0);
    
    formatName = convertGLEnumToString(format);

    ID = 0u;
    glGenTextures(1, &ID);

    assert(glIsTexture(ID));

    //Bind our texture 
    glBindTexture(GL_TEXTURE_2D, ID);
    glTexStorage2D(GL_TEXTURE_2D, levels, format, width, height);

    //Disable mipmaps for this texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //Unbind this texture until needed in future
    glBindTexture(GL_TEXTURE_2D, 0u);
}

BasicColorAttachmentTexture::~BasicColorAttachmentTexture() noexcept {
    if (glIsTexture(ID))
        glDeleteTextures(1, &ID);
}