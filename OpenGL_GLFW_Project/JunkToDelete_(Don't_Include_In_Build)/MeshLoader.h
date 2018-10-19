

#pragma once

#ifndef MESH_LOADER_H_
#define MESH_LOADER_H_

#include "ProjectConstants.h"
#include "ProjectParameters.h"

#include "Mesh.h"
#include "ObjAnalyzer.h"
#include "ObjLoader.h"



// New idea
//  Have this class create itself (or a seperate buffer object). 
//  Have this class queue up files for loading into said buffer.
//  Have a concurency-based loadAndAllocate() method for filling the buffer. This works by:
//  Have multiple vectors that the file is initially parsed into (task 1)
//  This function can call a seperate function that handles reorganizing data from the vectors into an element array with ideal spacial locatility (step 2)
//  Once all models have been ideally organized, both an Element buffer and a Vertex buffer are filled from the loaded data.
//  Eventually multi file formats will be supported for loading. 
enum class MeshFileFormat { OBJ, UNSUPPORTED}; //COLLATA

typedef unsigned long modelID;

class MeshLoader {
public:
	MeshLoader();
	~MeshLoader();

	
	modelID add(const char * filepath); 
	modelID add(const char * filepath, int loadGroup);

	void loadAll();
	void loadGroup(int groupID);


private:


};


#endif //MESH_LOADER_H_







////Yet another attempt at coming up with an asset-file-to- 
////GPU-ready-buffer loading system. The class in this file
////is intended to be the public class that is used by the rest
////of the app to load data. This class is intended to be used 
////as a singleton. 
////
////  Started by Forrest Miller on 9/21/2018
//
//
//// Thoughts:
////      *Have application provide to this class filepaths for all the files
////          that are to be loaded. 
////			-This class will pre-parse each file to figure out its (initial) requirements 
////			   {which may change if the data is re-arranged for indexing}
////			-This class will create a tracker for the requested file and return a
////			   handle to the outside application so that the contents of data can be 
////			   requested later. 
////		* Once all of the filepaths have been provided, this class has a function called 'build buffer' that
////			   will put all the data into a single buffer.
////		* Have everything loaded into one single buffer that is mapped for reading with
////		     coherent and persistent bits set. 
////		* This single buffer will be 2 (for double) to 3 (for tripple) times the size of the actual required space 
////		     for the data. Then each frame, it will be cycled which section of the buffer is updated and which section
////			 is being read from
////      * LIMITATIONS: Unfortunatly, static data will not be supported (which would require a seperate buffer) and mapping for 
////			writing or other stuff is not supported for now (because I need to set a scope to achieve instead of implementing the
////          most flexible and robust system first try. 
////      * It will probably be possible to do syncing on the application side (if necessary) through following this implemntation.
////			
//
//#pragma once
//
//#ifndef MESH_LOADER_H_
//#define MESH_LOADER_H_
//
//#include "ProjectConstants.h"
//#include "ProjectParameters.h"
//
//#include "Mesh.h"
//#include "ObjAnalyzer.h"
//#include "ObjLoader.h"
//
////enum class StorageStyle { STATIC, };
//
//enum class MeshFileFormat { OBJ, UNSUPPORTED}; //COLLATA
//
//typedef unsigned long modelID;
//
//class MeshLoader {
//public:
//	MeshLoader();
//	~MeshLoader();
//
//	
//	modelID add(const char * filepath); 
//	modelID add(const char * filepath, int loadGroup);
//
//	void loadAll();
//	void loadGroup(int groupID);
//
//
//private:
//
//
//};
//
//
//#endif //MESH_LOADER_H_
