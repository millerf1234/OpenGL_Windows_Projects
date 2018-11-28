//Implementation file for ObjAnalyzer class. See header for details...
//
// By Forrest Miller on September 14, 2018


#include "ObjAnalyzer.h"
#include <fstream>


namespace AssetLoadingInternal {

	///////////////////////////////////////////////////////
	///////////     SectionStatistics    /////////////
	///////////////////////////////////////////////////////
	ModelStatistics::SectionStatistics::SectionStatistics() {
		vertices = 0l;
		positions = 0l;
		texels = 0l;
		normals = 0l;
		faces = 0l;
		mNextSectionStatistics = nullptr;
	}

	///////////////////////////////////////////////////////
	///////////         ModelStatistics      //////////////
	///////////////////////////////////////////////////////
	size_t ModelStatistics::computeRequiredBufferSpace() const {
		if (mSectionStatisticsListHead == nullptr) 
			return 0ul;
		return 0ul;
	}


	////////////////////////////////////////////////////////
	//////////           ObjAnalyzer          //////////////
	////////////////////////////////////////////////////////
	void ObjAnalyzer::initialize() {
		mObjFileText = nullptr;
	}

	ObjAnalyzer::ObjAnalyzer(const char * filepath) {

	}

	ObjAnalyzer::ObjAnalyzer(ObjAnalyzer&& that) {

	}

	ObjAnalyzer& ObjAnalyzer::operator=(ObjAnalyzer&& that) {
		return *this;
	}

	inline std::unique_ptr<std::string> ObjAnalyzer::getObjFileText(const char * objFilepath) {
		std::unique_ptr<std::string> objFileText = std::make_unique<std::string>();
		std::ifstream inObjFile{ objFilepath };
		*objFileText = { std::istreambuf_iterator<char>(inObjFile), std::istreambuf_iterator<char>() };
		return (std::move(objFileText));
	}

} //namespace AssetLoadingInternal
