//
// This is just a separate header file containing all of the filepaths to resources that are used by this project.
// Since this project is currently intended for Windows only, these filepaths can be pretty simple.
//
//  Created by Forrest Miller on July 25, 2018 


//OOPS I have not been using this file to the extent that it was intended to be used. 
//Most of the work I have done has been hacked together though, so this file will eventually
//come much more into play



///////////////////
///  July 25, 2019   [One Year Later] Update
///////////////////
// I now have a class that wraps filesystem directories. To allow for 
//shared access to these directory objects across the codebase so as
//to avoid needlessly needing to always recreate them every time they
//are needed, each directory has been given its own function returning
//a static local instance of a FilesystemDirectory object for wrapping it. 
//////////////////



#pragma once

#ifndef RELATIVE_FILEPATHS_TO_RESOURCES_H_
#define RELATIVE_FILEPATHS_TO_RESOURCES_H_

#include "FilesystemDirectory.h"

//The following utility function is used to remove the ending 
//slash from each filepath to give just the directory name
inline std::string removeTrailingSlashFromPath(const char* str) noexcept {
    std::string dirName(str);
    auto iterToLastChar = (dirName.rbegin());
    if ( (*iterToLastChar == '\\') || (*iterToLastChar == '/') ) {
        dirName.pop_back();
    }
    return dirName;
}

///////////////////////////////
/////  Settings                      [i.e. Parameters For Initialization] 
///////////////////////////////

static const char * FILEPATH_TO_SETTINGS = R"(Settings\)";  //Raw String

//Returns a FilesystemDirectory object for the Settings directory
inline FilesystemDirectory* settingsDirectory() { //noexcept?
    static std::string dirPath = removeTrailingSlashFromPath(FILEPATH_TO_SETTINGS);
    //Static instance of FilesystemDirectory object created first time 
    //this function is called. It will then exist for remainder of 
    //program lifetime, allowing for fast access to it for all subsequent
    //function calls
    static FilesystemDirectory settingsDirectory(dirPath);
    return &settingsDirectory;
}



///////////////////////////////
/////  MODELS 
///////////////////////////////

static const char * FILEPATH_TO_MODELS = R"(obj\)";  //Raw String

//Returns a FilesystemDirectory object for the Models directory
inline FilesystemDirectory* modelsDirectory() { //noexcept?
    static std::string dirPath = removeTrailingSlashFromPath(FILEPATH_TO_MODELS);
    //Static instance of FilesystemDirectory object created first time 
    //this function is called. It will then exist for remainder of 
    //program lifetime, allowing for fast access to it for all subsequent
    //function calls
    static FilesystemDirectory modelsDirectory(dirPath);
    return &modelsDirectory;
}



///////////////////////////////
/////  FONTS
///////////////////////////////

static const char * FILEPATH_TO_FONTS = R"(Fonts\)";  //Raw String

//Returns a FilesystemDirectory object for the Fonts directory
inline FilesystemDirectory* fontsDirectory() {
    static std::string dirPath = removeTrailingSlashFromPath(FILEPATH_TO_FONTS);
    //Static instance of FilesystemDirectory object created first time 
    //this function is called. It will then exist for remainder of 
    //program lifetime, allowing for fast access to it for all subsequent
    //function calls
    static FilesystemDirectory fontsDirectory(dirPath);
    return &fontsDirectory;
}



///////////////////////////////
/////  Shaders
///////////////////////////////

static const char * FILEPATH_TO_SHADERS = R"(Shaders\)";  //Raw String

//Returns a FilesystemDirectory object for the Shaders directory
inline FilesystemDirectory* shadersDirectory() {
    static std::string dirPath = removeTrailingSlashFromPath(FILEPATH_TO_SHADERS);
    //Static instance of FilesystemDirectory object created first time 
    //this function is called. It will then exist for remainder of 
    //program lifetime, allowing for fast access to it for all subsequent
    //function calls
    static FilesystemDirectory shadersDirectory(dirPath);
    return &shadersDirectory;
}



/////////////////////////////////
///////  Sample Images
/////////////////////////////////
//
//static constexpr const char* FILEPATH_TO_SAMPLE_IMAGES = R"(SampleImages\)"; //Raw String
//
////Returns a FilesystemDirectory object for the SampleImages directory
//inline FilesystemDirectory* sampleImagesDirectory() {
//    static std::string dirPath = removeTrailingSlashFromPath(FILEPATH_TO_SAMPLE_IMAGES);
//    //Static instance of FilesystemDirectory object created first time 
//    //this function is called. It will then exist for remainder of 
//    //program lifetime, allowing for fast access to it for all subsequent
//    //function calls
//    static FilesystemDirectory sampleImagesDirectory(dirPath);
//    return &sampleImagesDirectory;
//}



///////////////////////////////
/////  Images
///////////////////////////////

static constexpr const char* FILEPATH_TO_IMAGES = R"(Images\)"; //Raw String

//Returns a FilesystemDirectory object for the Images directory
inline FilesystemDirectory* imagesDirectory() {
    static std::string dirPath = removeTrailingSlashFromPath(FILEPATH_TO_IMAGES);
    //Static instance of FilesystemDirectory object created first time 
    //this function is called. It will then exist for remainder of 
    //program lifetime, allowing for fast access to it for all subsequent
    //function calls
    static FilesystemDirectory imagesDirectory(dirPath);
    return &imagesDirectory;
}



///////////////////////////////
/////  Screenshots
///////////////////////////////

static constexpr const char* FILEPATH_TO_SCREENSHOTS= R"(Screenshots\)";  //Raw String

//Returns a FilesystemDirectory object for the Screenshots directory
inline FilesystemDirectory* screenshotsDirectory() {
    static std::string dirPath = removeTrailingSlashFromPath(FILEPATH_TO_SCREENSHOTS);
    //Static instance of FilesystemDirectory object created first time 
    //this function is called. It will then exist for remainder of 
    //program lifetime, allowing for fast access to it for all subsequent
    //function calls
    static FilesystemDirectory screenshotsDirectory(dirPath);
    return &screenshotsDirectory;
}



///////////////////////////////
/////   DEBUG OUTPUT 
///////////////////////////////

static constexpr const char* FILEPATH_TO_DEBUG_OUTPUT = R"(DebugOutput\)";

//Returns a FilesystemDirectory object for the DebugOutputReports directory
inline FilesystemDirectory* debugOutputDirectory() {
    static std::string dirPath = removeTrailingSlashFromPath(FILEPATH_TO_DEBUG_OUTPUT);
    //Static instance of FilesystemDirectory object created first time 
    //this function is called. It will then exist for remainder of 
    //program lifetime, allowing for fast access to it for all subsequent
    //function calls
    static FilesystemDirectory debugOutputDirectory(dirPath);
    return &debugOutputDirectory;
}



#endif //RELATIVE_FILEPATHS_TO_RESOURCES_H_
