#include "SimpleBuffer.h"

#define DEFAULT_BUFFER_MEMORY_SIZE 1024*1024    //1 MiB 

namespace ShaderInterface {

	SimpleBuffer::SimpleBuffer() {
		mVAO = 0u;
		mBufferID = 0u;
		mBufferSize = 0;
		mDataWasLoadedToGpu = false;
		glGenVertexArrays(1, &mVAO);
		if (mVAO == 0u)
			fprintf(WRNLOG, "\nError creating VAO for simple buffer! Object will not behave properly!\n");
	}


	SimpleBuffer::~SimpleBuffer() {
		if ((mVAO != 0u) || (mBufferID != 0))
			cleanupSelf();
	}

	SimpleBuffer::SimpleBuffer(SimpleBuffer&& that) {
		//Move VAO Ownership
		mVAO = that.mVAO;
		that.mVAO = 0u;

		//Move Buffer Ownership
		mBufferID = that.mBufferID;
		that.mBufferID = 0u;

		//Move Stored data
		mVertData = std::move(that.mVertData);

		//Transfer Object state
		mDataWasLoadedToGpu = that.mDataWasLoadedToGpu;
		that.mDataWasLoadedToGpu = false;

		mBufferSize = that.mBufferSize;
		that.mBufferSize = 0;
	}

	//void SimpleBuffer::createGLBuffer() {
	void SimpleBuffer::create() {
		if (ID() != 0u) {
			fprintf(WRNLOG, "\nWARNING! Unable to create a GLBuffer for this SimpleBuffer obj\n"
				"because a GLBuffer has already been created with ID %u", ID());
			return;
		}

		if (mVAO == 0u) {
			fprintf(WRNLOG, "\nWarning! No valid VAO has been created yet for this simple buffer!\n");
			fprintf(WRNLOG, "An attempt will be made to generate a new VAO.\n");
			glGenVertexArrays(1, &mVAO);
			if (mVAO == 0u) {
				fprintf(WRNLOG, "Error creating a VAO!\n");
				return;
			}
			else {
				fprintf(WRNLOG, "A VAO was successfully created!\n");
			}
		}

		glBindVertexArray(mVAO);

		glCreateBuffers(1, &mBufferID);

		//see: https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glBufferStorage.xhtml
		glNamedBufferStorage(mBufferID,									 //Buffer Name (ID)
							 DEFAULT_BUFFER_MEMORY_SIZE,						 //1 MiB of space
							 NULL,										 //No initial data
							 /* GL_MAP_WRITE_BIT | */GL_DYNAMIC_STORAGE_BIT); //Allow writing to buffer | Allow updating using glNamedBufferSubData
		mBufferSize = DEFAULT_BUFFER_MEMORY_SIZE;

		//Should I check here to see if the buffer had its memory allocated successfully?
																		 
		//glBindBuffer(GL_ARRAY_BUFFER, mBufferID); //No need to call this here yet...

		glBindVertexArray(0u);

		if (ID() != 0u) 
			fprintf(MSGLOG, "SimpleBuffer was created in OpenGL with ID: %u\n", ID());
		else 
			fprintf(ERRLOG, "\nERROR CREATING BUFFER!\n");
		
	}

	void SimpleBuffer::attach(const std::vector<GLfloat> & vertData) {
		if (hasVertData()) {
			fprintf(WRNLOG, "\nWARNING! This SimpleBuffer already has data attached, so unable to\n"
				"attach new data!\n");
			return;
		}
		else if (vertData.size() == 0u) {
			fprintf(WRNLOG, "\nWARNING! attachVertices called on a SimpleBuffer object but the provided\n"
				"data is empty!\n");
			return;
		}
		else if ((vertData.size() * sizeof(GLfloat)) > BUFFER_MEMORY_SIZE) {
			fprintf(WRNLOG, "\nWARNING! Attempting to attach data larger than the size of this buffer!\nPlease write more code to do this!\n");
			return;
		}

		//else 	
		std::vector<GLfloat>(vertData).swap(mVertData); //This is a fancy-schmancy way of copying vertData to mVertData
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
		else { 
			//Load the data to the GPU
			//NOTE: The difference between named and unnamed is unnamed uses the current buffer bound to the context
			//glBufferStorage
			//see: https://stackoverflow.com/questions/27810542/what-is-the-difference-between-glbufferstorage-and-glbufferdata

			glBindVertexArray(mVAO);
			glBindBuffer(GL_ARRAY_BUFFER, mBufferID);
			
			
			glNamedBufferSubData(mBufferID,                             //bufferName      
								 0,			                            //offset  (see https://gamedev.stackexchange.com/questions/54868/glbuffersubdata-and-offset-alignment )
								 mVertData.size() * sizeof(GLfloat),    //size
								 mVertData.data() );					//void ptr to start of data

			mDataWasLoadedToGpu = true;
			
		}
	}


	void SimpleBuffer::bindToContext() const {
		if ( !hasValidBufferID() || !(mVAO == 0) )
			return;

		glBindVertexArray(mVAO);
		glBindBuffer(GL_ARRAY_BUFFER, mBufferID);
	}


	//Clears the buffer with the specified value. Will clear the whole buffer.
	//I am guessing a bit here as to the data formats (2nd, 5th and 6th parameters)
	inline void SimpleBuffer::clear(const GLfloat value) {
		glClearNamedBufferSubData(mBufferID,           //name
								  GL_R32F,             //Internal format (I am guessing here)
								  0,                   //offset
								  BUFFER_MEMORY_SIZE,  //Size
								  GL_R32F,             //Format (of input value)  (again I am guessing here)
								  GL_FLOAT,			   //Data type
								  &value);             //Clear Value
		std::vector<GLfloat>::iterator iter = mVertData.begin();
		for (; iter != mVertData.end(); iter++)
			(*iter) = value;

	}


	void SimpleBuffer::cleanupSelf() {
		glDeleteVertexArrays(1, &mVAO);
		mVAO = 0u;
		glDeleteBuffers(1, &mBufferID);
		mBufferID = 0u;
		mDataWasLoadedToGpu = false;
	}
	




bool SimpleBuffer::hasValidBufferID() const {
	if (this->ID() == 0u) //No need for a call to Graphics Context if ID is 0
		return false;
	else {
		return ( static_cast<bool>(glIsBuffer(mBufferID)) );
	}
}


SimpleBuffer& SimpleBuffer::operator=(SimpleBuffer&& that) {
	//Move VAO Ownership
	mVAO = that.mVAO;
	that.mVAO = 0u;

	//Move Buffer Ownership
	mBufferID = that.mBufferID;
	that.mBufferID = 0u;

	//Move Stored data
	mVertData = std::move(that.mVertData);
	
	//Transfer Object state
	mDataWasLoadedToGpu = that.mDataWasLoadedToGpu;
	that.mDataWasLoadedToGpu = false;

	return *this;
}

} //Namespace ShaderInterface

