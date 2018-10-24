//Implementation file for the GenericVertexAttributeSet class. This class 
//provides a simple interface for providing data to serve as inputs for a
//GLProgram pipeline object. Is intended for static drawing (i.e. infrequent updates 
//to vertex data). See the header file 'GenericVertexAttributeSet.h' for more detail.
//
//  Code by Forrest Miller
//  Date:  September 24, 2018




#include "GenericVertexAttributeSet.h"

namespace ShaderInterface {

	GenericVertexAttributeSet::GenericVertexAttributeSet(int layoutLocations) {
		if (layoutLocations < 1)
			mActiveLocations = 1;
		else
			mActiveLocations = layoutLocations;

		createVertexBuffers(mActiveLocations);
		createVAO();
	}


	GenericVertexAttributeSet::~GenericVertexAttributeSet() {

	}

	void GenericVertexAttributeSet::sendDataToVertexBuffer(int binding, const std::vector<GLfloat> &data,
		int vertexSize, GLsizei vertexStride, GLvoid* offset) {
		//Check to make sure everything is okay to proceed
		if (binding > mActiveLocations) {
			fprintf(ERRLOG, "\nERROR: Attempting to bind data to vertex location %d\n"
				"which exceeds the number of available locations!\n", mActiveLocations);
			return;
		}
		if (data.size() < 1u) {
			fprintf(ERRLOG, "\nERROR! Please provide data when sending data to the vertex buffer!\n"
				"(The vector that was provided was empty!)\n");
			return;
		}

		//Populate the target buffer with the provided data
		glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffers[binding].id);
		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(GLfloat), data.data(), GL_STATIC_DRAW);


		//Configure the vertex attribute binding stuff
		glBindVertexArray(mVAO);
		glEnableVertexAttribArray(binding);

		//Bind the buffer to the binding spot
		glVertexAttribPointer(binding, vertexSize, GL_FLOAT, GL_FALSE, vertexStride, offset); //Last 2 params are stride and offset-of-first-vertex-into-buffer

	}


	//Private helper functions

	void GenericVertexAttributeSet::createVertexBuffers(int buffersToCreate) {
		GLuint * vboHandles = new GLuint[buffersToCreate];
		glGenBuffers(buffersToCreate, vboHandles);

		for (int i = 0; i < buffersToCreate; i++) {
			mVertexBuffers.emplace_back(VertexBuffer(vboHandles[i]));
		}


		fprintf(MSGLOG, "\nSuccessfully created %d Vertex Buffer Objects!\n", buffersToCreate);
		delete[] vboHandles;
	}

	void GenericVertexAttributeSet::createVAO() {
		mVAO = 0u;
		glGenVertexArrays(1, &mVAO);
		//glBindVertexArray(mVAO);
	}


} //namespace ShaderInterface