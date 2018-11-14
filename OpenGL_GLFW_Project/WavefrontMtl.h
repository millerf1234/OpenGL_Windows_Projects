//File:                           WavefrontMtl.h
//
//Description:                    Wrapper class for a '.mtl' file. More detailed description will be written 
//                                once I have completed this class and have more time...
//
//Reference:                      (See the pdf under 'notes' for this project for the one on MTL files)      
// 
//Programmer:                     Forrest Miller
//Date:                           November 14, 2018
//
//Notes:               -Based off an entire minute of in-depth internet research, it appears as though the
//                        default value various Phong implementations assign to Shiny is 30. to 40. 
//                        Shiny Represents the size of the specular reflection, with smaller numbers 
//                        representing a larger specular angle (diameter?)
//                        


//Here are some sample material files:
/*
newmtl flatwhite
Ka  0.5000  0.5000  0.5000
Kd  1.0000  1.0000  1.0000
illum 1

newmtl shinyred
Ka  0.1985  0.0000  0.0000
Kd  0.5921  0.0167  0.0000
Ks  0.5973  0.2083  0.2083
illum 2
Ns 100.2235

newmtl clearblue
Ka  0.0394  0.0394  0.3300
Kd  0.1420  0.1420  0.9500
illum 1
Tr 0.4300
*/


#pragma once

#ifndef WAVEFRONT_MTL_H_
#define WAVEFRONT_MTL_H_

//#include "LoggingMessageTargets.h"
#include "AssetInterface.h"   //Abstract base class for loadable assets
#include "AsciiAsset.h"       //For loading the asset

#include <array>
#include <optional>           //Used to represent optional types, requires C++17 

enum class MTL_ILLUM_MODEL { };

class WavefrontMtl final : public AssetInterface {
public:
	//Constructs a default material object. This object will get it's characteristics set to  
	//default material values, which will be very basic but will be complete enough
	//to allow objects with this material to still be rendered well enough. The default material
	//has no specular component, only ambient and diffuse. 
	WavefrontMtl();
	//Constructs a basic material with a color scheme matching the specified colors (or something close enough).
	//Will have ambient, diffuse and specular components. Red, Green and Blue values will be clamped to be in the 
	//range [0.0, 1.0].
	WavefrontMtl(float red, float green, float blue, float shiny = 40.0f);
	//Constructs a basic material with only an ambient component matching this constructors parameters. This means
	//diffuse and specular will both be set to all zeros. 
	WavefrontMtl(std::array<float, 3> kAmbient);
	WavefrontMtl(std::array<float, 3> kAmbient, std::array<float, 3> kDiffuse);
	WavefrontMtl(std::array<float, 3> kAmbient, std::array<float, 3> kDiffuse, std::array<float, 3> kSpecular);
	WavefrontMtl(const char * mtlFile);
	virtual ~WavefrontMtl() override;

	//Returns the name of the material. This name is either:
	//	(i) a statically assigned name given by the application for self-generated materials
	//    or
	//  (ii) the name found under the first occurance of a line matching the syntax 'newmtl [name]'.
	//
	std::string getMaterialName() const { return mMatName_; }

private:
	//Inheriter from AssetInterface:
	//    bool mLoaded_;
	//    bool mValid_;

	std::string mMatName_;
	std::unique_ptr<AssetLoadingInternal::AsciiAsset> mMtlFile_;

	

	//std::unique_ptr<PhongIllum> 

};

#endif //WAVEFRONT_MTL_H_