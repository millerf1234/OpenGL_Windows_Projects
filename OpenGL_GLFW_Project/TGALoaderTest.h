
//THis is just some code that can be used to test the TGALoader 
//class. It's leftover from when I was developing the class.

#pragma once

#ifndef TGA_IMAGE_TESTS_H_
#define TGA_IMAGE_TESTS_H_

#include "TGAImage.h"

#include "LoggingMessageTargets.h"


void doTGAImageTests() {


    TGAImage emptyTest("");
    fprintf(MSGLOG, "\nEmpty Test Reason(s) For Failure:\n%s", emptyTest.errorReportString().data());

    TGAImage testTGA1(R"(Images\Cubemap\green\green_bk.tga)");
    fprintf(MSGLOG, "\testTGA1  Reason(s) For Failure:\n%s", testTGA1.errorReportString().data());

    TGAImage testTGA2("Images\\Cubemap\\green\\green_bk.tga");
    fprintf(MSGLOG, "\testTGA2  Reason(s) For Failure:\n%s", testTGA2.errorReportString().data());

    TGAImage testTGA3(R"(C:\Users\Forrest\source\repos\OpenGL_GLFW_Project\OpenGL_GLFW_Project\Images\Cubemap\green\green_lf.tga)");
    fprintf(MSGLOG, "\testTGA3  Reason(s) For Failure:\n%s", testTGA3.errorReportString().data());


}


#endif //TGA_LOADER_TESTS_H_
