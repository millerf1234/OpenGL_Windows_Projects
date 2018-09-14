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
		if (vert == nullptr) { return false; }
		if (mState.mLinked) {
			fprintf(WRNLOG, "\nWarning! Unable to attach vert shader \"%s\"\nto this ShaderProgram "
				"because this ShaderProgram has already been linked!\n", vert);
			return false;
		}
		if (mState.mHasVert) {
			fprintf(WRNLOG, "\nWARNING! Unable to attach Vertex shader \"%s\" to this shaderprogram\n"
				"because this shaderprogram already has a vertex shader attached!\n", vert);
			return false;
		}
		if (mState.mHasCompute) {
			fprintf(WRNLOG, "\nWarning! Unable to attach Vertex shader \"%s\"\nto this "
				"ShaderProgram object because this ShaderProgram object has a Compute shader attached to it!\n", vert);
			return false;
		}
		mVertexShader = std::make_unique<VertexShader>(vert);
		if (!(mVertexShader->readyToBeAttached())) { //If something went wrong creating the vert shader
			mVertexShader.release();
			return false;
		}
		//Else we have a valid VertexShader compiled and ready for attachment, so we can attach it to our object
		glAttachShader(mProgramID, mVertexShader->ID());
		mState.mHasVert = true;
		mState.mReadyToLink = checkToSeeIfReadyToLinkAfterAttachingVert();
		return true;
	}

	void ShaderProgram::attachVert(const ShaderInterface::VertexShader * vert) {
		if (vert == nullptr) { return; }
		if (mState.mLinked) {
			fprintf(WRNLOG, "\nUnable to attach %s to shaderProgram because ShaderProgram has already been linked!\n", vert->getFilepath());
			return;
		}
		if (mState.mHasVert) {
			fprintf(ERRLOG, "\nError attaching vertex shader to program. This program already has a vert shader!\n");
			return;
		}
		if (mState.mHasCompute) {
			fprintf(WRNLOG, "\nWarning! Unable to attach Vertex shader \"%s\"\nto this "
				"ShaderProgram object because this ShaderProgram object has a Compute shader attached to it!\n", vert->getFilepath());
			return;
		}
		if (!(vert->readyToBeAttached())) {
			fprintf(ERRLOG, "\nError attaching vertex shader to program, shader is not currently ready to be attached!\n");
			return;
		}
		
		//One last extra check (that probably isnt necessary):
		if ( (vert->ID() == 0u) || (vert->type() != ShaderType::VERTEX) ) {
			fprintf(ERRLOG, "\nERROR! Attempting to attach ShaderID 0 as %s vert shader to this program!\n", vert->getFilepath());
			return;
		}
		glAttachShader(mProgramID, vert->ID());
		mState.mHasVert = true;
		mState.mReadyToLink = checkToSeeIfReadyToLinkAfterAttachingVert();
	}


	bool ShaderProgram::attachGeom(const char * geom) {
		if (geom == nullptr) { return false; }
		if (mState.mLinked) {
			fprintf(WRNLOG, "\nUnable to attach %s to shaderProgram because ShaderProgram has already been linked!\n", geom);
			return false;
		}
		if (mState.mHasGeom) {
			fprintf(WRNLOG, "\nWARNING! Unable to attach Geometry shader \"%s\" to this ShaderProgram\n"
				"because this ShaderProgram already has a Geom attached!\n", geom);
			return false;
		}
		if (mState.mHasCompute) {
			fprintf(WRNLOG, "\nWARNING! Unable to attach Geometry shader \"%s\"\nto this ShaderProgram "
				"because this ShaderProgram already has a Compute attached!\n", geom);
			return false;
		}
		mGeometryShader = std::make_unique<GeometryShader>(geom);
		if ( (mGeometryShader->readyToBeAttached())  &&  (mGeometryShader->type() == ShaderType::GEOMETRY) ) {
			glAttachShader(mProgramID, mGeometryShader->ID());
			mState.mHasGeom = true;
			return true;
		}
		else {
			mGeometryShader.release();
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
		if ( (geom->ID() == 0u) || (geom->type() != ShaderType::GEOMETRY) ) {
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
		if (tesse == nullptr) { return false; }
		if (mState.mLinked) {
			fprintf(WRNLOG, "\nUnable to attach %s to shaderProgram because ShaderProgram has already been linked!\n", tesse);
			return false;
		}
		if (mState.mHasTesse) {
			fprintf(WRNLOG, "\nWARNING! Unable to attach Tesselation Evaluation shader \"%s\" to this ShaderProgram\n"
				"because this ShaderProgram already has a tesse shader attached!\n", tesse);
			return false;
		}
		if (mState.mHasCompute) {
			fprintf(WRNLOG, "\nWARNING! Unable to attach Tesselation Evaluation shader \"%s\"\nto this ShaderProgram "
				"because this ShaderProgram already has a Compute attached!\n", tesse);
			return false;
		}
		mTesselationEvaluationShader = std::make_unique<TesselationEvaluationShader>(tesse);
		if ( (mTesselationEvaluationShader->readyToBeAttached())  &&  (mTesselationEvaluationShader->type() == ShaderType::TESSELATION_EVALUATION) ) {
			glAttachShader(mProgramID, mTesselationEvaluationShader->ID());
			mState.mHasTesse = true;
			mState.mReadyToLink = checkToSeeIfReadyToLinkAfterAttachingTesse();
			return true;
		}
		else {
			mTesselationEvaluationShader.release();
		}
		return false;
	}
	void ShaderProgram::attachTesse(const ShaderInterface::TesselationEvaluationShader * tesse) {
		//todo
	}
	bool ShaderProgram::attachTessc(const char * tessc) {
		if (tessc == nullptr) { return false; }
		if (mState.mLinked) {
			fprintf(WRNLOG, "\nUnable to attach %s to shaderProgram because ShaderProgram has already been linked!\n", tessc);
			return false;
		}
		if (mState.mHasTessc) {
			fprintf(WRNLOG, "\nWARNING! Unable to attach Tesselation Control shader \"%s\" to this ShaderProgram\n"
				"because this ShaderProgram already has a tessc shader attached!\n", tessc);
			return false;
		}
		if (mState.mHasCompute) {
			fprintf(WRNLOG, "\nWARNING! Unable to attach Tesselation Control shader \"%s\"\nto this ShaderProgram "
				"because this ShaderProgram already has a Compute attached!\n", tessc);
			return false;
		}
		mTesselationControlShader = std::make_unique<TesselationControlShader>(tessc);
		if ((mTesselationControlShader->readyToBeAttached()) && (mTesselationControlShader->type() == ShaderType::TESSELATION_CONTROL)) {
			glAttachShader(mProgramID, mTesselationControlShader->ID());
			mState.mHasTessc = true;
			mState.mReadyToLink = checkToSeeIfReadyToLinkAfterAttachingTessc();
			return true;
		}
		else {
			mTesselationControlShader.release();
		}
		return false;
	}
	void ShaderProgram::attachTessc(const ShaderInterface::TesselationControlShader * tessc) {
		//todo
	}
	bool ShaderProgram::attachFrag(const char * frag) {
		if (frag == nullptr) { return false; }
		if (mState.mLinked) {
			fprintf(WRNLOG, "\nWarning! Unable to attach frag shader \"%s\"\nto this ShaderProgram "
				"because this ShaderProgram has already been linked!\n", frag);
			return false;
		}
		if (mState.mHasFrag) {
			fprintf(WRNLOG, "\nWARNING! Unable to attach Fragment shader \"%s\"\nto this ShaderProgram "
				"because this shaderprogram already has a fragment shader attached!\n", frag);
			return false;
		}
		if (mState.mHasCompute) {
			fprintf(WRNLOG, "\nWarning! Unable to attach Fragment shader \"%s\"\nto this "
				"ShaderProgram object because this ShaderProgram object has a Compute shader attached to it!\n", frag);
			return false;
		}
		mFragmentShader = std::make_unique<FragmentShader>(frag);
		if ( !(mFragmentShader->readyToBeAttached())  ||  (mFragmentShader->type() != ShaderType::FRAGMENT) ) { //If something went wrong creating the fragment shader
			mFragmentShader.release();
			return false;
		}
		//Else we have a valid FragmentShader compiled and ready for attachment, so we can
		glAttachShader(mProgramID, mFragmentShader->ID());
		mState.mHasFrag = true;
		mState.mReadyToLink = checkToSeeIfReadyToLinkAfterAttachingFrag();
		return true;
	}

	void ShaderProgram::attachFrag(const ShaderInterface::FragmentShader * frag) {
		//todo
	}



	bool ShaderProgram::attachCompute(const char * compute) {
		if (compute == nullptr) { return false; }
		if (mState.mLinked) {
			fprintf(WRNLOG, "\nWarning! Unable to attach Compute shader \"%s\"\nto this ShaderProgram "
				"because this ShaderProgram has already been linked!\n", compute);
			return false;
		}
		return false;
		if (mState.mHasCompute) {
			fprintf(WRNLOG, "\nWARNING! Unable to attach Compute shader \"%s\"\nto this ShaderProgram "
				"because this shaderProgram has already been linked", compute);
			return false;
		}
		if (mState.mHasVert || mState.mHasGeom || mState.mHasTessc || mState.mHasTesse || mState.mHasFrag) {
			fprintf(WRNLOG, "\nWARNING! Unable to attach Compute shader \"%s\"\nto this ShaderProgram "
				"because this shader program has already had a differnt type of shader attached.\nCompute "
				"shaders must always be bound to their own ShaderProgram objects without\n"
				"any other shaders.\n", compute);
		}
		mComputeShader = std::make_unique<ComputeShader>(compute);
		if (!(mComputeShader->readyToBeAttached()) || (mComputeShader->type() != ShaderType::COMPUTE)) { //If something went wrong creating the Compute shader
			mFragmentShader.release();
			return false;
		}
		//Else we have a valid ComputeShader compiled and ready for attachment, so we can
		glAttachShader(mProgramID, mComputeShader->ID());
		mState.mHasCompute = true;
		mState.mReadyToLink = true;
		return true;

	}

	void ShaderProgram::attachCompute(const ShaderInterface::ComputeShader * compute) {
		//to do
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
		//todo
		fprintf(MSGLOG, "\nThe function 'use' within the shaderProgram class is not yet implemented!\n");
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

	bool ShaderProgram::checkToSeeIfReadyToLinkAfterAttachingVert() const {
		//Check to see if ShaderProgram is ready to be linked
		if (mState.mHasFrag) {
			if (mState.mHasTessc) {
				if (mState.mHasTesse) 
					return true;
				else                     //Still need a tesse to be ready to link
					return false;
			}
			else if (mState.mHasTesse) {
				if (mState.mHasTessc) 
					return true;
				else                      //Still need a tessc to be ready to link
					return false;	
			}
			else            //If neither tesselation shaders are attached
				return true;
		}
		//else 
		return false;; //Still need a Fragment shader before able to link	
	}

	bool ShaderProgram::checkToSeeIfReadyToLinkAfterAttachingTesse() const {
		if (mState.mHasVert && mState.mHasFrag) {
			if (!mState.mHasTessc) 
				return false;
			return true;
		} //else if missing either vert or frag
		return false;
	}

	bool ShaderProgram::checkToSeeIfReadyToLinkAfterAttachingTessc() const {
		if (mState.mHasVert && mState.mHasFrag) {
			if (!mState.mHasTesse)
				return false;
			return true;
		} //else if missing either vert or frag
		return false;
	}

	bool ShaderProgram::checkToSeeIfReadyToLinkAfterAttachingFrag() const {
		//Check to see if ShaderProgram is ready to be linked
		if (mState.mHasVert) {
			if (mState.mHasTessc) {
				if (mState.mHasTesse)
					return true;
				else                     //Still need a tesse to be ready to link
					return false;
			}
			else if (mState.mHasTesse) {
				if (mState.mHasTessc)
					return true;
				else                      //Still need a tessc to be ready to link
					return false;
			}
			else            //If neither tesselation shaders are attached
				return true;
		}
		//else 
		return false;; //Still need a Vertex shader before able to link	

	}

//} //namespace ShaderInterface


