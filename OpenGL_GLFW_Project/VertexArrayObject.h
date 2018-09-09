//Wrapper for a OpenGL context-generated Vertex Array Object used to 
//represent vertex attributes/layout format. The proper method for
//using this object is to have a single Vertex Array Object bound in a 
//1-to-1 relationship for each Vertex Array Object in use. 

//Created by Forrest Miller on  9/7/2018

#pragma once

#ifndef VERTEX_ARRAY_OBJECT_H_
#define VERTEX_ARRAY_OBJECT_H_

#include "ProjectSetup.h"
#include "ProjectConstants.h"


namespace ShaderInterface {
	namespace {


		class VertexArrayObject {
		public:
			////////////////
			///  Deleted Functionality
			///////////////
			VertexArrayObject() = delete;
			VertexArrayObject(const VertexArrayObject &) = delete;
			VertexArrayObject& operator=(const VertexArrayObject &) = delete;

			///////////////
			///  Constructor
			//////////////
			VertexArrayObject(GLuint VertexBufferID);

			///////////////
			///  Destructor
			//////////////
			~VertexArrayObject();



		private:
			//Member Variables
			GLuint mID;
			GLuint mPairedVertexBufferID;

			bool mError;
			bool mHasAFormatSet;
			
			
			//Helper functions
			void initialize();
		};



	} //Anonymous namespace 
}//namespace ShaderInterface

#endif //VERTEX_ARRAY_OBJECT_H_

