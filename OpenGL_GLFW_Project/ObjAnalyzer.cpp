//Implementation file for ObjAnalyzer class. See header for details...
//
// By Forrest Miller on September 14, 2018


#include "ObjAnalyzer.h"
#include <fstream>

//   //For reference
//typedef struct ModelStatistics {
//	long vertices;
//	long positions;
//	long texels;
//	long normals;
//	long faces;
//};

namespace AssetLoadingInternal {

	size_t ModelStatistics::computeRequiredBufferSpace() const {


	}

	void ObjAnalyzer::initialize() {
		mObjFileText = nullptr;
	}

	ObjAnalyzer::ObjAnalyzer(const char * filepath) {

	}

	ObjAnalyzer::ObjAnalyzer(ObjAnalyzer&& that) {

	}

	ObjAnalyzer& ObjAnalyzer::operator=(ObjAnalyzer&& that) {

	}

	inline std::unique_ptr<std::string> getObjFileText(const char * objFilepath) {
		std::unique_ptr<std::string> objFileText = std::make_unique<std::string>();
		std::ifstream inObjFile{ objFilepath };
		*objFileText = { std::istreambuf_iterator<char>(inObjFile), std::istreambuf_iterator<char>() };
		return (std::move(objFileText));
	}

} //namespace AssetLoadingInternal
