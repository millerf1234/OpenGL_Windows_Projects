
//File:           WavefrontObj.h
//Class:          WavefrontObj
//Base Class:     AssetInterface
//
//Dependent on:   AssetInterface.h   -- inherits from base class
//                Mesh.h             -- Used to store loaded data for processing by rest of application
//                AsciiAsset.h       -- Used to assist in parsing file
//                
//
//Description:    This class wraps a Wavefront '.obj' file. It can be used
//                to parse model information from the file and create a Mesh
//                object from it. 
//
// 
// Notes:         I am note sure yet, but I am thinking this class will have its own 
//                internal way of storing and ordering parsed data, and then it can
//                emplace and distribute the data in the form of a 'Mesh' object.
//
//                For a brief description of the '.obj' file format, see the link:
//                  https://www.fileformat.info/format/wavefrontobj/egff.htm
//				  and for a more detailed description see:
//				    http://paulbourke.net/dataformats/obj/
//					
//
//Future additions:    todo  (maybe make code less confusing)
//
//
//  Programmer:       Forrest Miller
//  Date(s):      
//       Created:     October 15, 2018
//   Version 0.1:     October 23, 2018

#pragma once

#ifndef WAVEFRONT_OBJ_H_
#define WAVEFRONT_OBJ_H_

#include "AssetInterface.h" //This class inherits from base class found in this file
#include "Mesh.h"           //Loaded data will be placed into "Mesh.h"
#include "Line.h"           //Used internally by class
#include "Face.h"           //Used internally by class
#include "AsciiAsset.h"     //Used internally by class

class WavefrontObj final : public AssetInterface {
public:
	/*                      /////////////////////
	                        //   Constructors  //
	                        /////////////////////                            */
	WavefrontObj(const char * filepath);
	WavefrontObj(std::string filepath);

	//   Eventually      (or maybe make a seperate class for tieing Wavefront file-collections together):
	//WavefrontObj(std::vector<std::string> filepaths); //Will parse extensions on filepaths 


	/*                        ///////////////////
	                          //  Destructor  ///
	                          ///////////////////                            */
	virtual ~WavefrontObj() override { ; }



	/*                    //////////////////////////
	                      //  Move Functionality  //
	                      //////////////////////////                         */        
	WavefrontObj(WavefrontObj&&);
	WavefrontObj& operator=(WavefrontObj&&);



	/*                 ////////////////////////////////
	                   //  AssetInterface Overrides  //
					   ////////////////////////////////                      */

	//Returns the size of the loaded asset data (just the mesh data)
	virtual size_t size() const override /*;*/ { return 0; }
	//Returns the amount of storage space required to store the asset
	virtual GLsizei requiredStorageSpace() const override  /*;*/ { return 0; }
	//Loads the asset into application memory. Most likely this will involve
	//parsing a file. Does nothing if the asset has already been loaded.
	//The parameters dictate whether to load/generate additional mesh data.
	virtual bool load() override  /*;*/ { return false; }


	/*                      ////////////////////////
					        //  Public Interface  //
					        ////////////////////////                         */

	//Retrieves the data loaded by this object as a mesh as a unique_ptr. Will 
	//return a default mesh if the object was unable to load.
	std::unique_ptr<Mesh> getFullMesh() /*const*/; 

	std::unique_ptr<Mesh> getObject(int ObjID) /*const*/;
	std::unique_ptr<Mesh> getObject(std::string objName) /*const*/;
	std::unique_ptr<Mesh> getGroup(int groupID) /*const*/;
	std::unique_ptr<Mesh> getGroup(std::string groupName) /*const*/;

	//Attaches the material file to this object. The material will be automatically linked
	//if name of material matches the name of a material in the '.obj' file. 
	void attachMaterial(std::string materialFilePath);

	//Attaches the material file to this object as though it's name was really  
	//'materialName.'
	void attachMaterialAs(std::string materialFilePath, std::string materialName);


	/*                   ///////////////////////////////
	                     //   Disabled Functionality  //
	                     ///////////////////////////////                     */

	//Must provide a filepath for object construction 
	WavefrontObj() = delete;
	//Copying is disabled
	WavefrontObj(const WavefrontObj &) = delete;
	WavefrontObj& operator=(const WavefrontObj&) = delete;


private:

	static constexpr const char * DEFAULT_NAME = "default";
	static constexpr const char * NO_MATERIAL = "None";

	//Each object in the '.obj' file (denoted by 'o' in filetext) will be assigned
	//a unique integer ID.
	using ObjectID = int;

	//A struct is used to track each object's name and ID. 
	struct Object {
		ObjectID id;
		std::string objectName;
	};

	//A struct is used to hold group info. Groups are deliminated in '.obj' file by the letter 'g'
	//Objects without any grouping are placed in a default group.
	struct Group {
		ObjectID objectID;
		std::string groupName;
		std::vector<std::shared_ptr<AssetLoadingInternal::AsciiAsset>> materials;
		std::vector < AssetLoadingInternal::Face > faces;
	};


	std::vector<std::unique_ptr<struct Group>> mGroups_;
	std::vector < std::unique_ptr<struct Object>> mObjects_;
	std::unique_ptr<AssetLoadingInternal::AsciiAsset> mFile_;




	//Helper member functions  (Functions appear in the order which they are to be called)
	void initialize();

	//Attempts to load the filepath as a string into this object's AsciiAsset member. 
	bool aquireFile(std::string filepath);

	void logError(const char * message) const;

	//Function to parse the '.obj' file
	//void parseFile(); 

	//


	//void parseMaterials();
	//void findDuplicateFaces();

	//void organizeData();

	//int detectComponentsPerPosition();
	//void parseVertices();

	//bool detectIfHasNormals();
	//void parseNormals();

	//bool detectIfHasTextureCoords();
	//void parseTextureCoords();

	
};







#endif //WAVEFRONT_OBJ_H_