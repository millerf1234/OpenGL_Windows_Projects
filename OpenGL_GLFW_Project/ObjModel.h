//Obj Model 
//Intended to be used to load an obj file into a Mesh object. Should be
//paired up with a Mtl file Loading class

#pragma once

#ifndef OBJ_MODEL_H_
#define OBJ_MODEL_H_

#include "FilepathWrapper.h"
#include "AsciiAsset.h"
#include "Mesh.h"


class ObjModel {
public:
	ObjModel() = delete;
	ObjModel(const char * fp);
	ObjModel(std::string fp);
	ObjModel(Filepath fp);
	~ObjModel();

	//Copying not allowed
	ObjModel(const ObjModel&) = delete;
	ObjModel& operator=(const ObjModel&) = delete;

	//Moving is allowed
	ObjModel(ObjModel&&);
	ObjModel& operator=(ObjModel&&);

private:
	std::unique_ptr<AssetLoadingInternal::AsciiAsset> mOBJFile;

};

#endif OBJ_MODEL_H_

