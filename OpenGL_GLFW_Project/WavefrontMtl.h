//File:                           WavefrontMtl.h
//
//Description:                    Wrapper class for a '.mtl' file. More detailed description will be written 
//                                once I have completed this class and have more time...
//
//Reference:                      http://paulbourke.net/dataformats/mtl/
//
// 
//Programmer:                     Forrest Miller
//Date:                           November 14, 2018
//
//                        


//Here are some sample materials that could exist in a file:
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

#ifdef WAVEFRONT_MTL_H_
#define WAVEFRONT_MTL_H_

//#include "LoggingMessageTargets.h"
#include "AssetInterface.h"                  //Abstract base class for loadable assets
#include "AsciiAsset.h"                      //For loading the asset
#include "PhongShadingParameterSet.h"               
#include "DefaultPhongIllumination.h"

#include <array>
#include <optional>           //Used to represent optional types, requires C++17 

//                           //These illumination models were gather from: https://en.wikipedia.org/wiki/Wavefront_.obj_file#Basic_materials
enum class MTL_ILLUM_MODEL { COLOR_ON_AND_AMBIENT_OFF,                                                 //illum 0
							 COLOR_ON_AND_AMBIENT_ON,                                                  //illum 1
	                         HIGHLIGHT_ON,                                                             //illum 2
	                         REFLECTION_ON_AND_RAYTRACE_ON,                                            //illum 3
	                         GLASS_TRANSPARENCY_ON_AND_RAYTRACE_REFLECTION_ON,                         //illum 4
	                         FRESNEL_REFLECTION_ON_AND_RAYTRACE_ON,                                    //illum 5
	                         REFRACTION_TRANSPARENCY_ON_AND_FRESNEL_REFLECTION_OFF_AND_RAYTRACE_ON,    //illum 6
	                         REFRACTION_TRANSPARENCY_ON_AND_FRESNEL_REFLECTION_ON_AND_RAYTRACE_ON,     //illum 7
	                         REFLECTION_ON_AND_RAYTRACE_OFF,                                           //illum 8
	                         GLASS_TRANSPARENCY_ON_AND_RAYTRACE_REFLECTION_OFF,                        //illum 9
	                         CASTS_SHADOWS_ONTO_INVISIBLE_SURFACES,                                    //illum 10
	                         UNRECOGNIZED_ILLUM_MODEL                                                  //illum 11+

};


//Use an array as a quick lookup table for when parsing illumination models.
//This way illum 0 is at index 0, illum 1 is at index 1, and so on... 
static const std::array<MTL_ILLUM_MODEL, 12> ILLUM_MODEL = { MTL_ILLUM_MODEL::COLOR_ON_AND_AMBIENT_OFF,
															 MTL_ILLUM_MODEL::COLOR_ON_AND_AMBIENT_ON,
															 MTL_ILLUM_MODEL::HIGHLIGHT_ON,
															 MTL_ILLUM_MODEL::REFLECTION_ON_AND_RAYTRACE_ON,
															 MTL_ILLUM_MODEL::GLASS_TRANSPARENCY_ON_AND_RAYTRACE_REFLECTION_ON,
															 MTL_ILLUM_MODEL::FRESNEL_REFLECTION_ON_AND_RAYTRACE_ON,
															 MTL_ILLUM_MODEL::REFRACTION_TRANSPARENCY_ON_AND_FRESNEL_REFLECTION_OFF_AND_RAYTRACE_ON,
															 MTL_ILLUM_MODEL::REFRACTION_TRANSPARENCY_ON_AND_FRESNEL_REFLECTION_ON_AND_RAYTRACE_ON,
															 MTL_ILLUM_MODEL::REFLECTION_ON_AND_RAYTRACE_OFF,
															 MTL_ILLUM_MODEL::GLASS_TRANSPARENCY_ON_AND_RAYTRACE_REFLECTION_OFF,
															 MTL_ILLUM_MODEL::CASTS_SHADOWS_ONTO_INVISIBLE_SURFACES,
															 MTL_ILLUM_MODEL::UNRECOGNIZED_ILLUM_MODEL };
                                                             


class WavefrontMtlLoader final : public AssetInterface {
public:
	//Constructs a WavefrontMtl object. The object will be valid and will have its 
	//data set to match the default phong illumination preset. 
	//WavefrontMtlLoader();
	
	//WavefrontMtlLoader(const char * mtlFile);
	//virtual ~WavefrontMtlLoader() override;

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

	
	

	

};

#endif //WAVEFRONT_MTL_H_