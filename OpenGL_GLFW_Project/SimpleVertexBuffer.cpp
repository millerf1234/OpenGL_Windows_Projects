#include "SimpleVertexBuffer.h"

#define DEFAULT_BUFFER_MEMORY_SIZE 1024*1024    //1 MiB 

namespace ShaderInterface {

	//////////////////////////////////////////////
	///////         Construction         /////////
	//////////////////////////////////////////////
	void SimpleVertexBuffer::initialize() {
		mVAO = 0u;
		mBufferID = 0u;
		mBufferSize = 0;
		mDataWasLoadedToGpu = false;
		glGenVertexArrays(1, &mVAO);

		//THis comment is from when before I realized how VAO's worked (I still don't fully understand them)
		//
		// glIsVertexArray will return false until the vertex array is bound for the first time. So not sure how to check initially that a VAO was created. Guess I just need to assume OpenGL knows what it's doing...
		//
		//fprintf(MSGLOG, "\nglIsVertexArray returns %c\n", glIsVertexArray(mVAO)); //For Debug
		//"If array is a name returned by glGenVertexArrays, by that has not yet been bound through a call to glBindVertexArray, then the name is not a vertex array object and glIsVertexArray returns GL_FALSE." //From https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glIsVertexArray.xhtml
		//if ((mVAO == 0u) || (glIsVertexArray(mVAO) == GL_FALSE)) {
		//	fprintf(WRNLOG, "\nError creating VAO for simple buffer! Object may not behave properly!\n");
		//}
	}

	SimpleVertexBuffer::SimpleVertexBuffer() {
		initialize();
		mBufferSize = DEFAULT_BUFFER_MEMORY_SIZE;
	}

	SimpleVertexBuffer::SimpleVertexBuffer(GLsizeiptr size) {
		initialize();
		mBufferSize = size;
	}


	/////////////////////////////////////////////
	///////         Destruction         /////////
	/////////////////////////////////////////////
	void SimpleVertexBuffer::cleanupSelf() {
		glDeleteVertexArrays(1, &mVAO);
		mVAO = 0u;
		glDeleteBuffers(1, &mBufferID);
		mBufferID = 0u;
		mDataWasLoadedToGpu = false;
	}

	SimpleVertexBuffer::~SimpleVertexBuffer() {
		if ((mVAO != 0u) || (mBufferID != 0))
			cleanupSelf();
	}


	//////////////////////////////////////////////
	///////            Moving            /////////
	//////////////////////////////////////////////
	//Move Constructor
	SimpleVertexBuffer::SimpleVertexBuffer(SimpleVertexBuffer&& that) {
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

	//Move operator
	SimpleVertexBuffer& SimpleVertexBuffer::operator=(SimpleVertexBuffer&& that) {
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

	
	/////////////////////////////////////////////
	///////       Public Interface      /////////
	/////////////////////////////////////////////
	
	void SimpleVertexBuffer::create() {
		if ( mBufferID != 0u || glIsBuffer(mBufferID) ) {
			fprintf(WRNLOG, "\nWARNING! Unable to create a GLBuffer for this SimpleBuffer obj\n"
				"because a GLBuffer has already been created with ID %u", mBufferID);
			return;
		}

		if ( mVAO == 0u || !glIsVertexArray(mVAO) ) {
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
							 mBufferSize,						         //Storage space to allocate
							 NULL,										 //No initial data
							 GL_DYNAMIC_STORAGE_BIT);                    //Allow updating using glNamedBufferSubData
		
		
		//Should I check here to see if the buffer had its memory allocated successfully?
		/*
		if (mBufferID != 0u)
			fprintf(MSGLOG, "SimpleBuffer was created in OpenGL with ID: %u\n", ID());
		else
			fprintf(ERRLOG, "\nERROR CREATING BUFFER!\n");          */

		//glBindBuffer(GL_ARRAY_BUFFER, mBufferID); //No need to call this here yet...
		glBindVertexArray(0u);

	}

	void SimpleVertexBuffer::attach(const std::vector<GLfloat> & vertData) {
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
		else if (static_cast<GLsizeiptr>((vertData.size() * sizeof(GLfloat))) > mBufferSize) {
			fprintf(ERRLOG, "\nERROR! Attempting to attach data larger than the size of this SimpleBuffer!\n"
				"Simple Buffer's size is %d while provided data is size %d\n", mBufferSize,
				(vertData.size() * sizeof(GLfloat)) );
			return;
		}

		//else 	
		std::vector<GLfloat>(vertData).swap(mVertData); //This is a fancy-schmancy way of copying vertData to mVertData
	
	}


	void SimpleVertexBuffer::setLayout(VertLayout layout) {
		if (!hasValidVAO()) {
			fprintf(WRNLOG, "\nWARNING! This SimpleVertexBuffer does not yet have a valid VAO!\n"
				"Unable to set vertex layout for buffer!\n");
			return;
		}
		else {
			fprintf(MSGLOG, "\nSpecifing Vertex Layout...\n");
		}
		glBindVertexArray(mVAO);

		switch (layout) {
			CASE_ENUM(VertLayout::VERT2);
			//see pages 111-112 in OpenGL SUper Bible
			glVertexArrayVertexBuffer(mVAO, 0, mBufferID, 0, sizeof(GLfloat) * 2); //This binds our VBO to our VAO
			glVertexArrayAttribFormat(mVAO, 0, 2, GL_FLOAT, GL_FALSE, 0);
			glEnableVertexArrayAttrib(mVAO, 0);
			break;
			CASE_ENUM(VertLayout::VERT3);
			fprintf(MSGLOG, "\nNot Yet Implemented!\n");
			break;
		}
	}

	void SimpleVertexBuffer::loadBufferToGPU() {
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

			//glBindVertexArray(mVAO); //This might not be necessary
			//glBindBuffer(GL_ARRAY_BUFFER, mBufferID);
			
			
			glNamedBufferSubData(mBufferID,                             //bufferName      
								 0,			                            //offset  (see https://gamedev.stackexchange.com/questions/54868/glbuffersubdata-and-offset-alignment )
								 mVertData.size() * sizeof(GLfloat),    //size
								 mVertData.data() );					//void ptr to start of data

			mDataWasLoadedToGpu = true;
			
		}
	}


	void SimpleVertexBuffer::bindToContext() const {
		if ( !hasValidBufferID() || (mVAO == 0) )
			return;

		glBindVertexArray(mVAO);
		glBindBuffer(GL_ARRAY_BUFFER, mBufferID);
	}


	//Clears the buffer with the specified value. Will clear the whole buffer.
	//I am guessing a bit here as to the data formats (2nd, 5th and 6th parameters)
	inline void SimpleVertexBuffer::clear(const GLfloat value) {
		if (!hasValidBufferID())
			return;

		//Clear OpenGL's copy of the data
		glClearNamedBufferSubData(mBufferID,           //name
								  GL_R32F,             //Internal format (I am guessing here)
								  0,                   //offset
								  mBufferSize,  //Size
								  GL_R32F,             //Format (of input value)  (again I am guessing here)
								  GL_FLOAT,			   //Data type
								  &value);             //Clear Value

		//Clear this objects copy of the data
		std::vector<GLfloat>::iterator iter = mVertData.begin();
		for (; iter != mVertData.end(); iter++)
			(*iter) = value;

	}

	bool SimpleVertexBuffer::hasValidBufferID() const {
		if (this->mBufferID == 0u) //No need for a call to Graphics Context if ID is 0
			return false;
		else 
			return ( static_cast<bool>(glIsBuffer(mBufferID)) );
	
	}

	bool SimpleVertexBuffer::hasValidVAO() const {
		if (this->mVAO == 0u)
			return false;
		else
			return ( static_cast<bool>(glIsVertexArray(mVAO)) );
	}


} //Namespace ShaderInterface

