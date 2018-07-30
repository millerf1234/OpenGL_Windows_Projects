//Redone version of CompiledShader    (i.e. Version_old (written over 7/24/2018-7/27/2018) )
//
// See Header file for more details 
//
//Code by Forrest Miller           Version2 (7/29/2018 - 7/30/2018)



#include "CompiledShader.h"

namespace ShaderInterface {

	/*CompiledShader::CompiledShader() {
		initialize();
	}*/

	CompiledShader::CompiledShader(const char * sourceFilepath) {
		initialize();
		mFilepath = sourceFilepath;
		if (sourceFilepath == nullptr) {
			mError = true;
			mValidFilepath = false;
			return;
		}
		if (!loadSourceFile()) {
			fprintf(ERRLOG, "\nError reading ShaderSource \"%s\"\nNo file found at this location!\n", mFilepath);
			mError = true;
			mValidFilepath = false;
			return;
		}
	}



	bool CompiledShader::compile() {

		return true;
	}


	bool CompiledShader::loadSourceFile() {


		return true;
	}

	void CompiledShader::initialize() {
		mShaderID = ShaderID(0u, ShaderType::UNSPECIFIED);
		mIsDecomissioned = false;
		mFilepath = "\0";
		mSourceText = nullptr;
		mError = false;
		mReadyToBeAttached = false;
		mHasFilepath = false;
		mValidFilepath = false;
	}

	bool CompiledShader::checkIfValidFilepath(std::ifstream& inFileStream) {


		return true;
	}


	bool CompiledShader::checkForCompilationErrors() {


		return true;
	}

} //namespace ShaderInterface