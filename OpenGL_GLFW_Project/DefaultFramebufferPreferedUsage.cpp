//
//
//
//
//
//
//

#include "FramebufferPreferedUsage.h"



FramebufferPreferedUsage::FramebufferPreferedUsage(GLuint framebufferName) :
                                         mName_(framebufferName), 
                                         isDefaultFramebuffer(0u == framebufferName) { 
    
}

FramebufferPreferedUsage::~FramebufferPreferedUsage() noexcept {

}