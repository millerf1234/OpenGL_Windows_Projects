//Implementation file for the VertexArrayObject class.
// Forrest Miller   9/7/2018

#include "VertexArrayObject.h"

namespace ShaderInterface {
	//namespace {
		void VertexArrayObject::initialize() {
			
		}

		VertexArrayObject::VertexArrayObject(GLuint vertexBufferID) {
			mID = vertexBufferID;
		}

		VertexArrayObject::~VertexArrayObject() {
			fprintf(MSGLOG, "\nDeleting VAO!\n");
		}

	//} //anonymous namespace
} //namespace ShaderInterface



