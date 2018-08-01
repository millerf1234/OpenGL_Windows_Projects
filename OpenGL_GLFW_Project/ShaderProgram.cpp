//File:      ShaderProgram.cpp 
//Class:     ShaderProgram
//Namespace: Global
//Programmer: Forrest Miller
//Date(s): Late July - Early August 2018     
//
//  See the header file for more detail
//

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

		this->mState.mHasVert = that.mState.mHasVert;
		this->mState.mHasGeom = that.mState.mHasGeom;
		this->mState.mHasTessc = that.mState.mHasTessc;
		this->mState.mHasTesse = that.mState.mHasTesse;
		this->mState.mHasFrag = that.mState.mHasFrag;

		this->mState.mValid = that.mState.mValid;
		that.mState.mValid = false;
		this->mState.mError = that.mState.mError;
		this->mState.mReadyToLink = that.mState.mReadyToLink;
		
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

		this->mState.mHasVert = that.mState.mHasVert;
		this->mState.mHasGeom = that.mState.mHasGeom;
		this->mState.mHasTessc = that.mState.mHasTessc;
		this->mState.mHasTesse = that.mState.mHasTesse;
		this->mState.mHasFrag = that.mState.mHasFrag;

		this->mState.mValid = that.mState.mValid;
		that.mState.mValid = false;
		this->mState.mLinked = that.mState.mLinked;
		this->mState.mError = that.mState.mError;
		this->mState.mReadyToLink = that.mState.mReadyToLink;

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
		if (mState.mLinked) {
			fprintf(WRNLOG, "\nWarning! Unable to attach vert shader \"%s\"\nto this ShaderProgram "
				"because this ShaderProgram has already been linked!\n", vert);
			return false;
		}
		if (mState.mHasVert) {
			fprintf(WRNLOG, "\nWARNING! Unable to attach Vertex shader \"%s\" to this shaderprogram\n"
				"because this shaderprogram already has vert \"%s\" attached!\n", vert, mVertexShader->getFilepath());
			return false;
		}
		if (mState.mHasCompute) {
			fprintf(WRNLOG, "\nWarning! Unable to attach Vertex shader \"%s\"\nto this "
				"ShaderProgram object because this ShaderProgram object has a Compute shader attached to it!\n", vert);
			return false;
		}
		mVertexShader = std::make_unique<VertexShader>(vert);
		if (!(mVertexShader->readyToBeAttached())) { //If something went wrong creating the vert shader
			mVertexShader = nullptr;
			return false;
		}
		//Else we have a valid VertexShader compiled and ready for attachment, so we can
		glAttachShader(mProgramID, mVertexShader->ID());
		mState.mHasVert = true;
		//Check to see if ShaderProgram is ready to be linked
		if (mState.mHasFrag) { 
			if (mState.mHasTessc) {
				if (mState.mHasTesse) {
					mState.mReadyToLink = true;
				}
				else { //Still need a tesse to be ready to link
					mState.mReadyToLink = false;
				}
			}
			else if (mState.mHasTesse) {
				if (mState.mHasTessc) {
					mState.mReadyToLink = true;
				}
				else { //Still need a tessc to be ready to link
					mState.mReadyToLink = false;
				}
			}
			else { //If neither tesselation shaders are attached
				mState.mReadyToLink = true;
			}
		}
		else {
			mState.mReadyToLink = false; //Still need a Fragment shader before able to link
		}
		return true;
	}

	
	void ShaderProgram::attachVert(const ShaderInterface::VertexShader * vert) {
		if (vert == nullptr) { return; }
		if (mState.mLinked) {
			fprintf(WRNLOG, "\nUnable to attach %s to shaderProgram because ShaderProgram has already been linked!\n", vert->getFilepath());
			return;
		}
		if (!(vert->readyToBeAttached())) {
			fprintf(ERRLOG, "\nError attaching vertex shader to program, shader is invalid!\n");
			return;
		}
		if (mState.mHasVert) {
			fprintf(ERRLOG, "\nError attaching vertex shader to program. This program already has a vert shader!\n");
			return;
		}
		if (mState.mHasCompute) {
			fprintf(ERRLOG, "\nError attaching vertex shader \"%s\" to this ShaderProgram!", vert->getFilepath());
			if ()
		}
		//One last extra check (that probably isnt necessary):
		if (vert->ID() == 0u) {
			fprintf(ERRLOG, "\nERROR! Attempting to attach ShaderID 0 as %s vert shader to this program!\n", vert->getFilepath());
			return;
		}
		glAttachShader(mProgramID, vert->ID());
		mState.mHasVert = true;
		if (mState.mHasFrag) {
			mState.mReadyToLink = true;
		}
	}


	bool ShaderProgram::attachGeom(const char * geom) {
		if (mState.mLinked) {
			fprintf(WRNLOG, "\nUnable to attach %s to shaderProgram because ShaderProgram has already been linked!\n", geom);
			return false;
		}
		if ((mState.mHasGeom) || (mGeometryShader != nullptr)) {
			fprintf(WRNLOG, "\nWARNING! Unable to attach Geometry shader \"%s\" to this ShaderProgram\n"
				"because this ShaderProgram already has geom \"%s\" attached!\n", geom, mGeometryShader->getFilepath());
			return false;
		}
		mGeometryShader = std::make_unique<GeometryShader>(geom);
		if (mGeometryShader->readyToBeAttached()) {
			glAttachShader(mProgramID, mGeometryShader->ID());
			mState.mHasGeom = true;
			return true;
		}
		else {
			mGeometryShader = nullptr;
		}
		return false;
	}
	void ShaderProgram::attachGeom(const ShaderInterface::GeometryShader * geom) {
		if (geom == nullptr) { return; }
		if (mState.mLinked) {
			fprintf(WRNLOG, "\nUnable to attach %s to shaderProgram because ShaderProgram has already been linked!\n", geom->getFilepath());
			return;
		}
		if (!(geom->readyToBeAttached())) {
			fprintf(ERRLOG, "\nError attaching geometry shader to program, the geometry shader is invalid!\n");
			return;
		}
		if (mState.mHasGeom) {
			fprintf(ERRLOG, "\nError attaching geometry shader to program. This program already has a geometry shader!\n");
			return;
		}
		//One last extra check (that probably isnt necessary):
		if (geom->ID() == 0u) {
			fprintf(ERRLOG, "\nERROR! Attempting to attach ShaderID 0 as \"%s\" geom shader to this program!\n", geom->getFilepath());
			return;
		}
		glAttachShader(mProgramID, geom->ID());
		mState.mHasGeom = true;
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
		if (mState.mLinked) {
			fprintf(WRNLOG, "\nWARNING! Unable to link this shader program because this shader program has already been linked!\n");
			return;
		}
		else if (!mState.mReadyToLink) {
			fprintf(WRNLOG, "\nWARNING! Unable to link shader program because Shader program is not ready to be linked!\n");
			//Figure out why it object is not ready to link and print out findings
			if (mState.mHasCompute) { //A compute shader must always be used in isolation?
				if (mState.mHasVert) {
					fprintf(WRNLOG, "Shader program is unable to link because program has both a Compute and a Vertex shader attached!\n");
				}
				if (mState.mHasGeom) {
					fprintf(WRNLOG, "Shader program is unable to link because program has both a Compute and a Geometry shader attached!\n");
				}
				if (mState.mHasTessc) {
					fprintf(WRNLOG, "Shader program is unable to link because program has both a Compute and a Tesselation Control shader attached!\n");
				}
				if (mState.mHasTesse) {
					fprintf(WRNLOG, "Shader program is unable to link because program has both a Compute and a Tesselation Evaluation shader attached!\n");
				}
				if (mState.mHasFrag) {
					fprintf(WRNLOG, "Shader program is unable to link because program has both a Compute and a Fragment shader attached!\n");
				}
			}
			else {
				if (mState.mHasTessc && !mState.mHasTesse) {
					fprintf(WRNLOG, "Shader program is unable to link because a Tesselation Control is attached but no Tesselation Evaluation is attached!\n");
				}
				else if (!mState.mHasTessc && mState.mHasTesse) {
					fprintf(WRNLOG, "Shader program is unable to link because a Tesselation Control is attached but no Tesselation Evaluation is attached!\n");
				}
				if (!mState.mHasVert) {
					fprintf(WRNLOG, "Shader Program is unable to link because program has not had a vert shader attached yet!\n");
				}
				if (!mState.mHasFrag) {
					fprintf(WRNLOG, "Shader program is unable to link becuase program has not had a frag shader attached yet!\n");
				}
			}
			return;
		}
		else if (mState.mError) {//If an error has occured before linking
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
				mState.mError = true;
				mState.mReadyToLink = false;
				mState.mLinked = false;
				mState.mValid = false;

				glGetProgramInfoLog(mProgramID, 768, NULL, infoLog);
				fprintf(ERRLOG, "\nERROR OCCURED WHILE LINKING SHADERPROGRAM!\nError log:\n\t%s\n", infoLog);
				return;
			}

			mState.mReadyToLink = false;
			mState.mLinked = true;
			mState.mValid = true;
		}
	}

	
	void ShaderProgram::use() {

	}



	void ShaderProgram::initialize() {
		mProgramID = 0u;
		mState = ProgramState();  //Explicity call default constructor
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


	void ShaderProgram::generateGLProgramHandle() {
		mProgramID = glCreateProgram();
	}

	void ShaderProgram::initializeUniformLocationTracker() {
		uniforms = std::make_unique<UniformLocationTracker>(mProgramID);
	}




//} //namespace ShaderInterface


