//I am not sure yet if I am going to proceed with a design that uses this class, so 
//my header commenting will be sparse for now. 
//A basic description of this class though is that it takes an OBJ filepath and 
//proceeds to copy it to an application-local string, and then it analyzes the string
//to get statistics on the model. The intention then is that many instances of this
//class can get sent to a buffer-allocator class that can arrange them as needed to
//allow for models of the same format to get stored in the same buffer. 
//
// Forrest Miller  on September 14, 2018

//This class is influenced by the .obj loading code in the Vulkan Cookbook by Pawel Lapinski
//and the .obj loading class from my previous project 'Star Suzerian'. 

#pragma once

#include "ProjectConstants.h"
#include "ProjectParameters.h"

#include <future>
#include <thread>
#include <tuple>

namespace AssetLoadingInternal {

	typedef struct ModelStatistics {
		long vertices;
		long positions;
		long texels;
		long normals;
		long faces;
		
		size_t computeRequiredBufferSpace() const; //Implementation in .cpp file
	};

	class ObjAnalyzer {
	public:
		//No default constructor or copying allowed
		ObjAnalyzer() = delete;
		ObjAnalyzer(const ObjAnalyzer&) = delete;
		ObjAnalyzer& operator=(const ObjAnalyzer&) = delete;

		ObjAnalyzer(const char * filepath);
		//Eventually I will add a second constructor that looks something like:
		//ObjAnalyzer(std::tuple<const char *, bool, int>);  //Tuple is <filepath, staticMesh, drawGroup> 

		//Moving is allowed 
		ObjAnalyzer(ObjAnalyzer&&);
		ObjAnalyzer& operator=(ObjAnalyzer&&);


	private:
		std::unique_ptr<std::string> mObjFileText;
		const char * mObjFilepath;
		ModelStatistics mModelInfo;

		bool mValidFilepath;
		bool mError;


		//Private Helper functions
		void initialize();
		std::unique_ptr<std::string> getObjFileText(const char * objFilepath);
	};

} //namespace AssetLoadingInternal
