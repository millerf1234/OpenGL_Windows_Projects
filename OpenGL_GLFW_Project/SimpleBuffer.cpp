#include "SimpleBuffer.h"

namespace ShaderInterface {

	SimpleBuffer::SimpleBuffer() {
		mBufferID = 0u;
		mDataWasLoadedToGpu = false;

	}


	SimpleBuffer::~SimpleBuffer() {

	}

		//SimpleBuffer::SimpleBuffer(const SimpleBuffer&) { 
	    //		//todo...
		//}
		//SimpleBuffer::SimpleBuffer(SimpleBuffer&&) {
		//      //todo...
		//}

		//SimpleBuffer::SimpleBuffer(SimpleBuffer&&);

	void SimpleBuffer::createGLBuffer() {
		if (ID() != 0u) {
			fprintf(WRNLOG, "\nWARNING! Unable to create a GLBuffer for this SimpleBuffer obj\n"
				"because a GLBuffer has already been created with ID %u", ID());
			return;
		}

		glCreateBuffers(1, &mBufferID);
		if (ID() != 0u)
			fprintf(MSGLOG, "SimpleBuffer was created in OpenGL with ID: %u\n", ID());

	}

	void SimpleBuffer::attachVertices(const std::vector<float> & vertData) {
		if (hasVertData()) {
			fprintf(WRNLOG, "\nWARNING! This SimpleBuffer already has data attached, so unable to\n"
				"attach new data!\n");
			return;
		}
		else if (vertData.size() == 0u) {
			fprintf(WRNLOG, "\nWARNING! attachVertices called on a SimpleBuffer object but the provided\n"
				"data was empty!\n");
			return;
		}
		else {
			std::vector<GLfloat>(vertData).swap(mVertData); //This is a fancy-schmancy way of copying vertData to mVertData
		}
	}


	void SimpleBuffer::loadBufferToGPU() {
		if (!hasValidBufferID()) {
			fprintf(WRNLOG, "\nWARNING! BUFFER DOES NOT HAVE A VALID OpenGL ID!\n"
				"\tThis Buffer's ID number is %u", this->ID());
			mDataWasLoadedToGpu = false;
			return;
		}
		else if (!hasVertData()) {
			fprintf(WRNLOG, "\nWARNING! SIMPLE BUFFER OBJECT DOES NOT CONTAIN ANY DATA TO BUFFER!\n");
			return;
		}
		else { //Load the data to the GPU
			//NOTE: The difference between named and unnamed is unnamed uses the current buffer bound to the context
			//glBufferStorage
			//see: https://stackoverflow.com/questions/27810542/what-is-the-difference-between-glbufferstorage-and-glbufferdata
		}
	}


	void SimpleBuffer::bindToContext() const {
		if (!hasValidBufferID() )
	}

	void SimpleBuffer::cleanupSelf() {

	}
	




bool SimpleBuffer::hasValidBufferID() const {
	if (this->ID() == 0u) //No need for a call to Graphics Context if ID is 0
		return false;
	else {
		return ( static_cast<bool>(glIsBuffer(mBufferID)) );
	}
}

} //Namespace ShaderInterface

