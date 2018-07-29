#include "ShaderProgram.h"

using namespace ShaderInterface;  //Hopefully this doesn't get imported into the global namespace

	ShaderProgram::ShaderProgram() {
		initialize(); //Gives initial values to member variables
		
	}


	ShaderProgram::~ShaderProgram() {
		if (mProgramID != 0u) {
			fprintf(MSGLOG, "\nDeleting ShaderProgram %u!\n", mProgramID);
			glDeleteProgram(mProgramID);
			mProgramID = 0u;
		}
	}

	//Move Constructor
	ShaderProgram::ShaderProgram(ShaderProgram&& that) {
		this->mProgramID = that.mProgramID;
		that.mProgramID = 0u;

		this->mHasVert = that.mHasVert;
		this->mHasGeom = that.mHasGeom;
		this->mHasTessc = that.mHasTessc;
		this->mHasTesse = that.mHasTesse;
		this->mHasFrag = that.mHasFrag;

		this->mValid = that.mValid;
		that.mValid = false;
		this->mError = that.mError;
		this->mReadyToLink = that.mReadyToLink;
		
		if (that.mVertexShader) {
			this->mVertexShader = std::move(that.mVertexShader);
		}
		if (that.mGeometryShader) {
			this->mGeometryShader = std::move(that.mGeometryShader);
		}
		if (that.mTesselationControlShader) {
			this->mTesselationControlShader = std::move(that.mTesselationControlShader);
		}
		if (that.mTesselationEvaluationShader) {
			this->mTesselationEvaluationShader = std::move(that.mTesselationEvaluationShader);
		}
		if (that.mFragmentShader) {
			this->mFragmentShader = std::move(that.mFragmentShader);
		}
		if (that.mVertexShader) {
			this->mVertexShader = std::move(that.mVertexShader);
		}
		this->uniforms = std::move(that.uniforms);
	}
	
	//Move assignment operator
	ShaderProgram& ShaderProgram::operator=(ShaderProgram&& that) {
		this->mProgramID = that.mProgramID;
		that.mProgramID = 0u;

		this->mHasVert = that.mHasVert;
		this->mHasGeom = that.mHasGeom;
		this->mHasTessc = that.mHasTessc;
		this->mHasTesse = that.mHasTesse;
		this->mHasFrag = that.mHasFrag;

		this->mValid = that.mValid;
		that.mValid = false;
		this->mLinked = that.mLinked;
		this->mError = that.mError;
		this->mReadyToLink = that.mReadyToLink;

		if (that.mVertexShader) {
			this->mVertexShader = std::move(that.mVertexShader);
		}
		if (that.mGeometryShader) {
			this->mGeometryShader = std::move(that.mGeometryShader);
		}
		if (that.mTesselationControlShader) {
			this->mTesselationControlShader = std::move(that.mTesselationControlShader);
		}
		if (that.mTesselationEvaluationShader) {
			this->mTesselationEvaluationShader = std::move(that.mTesselationEvaluationShader);
		}
		if (that.mFragmentShader) {
			this->mFragmentShader = std::move(that.mFragmentShader);
		}
		if (that.mComputeShader) {
			this->mComputeShader = std::move(that.mComputeShader);
		}
		this->uniforms = std::move(that.uniforms);
		return *this;
	}

	//Interface functions
	bool ShaderProgram::attachVert(const char * vert) {
		if (mLinked) {
			fprintf(WRNLOG, "\nUnable to attach %s to shaderProgram because ShaderProgram has already been linked!\n", vert);
			return false;
		}
		if ((mHasVert) || (mVertexShader != nullptr)) {
			fprintf(WRNLOG, "\nWARNING! Unable to attach Vertex shader \"%s\" to this shaderprogram\n"
				"because this shaderprogram already has vert \"%s\" attached!\n", vert, mVertexShader->getFilepath());
			return false;
		}
		mVertexShader = std::make_unique<VertexShader>(vert);
		if (mVertexShader->valid()) {
			glAttachShader(mProgramID, mVertexShader->shaderID());
			mHasVert = true;
			if (mHasFrag && !mHasCompute) {
				mReadyToLink = true;
			}
			return true;
		}
		else {
			mVertexShader = nullptr;
		}
		return false;
	}
	void ShaderProgram::attachVert(const ShaderInterface::VertexShader * vert) {
		if (vert == nullptr) { return; }
		if (mLinked) {
			fprintf(WRNLOG, "\nUnable to attach %s to shaderProgram because ShaderProgram has already been linked!\n", vert->getFilepath());
			return;
		}
		if (!(vert->valid())) {
			fprintf(ERRLOG, "\nError attaching vertex shader to program, shader is invalid!\n");
			return;
		}
		if (mHasVert) {
			fprintf(ERRLOG, "\nError attaching vertex shader to program. This program already has a vert shader!\n");
			return;
		}
		//One last extra check (that probably isnt necessary):
		if (vert->shaderID() == 0u) {
			fprintf(ERRLOG, "\nERROR! Attempting to attach ShaderID 0 as %s vert shader to this program!\n", vert->getFilepath());
			return;
		}
		glAttachShader(mProgramID, vert->shaderID());
		mHasVert = true;
		if (mHasFrag && !mHasCompute) {
			mReadyToLink = true;
		}
	}



	bool ShaderProgram::attachGeom(const char * geom) {
		if (mLinked) {
			fprintf(WRNLOG, "\nUnable to attach %s to shaderProgram because ShaderProgram has already been linked!\n", geom);
			return false;
		}
		if ((mHasGeom) || (mGeometryShader != nullptr)) {
			fprintf(WRNLOG, "\nWARNING! Unable to attach Geometry shader \"%s\" to this ShaderProgram\n"
				"because this ShaderProgram already has geom \"%s\" attached!\n", geom, mGeometryShader->getFilepath());
			return false;
		}
		mGeometryShader = std::make_unique<GeometryShader>(geom);
		if (mGeometryShader->valid()) {
			glAttachShader(mProgramID, mGeometryShader->shaderID());
			mHasGeom = true;
			return true;
		}
		else {
			mGeometryShader = nullptr;
		}
		return false;
	}
	void ShaderProgram::attachGeom(const ShaderInterface::GeometryShader * geom) {
		if (geom == nullptr) { return; }
		if (mLinked) {
			fprintf(WRNLOG, "\nUnable to attach %s to shaderProgram because ShaderProgram has already been linked!\n", geom->getFilepath());
			return;
		}
		if (!(geom->valid())) {
			fprintf(ERRLOG, "\nError attaching geometry shader to program, the geometry shader is invalid!\n");
			return;
		}
		if (mHasGeom) {
			fprintf(ERRLOG, "\nError attaching geometry shader to program. This program already has a geometry shader!\n");
			return;
		}
		//One last extra check (that probably isnt necessary):
		if (geom->shaderID() == 0u) {
			fprintf(ERRLOG, "\nERROR! Attempting to attach ShaderID 0 as \"%s\" geom shader to this program!\n", geom->getFilepath());
			return;
		}
		glAttachShader(mProgramID, geom->shaderID());
		mHasGeom = true;
	}
	


	void ShaderProgram::attachTess(const ShaderInterface::TesselationControlShader * tessc, const ShaderInterface::TesselationEvaluationShader * tesse) {
		attachTesse(tesse); 
		attachTessc(tessc);
	}
	bool ShaderProgram::attachTesse(const char * tesse) {
		return false;
	}
	void ShaderProgram::attachTesse(const ShaderInterface::TesselationEvaluationShader * tesse) {

	}
	bool ShaderProgram::attachTessc(const char * tessc) {
		return false;
	}
	void ShaderProgram::attachTessc(const ShaderInterface::TesselationControlShader * tessc) {

	}
	bool ShaderProgram::attachFrag(const char * frag) {
		return false;
	}
	void ShaderProgram::attachFrag(const ShaderInterface::FragmentShader * frag) {

	}
	bool ShaderProgram::attachCompute(const char * compute) {
		return false;
	}
	void ShaderProgram::attachCompute(const ShaderInterface::ComputeShader * compute) {

	}

	void ShaderProgram::link(bool cacheLocalShaders) {
		if (mLinked) {
			fprintf(WRNLOG, "\nWARNING! Unable to link this shader program because this shader program has already been linked!\n");
			return;
		}
		else if (!mReadyToLink) { 
			fprintf(WRNLOG, "\nWARNING! Unable to link shader program because Shader program is not ready to be linked!\n");
			//Figure out why it object is not ready to link and print out findings
			if (mHasCompute) { //A compute shader must always be used in isolation?
				if (mHasVert) {
					fprintf(WRNLOG, "Shader program is unable to link because program has both a Compute and a Vertex shader attached!\n");
				}
				if (mHasGeom) {
					fprintf(WRNLOG, "Shader program is unable to link because program has both a Compute and a Geometry shader attached!\n");
				}
				if (mHasTessc) {
					fprintf(WRNLOG, "Shader program is unable to link because program has both a Compute and a Tesselation Control shader attached!\n");
				}
				if (mHasTesse) {
					fprintf(WRNLOG, "Shader program is unable to link because program has both a Compute and a Tesselation Evaluation shader attached!\n");
				}
				if (mHasFrag) {
					fprintf(WRNLOG, "Shader program is unable to link because program has both a Compute and a Fragment shader attached!\n");
				}
			}
			else {
				if (mHasTessc && !mHasTesse) {
					fprintf(WRNLOG, "Shader program is unable to link because a Tesselation Control is attached but no Tesselation Evaluation is attached!\n");
				}
				else if (!mHasTessc && mHasTesse) {
					fprintf(WRNLOG, "Shader program is unable to link because a Tesselation Control is attached but no Tesselation Evaluation is attached!\n");
				}
				if (!mHasVert) {
					fprintf(WRNLOG, "Shader Program is unable to link because program has not had a vert shader attached yet!\n");
				}
				if (!mHasFrag) {
					fprintf(WRNLOG, "Shader program is unable to link becuase program has not had a frag shader attached yet!\n");
				}
			}
			return;
		}
		else if (mError) {//If an error has occured before linking
			fprintf(ERRLOG, "\nERROR! Unable to link ShaderProgram because it encountered an error!\n");
			return;
		}
		else { //Program is ready to be linked and hasn't already been linked and no errors 
			GLchar infoLog[768]; //Used to store results of linking
			GLint success;

			//Link the program
			glLinkProgram(mProgramID);
			glGetProgramiv(mProgramID, GL_LINK_STATUS, &success);
			if (!success) {
				mError = true;
				mReadyToLink = false;
				mLinked = false;
				mValid = false;

				glGetProgramInfoLog(mProgramID, 768, NULL, infoLog);
				fprintf(ERRLOG, "\nERROR OCCURED WHILE LINKING SHADERPROGRAM!\nError log:\n\y%s\n", infoLog);
				return;
			}

			mReadyToLink = false;
			mLinked = true;
			mValid = true;
		}
	}

	//
	void ShaderProgram::use() {

	}



	void ShaderProgram::initialize() {
		mProgramID = 0u;
		initializeBooleans();
		uniforms = nullptr;
		mVertexShader = nullptr;
		mGeometryShader = nullptr;
		mTesselationControlShader = nullptr;
		mTesselationEvaluationShader = nullptr;
		mFragmentShader = nullptr;
		mComputeShader = nullptr;

		generateGLProgramHandle(); //Creates a new program with OpenGL
		initializeUniformLocationTracker();
	}

	void ShaderProgram::initializeBooleans() {
		mValid = false;
		mLinked = false;
		mHasVert = false;
		mHasGeom = false;
		mHasTesse = false;
		mHasTessc = false;
		mHasFrag = false;
		mHasCompute = false;
		mError = false;
		mReadyToLink = false;
	}

	void ShaderProgram::generateGLProgramHandle() {
		mProgramID = glCreateProgram();
	}

	void ShaderProgram::initializeUniformLocationTracker() {
		uniforms = std::make_unique<UniformLocationTracker>(mProgramID);
	}




//} //namespace ShaderInterface


