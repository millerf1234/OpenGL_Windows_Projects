



#include "FramebufferPreferredUsage.h"

namespace {
    const std::string creationMsgPt1 = "Constructing Preferred Usage Object For Framebuffer ";
}


FramebufferPreferredUsage::FramebufferPreferredUsage(GLuint framebufferName) :
                                        mIsDefaultFramebuffer_(0u == framebufferName),
                                        mName_(framebufferName),
                                        mTCreation_(creationMsgPt1 + std::to_string(framebufferName)) {
    assert(glfwGetCurrentContext());

}

FramebufferPreferredUsage::~FramebufferPreferredUsage() noexcept {

}
