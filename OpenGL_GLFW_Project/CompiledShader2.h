////This is a rewritten version of my first version of CompiledShader. The previous
////version put everything in place and worked fine, I was concerned however that it's
////implementation was slightly sloppy and had become bloated (like OpenGL already stores the 
////shader source string so there is no reason to store a local copy). Thus I decided to try
////to rewrite the whole class structure using my previous version as a guide while trying to keep
////everything more streamlined and light-weight.
////
////
//// The original version was written by Forrest Miller from between 7/24/2018 and 7/27/2018
//// This new version was written by Forrest Miller on 7/29/2018
//
//#pragma once
//
//#ifndef COMPILED_SHADER_2_H_
//#define COMPILED_SHADER_2_H_
//
//#include <fstream>
//
//#include "ProjectConstants.h"
//#include "ProjectParameters.h"
//
//
//namespace ShaderInterface {
//
//	//Enum for the various shader types
//	enum class ShaderType {VERTEX, GEOMETRY, TESSELATION_CONTROL, TESSELATION_EVALUATION, FRAGMENT, COMPUTE /*, UNSPECIFIED */};
//
//	
//
//	typedef struct ShaderID {
//		GLuint mID;
//		ShaderType mType;
//		ShaderID(GLuint id, ShaderType type) : mID(id), mType(type) { ; }
//	} ShaderID;
//
//
//	class CompiledShader2 {
//	public:
//		CompiledShader2() = delete;
//		CompiledShader2(const char * sourceFilepath);
//
//		virtual ~CompiledShader2();
//
//		void decommision();
//		void reinstate();
//
//	protected:
//		ShaderID mShaderID;
//		
//		//Protected functions:
//		bool compile();
//		virtual void aquireShaderID() = 0;
//
//	private:
//		const char * mFilepath;
//		std::unique_ptr<std::string> mSourceText;
//		bool mReadyToBeAttached;
//		bool mError;
//		bool mValidFilepath;
//
//	};
//
//}  //namespace ShaderInterface
//
//#endif //COMPILED_SHADER_2_H_