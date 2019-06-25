//
//  Class representing the state of the Default Framebuffer 
//
//  Current implementation of this class is lazy with everything being
//  public. This class's purpose is meant for purely informational 
//  purposes though so it would be foolish and probably not very 
//  useful for client code to modify this object externally. 
//
//
//
//


#pragma once

#ifndef DEFAULT_FRAMEBUFFER_STATE_H_
#define DEFAULT_FRAMEBUFFER_STATE_H_


#include <optional>
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





//Outlines the current sampling behavior enabled on this buffer/
//Member Fields: 
//         int  samples;       //The number of MSAA samples in use
//         int  sampleBuffers; //The number of MSAA buffers in use
typedef struct FramebufferSampling {
    int samples;
    int sampleBuffers;
} CurrentSamplingConfiguration;




//GLFW suggests in its documentation to retrieve the following
//properties of the default framebuffer using the function 
// 'glGetFramebufferAttachmentParameteriv()'. (See the GLFW
//link above under references)
class DefaultFramebufferState {
public:
    //So it's a long story but for the default framebuffer the available 
    //attachments are referred to under the following GLenums:
    //        GL_FRONT_LEFT, GL_FRONT_RIGHT, GL_BACK_LEFT,
    //             GL_BACK_RIGHT, GL_DEPTH or GL_STENCIL,
    //Or at least those are the only available attachments that can be 
    //targeted using 'glGetFramebufferAttachmentParameteriv()'. I am unclear
    //still about whether these must always be default 
    //Let's try to see what we can find out about all of them though. Let's
    //represent each one with the following struct
    typedef struct DefaultFBAttachment {

        GLint targetID;
        enum class Target {
            FRONT_LEFT, FRONT_RIGHT, BACK_LEFT,
            BACK_RIGHT, DEPTH, STENCIL, INVALID
        };
        Target target;
        GLint objectTypeID;
        enum class ObjectType {
            NONE, FRAMEBUFFER_DEFAULT,
            TEXTURE, RENDERBUFFER, OTHER
        };
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


    DefaultFramebufferState() : frontLeft{ GL_FRONT_LEFT },
                                frontRight{ GL_FRONT_RIGHT },
                                backLeft{ GL_BACK_LEFT },
                                backRight{ GL_BACK_RIGHT },
                                depth{ GL_DEPTH },
                                stencil{ GL_STENCIL }         {

    }
    DefFBAttachment frontLeft;
    DefFBAttachment frontRight;
    DefFBAttachment backLeft;
    DefFBAttachment backRight;
    DefFBAttachment depth;
    DefFBAttachment stencil;

    //Refresh's this object to erase all of its current data then
    //recreate itself based off the current Default Framebuffer.
    //Useful for detecting changes to the Default Framebuffer.
    //WARNING! All Existing Object State WIll Be Lost
    void refresh() noexcept {
        frontLeft = DefFBAttachment{ GL_FRONT_LEFT };
        frontRight = DefFBAttachment{ GL_FRONT_RIGHT };
        backLeft = DefFBAttachment{ GL_BACK_LEFT };
        backRight = DefFBAttachment{ GL_BACK_RIGHT };
        depth = DefFBAttachment{ GL_DEPTH };
        stencil = DefFBAttachment{ GL_STENCIL };
    }

};


#endif //DEFAULT_FRAMEBUFFER_STATE_H_