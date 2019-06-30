


#pragma once

#ifndef FRAMEBUFFER_BASE_H_
#define FRAMEBUFFER_BASE_H_

#include "GlobalIncludes.h"

class FramebufferBase {
public:
    FramebufferBase() = default;

    virtual void enableStencil() = 0;
    virtual void enableDepth() = 0;

protected:
    GLuint mID_;
    /* //Not needed since can just check if mID_ is 0 or not...
    enum class FBTYPE {OFFSCREEN, SCREEN};
    FMTYPE mType_;
    */
};

#endif //FRAMEBUFFER_BASE_H_

