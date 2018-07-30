//This is a common header file used by Shader Processing objects to 
//track their error state. 
//
//
// Forrest Miller    July 30, 2018


#pragma once

#ifndef SHADER_INTERFACE_STATUS_CODE_H_
#define SHADER_INTERFACE_STATUS_CODE_H_

///For logging 
#ifndef MSGLOG
#define MSGLOG stdout
#endif

#ifndef ERRLOG
#define ERRLOG stderr
#endif 

#ifndef WRNLOG
#define WRNLOG stderr
#endif 

namespace ShaderInterface {
	namespace UNFINISHED_UNUSED {

		//enum ShaderInterfaceErrorCodes {
		enum ShaderInterfaceStatusCode {
			SI_NO_ERROR,                         //No error
			SI_BAD_FILEPATH,					 //
			SI_BAD_COPY,                         //For when an illegal copy operation is attempted/performed
			SI_GL_ERROR,						 //For an error within the graphics context 
			SI_TYPE_MISMATCH,                    //For an illegal operation on mismatched-derived types 
			SI_LOCAL_DATA_AND_GLCONTEXT_TYPE_MISMATCH,
			SI_UNCOMPILED,
			SI_DECOMISSIONED,
			SI_UNINITIALIZED,
			SI_MARKED_FOR_DESTRUCTION,
			SI_POTENTIAL_OBJECT_SLICING_DETECTED,
		};


		//Also there will be a mini stack implemented globally that can be used on objects for codes

		typedef struct SIStatusCodeStack final {
		private:
			ShaderInterfaceStatusCode * mStatusCode;

			typedef struct SIStatusCodeBucket {
				ShaderInterfaceStatusCode mSelf;
				SIStatusCodeBucket * next;
			} SIStatusCodeBucket;

			//Add Push, Pop, Print functions...

		} SICodeStack;

	} //namespace UNFINISHED_UNUSED

} //namespace ShaderInterface

#endif SHADER_INTERFACE_STATUS_CODE_H_
