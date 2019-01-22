//File:      ShaderProgram.cpp 
//Class:     ShaderProgram
//Namespace: Global
//Programmer: Forrest Miller
//Date(s): Late July - Early August 2018     
//Update(s):
//  September 14-15, 2018  --  Added support for attaching secondary shaders (i.e. shaders without a 'main()' function)
//  October 23, 2018     --    Added function release() to free shader resources allocated from/by the GL Context 
//
//
//
//  Known Issues:  It is currently possible to attach a primary shader to a shader program, then mark the shader as
//				   a secondary and reattach it using the appropriate 'attachSecondary' function. The ShaderProgram 
//				   will not be able to link if this happens, but this class will not catch it ahead of time.
//    Workaround:  Don't do that
//
//  See the header file for more detail
//

#include "ShaderProgram.h"


using namespace ShaderInterface;  //Hopefully this doesn't get imported into the global namespace


//Constructor for internal struct that tracks the shader programs state
ShaderProgram::ProgramState::ProgramState() {
	mValid = false;
	mLinked = false;
	mHasVert = false;
	mHasGeom = false;
	mHasTesse = false;
	mHasTessc = false;
	mHasFrag = false;
	mHasCompute = false;
	mAttachedSecondaryVertCount = 0;
	mAttachedSecondaryGeomCount = 0;
	mAttachedSecondaryTesseCount = 0;
	mAttachedSecondaryTesscCount = 0;
	mAttachedSecondaryFragCount = 0;
	mAttachedSecondaryComputeCount = 0;

	mError = false;
	mReadyToLink = false;
	mReleased = false;
}



void ShaderProgram::initialize() {
	mProgramID = 0u;
	mState = ProgramState();  //Explicitly call default constructor
	mVertexShader = nullptr;
	mGeometryShader = nullptr;
	mTesselationControlShader = nullptr;
	mTesselationEvaluationShader = nullptr;
	mFragmentShader = nullptr;
	mComputeShader = nullptr;

	generateGLProgramHandle(); //Creates a new program with OpenGL
	initializeUniformLocationTracker();
}


ShaderProgram::ShaderProgram() : uniforms() {
	initialize(); //Gives initial values to member variables
}


ShaderProgram::~ShaderProgram() {
	if ( (mProgramID != 0u) || (!(mState.mReleased)) ) {
		release(); 
	}
}

//Move Constructor
ShaderProgram::ShaderProgram(ShaderProgram&& that) noexcept : uniforms() {
	this->mProgramID = that.mProgramID;
	that.mProgramID = 0u;

	this->mState.mHasVert = that.mState.mHasVert;
	this->mState.mHasGeom = that.mState.mHasGeom;
	this->mState.mHasTessc = that.mState.mHasTessc;
	this->mState.mHasTesse = that.mState.mHasTesse;
	this->mState.mHasFrag = that.mState.mHasFrag;

	this->mState.mAttachedSecondaryVertCount = that.mState.mAttachedSecondaryVertCount;
	this->mState.mAttachedSecondaryGeomCount = that.mState.mAttachedSecondaryGeomCount;
	this->mState.mAttachedSecondaryTesseCount = that.mState.mAttachedSecondaryTesseCount;
	this->mState.mAttachedSecondaryTesscCount = that.mState.mAttachedSecondaryTesscCount;
	this->mState.mAttachedSecondaryFragCount = that.mState.mAttachedSecondaryFragCount;
	this->mState.mAttachedSecondaryComputeCount = that.mState.mAttachedSecondaryComputeCount;

	this->mState.mAttachedSecondaries.swap(that.mState.mAttachedSecondaries);

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
ShaderProgram& ShaderProgram::operator=(ShaderProgram&& that) noexcept {
	if (this != &that) {
		this->mProgramID = that.mProgramID;
		that.mProgramID = 0u;

		this->mState.mHasVert = that.mState.mHasVert;
		this->mState.mHasGeom = that.mState.mHasGeom;
		this->mState.mHasTessc = that.mState.mHasTessc;
		this->mState.mHasTesse = that.mState.mHasTesse;
		this->mState.mHasFrag = that.mState.mHasFrag;

		this->mState.mAttachedSecondaryVertCount = that.mState.mAttachedSecondaryVertCount;
		this->mState.mAttachedSecondaryGeomCount = that.mState.mAttachedSecondaryGeomCount;
		this->mState.mAttachedSecondaryTesseCount = that.mState.mAttachedSecondaryTesseCount;
		this->mState.mAttachedSecondaryTesscCount = that.mState.mAttachedSecondaryTesscCount;
		this->mState.mAttachedSecondaryFragCount = that.mState.mAttachedSecondaryFragCount;
		this->mState.mAttachedSecondaryComputeCount = that.mState.mAttachedSecondaryComputeCount;

		this->mState.mAttachedSecondaries.swap(that.mState.mAttachedSecondaries);

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
	}
	return *this;
}

	//Interface functions

	bool ShaderProgram::attachVert(const char * vert) {
		if (vert == nullptr) { return false; }
		if (mState.mLinked) {
			fprintf(WRNLOG, "\nWarning! Unable to attach Vertex shader \"%s\"\n"
                "to this ShaderProgram because this ShaderProgram has already been linked!\n", vert);
			return false;
		}
		if (mState.mHasVert) {
			fprintf(WRNLOG, "\nWARNING! Unable to attach Vertex shader \"%s\" to this ShaderProgram\n"
				"because this ShaderProgram already has a vertex shader attached!\n", vert);
			return false;
		}
		if ( (mState.mHasCompute) || (mState.mAttachedSecondaryComputeCount > 0) ) {
			fprintf(WRNLOG, "\nWarning! Unable to attach Vertex shader \"%s\"\n"
                "to this ShaderProgram object because this ShaderProgram object has\n"
                "a Compute shader attached to it!\n", vert);
			return false;
		}
		mVertexShader = std::make_unique<VertexShader>(vert);
		if ( !(mVertexShader->readyToBeAttached()) ) { //If something went wrong creating the vert shader
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
			fprintf(WRNLOG, "\nUnable to attach %s to shaderProgram because ShaderProgram has already been linked!\n", vert->getFilepath().c_str());
			return;
		}
		if (mState.mHasVert) {
			fprintf(ERRLOG, "\nError attaching vertex shader to program. This program already has a primary vertex shader!\n");
			return;
		}
		if ((mState.mHasCompute) || (mState.mAttachedSecondaryComputeCount > 0)) {
			fprintf(WRNLOG, "\nWarning! Unable to attach Vertex shader \"%s\"\nto this "
				"ShaderProgram object because this ShaderProgram object has a Compute shader attached to it!\n", vert->getFilepath().c_str());
			return;
		}
		if (!(vert->readyToBeAttached())) {
			fprintf(ERRLOG, "\nError attaching Vertex shader to program, shader is not currently ready to be attached!\n");
			return;
		}
		if (vert->markedAsSecondary()) {
			fprintf(ERRLOG, "\nError! The vertex shader \"%s\"\n"
				"has been marked as a secondary shader, which prevents it from being\n"
				"able to be attached to the primary vertex shader spot!\n"
				"Please use the 'attachSecondaryVert()' function to attach this shader.\n", vert->getFilepath().c_str());
			return;
		}
		//One last extra check (that probably isnt necessary):
		if ( (vert->ID() == 0u) || (vert->type() != ShaderType::VERTEX) ) {
			fprintf(ERRLOG, "\nERROR! Attempting to attach ShaderID 0 as %s vert shader to this program!\n", vert->getFilepath().c_str());
			return;
		}

		glAttachShader(mProgramID, vert->ID());
		mState.mHasVert = true;
		mState.mReadyToLink = checkToSeeIfReadyToLinkAfterAttachingVert();
	}

	void ShaderProgram::attachSecondaryVert(const ShaderInterface::VertexShader * secondaryVert) {
		if (secondaryVert == nullptr) { return; }
		if (!(secondaryVert->readyToBeAttached())) {
			fprintf(ERRLOG, "\nError! Unable to attach secondary Vertex shader \"%s\"\n"
				"to this ShaderProgram because the secondary Vertex shader is not ready\n"
				"to be attached!\n", secondaryVert->getFilepath().c_str());
			return;
		}
		if (mState.mLinked) {
			fprintf(WRNLOG, "\nUnable to attach %s to shaderProgram because ShaderProgram has "
				"already been linked!\n", secondaryVert->getFilepath().c_str());
			return;
		}
		if ( (mState.mHasCompute) || (mState.mAttachedSecondaryComputeCount > 0) ) {
			fprintf(WRNLOG, "\nWarning! Unable to attach secondary Vertex shader \"%s\"\nto this "
				"ShaderProgram object because this ShaderProgram object has a Compute shader\n"
				"attached to it!\n", secondaryVert->getFilepath().c_str());
			return;
		}
		if ( (!secondaryVert->markedAsSecondary()) ) {
			fprintf(ERRLOG, "\nError! Attempting to attach secondaryVert shader \"%s\"\n"
				"as a secondary shader, but this shader was never marked as secondary!\n"
				"Please use the function makeSecondary() on the vertex shader to allow it to\n"
				"be attached as a secondary. Please note that secondary shaders can not contain\n"
				"a 'main()' function!\n", secondaryVert->getFilepath().c_str());
			return;
		}

		//Check to see if this shader has already been attach as a secondary
		if (checkAttachedSecondariesForMatch(secondaryVert->shaderID()) ) {
			fprintf(ERRLOG, "\nERROR! ShaderProgram has determined that the Secondary Vertex Shader with ID %u\n"
				"has already been attached to this ShaderProgram!\n\n", secondaryVert->shaderID().mID);
			return;
		}
		
		//If we made it this far, then it is safe to attach the shader as a secondary
		addShaderToAttachedSecondaries(secondaryVert); //Track the newly added secondary
		glAttachShader(mProgramID, secondaryVert->ID()); //Attach the secondary to the shaderProgram
		mState.mAttachedSecondaryVertCount++; //Increment the number of attached secondary vertex shaders
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
		if ((mState.mHasCompute) || (mState.mAttachedSecondaryComputeCount > 0)) {
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
			fprintf(WRNLOG, "\nUnable to attach %s to shaderProgram because ShaderProgram has already been linked!\n", geom->getFilepath().c_str());
			return;
		}
		if (!(geom->readyToBeAttached())) {
			fprintf(ERRLOG, "\nError attaching geometry shader to program, the geometry shader is not ready to be attached to a program!\n");
			return;
		}
		if (mState.mHasGeom) {
			fprintf(ERRLOG, "\nError attaching geometry shader to program. This program already has a geometry shader!\n");
			return;
		}
		if ((mState.mHasCompute) || (mState.mAttachedSecondaryComputeCount > 0)) {
			fprintf(ERRLOG, "\nError attaching geometry shader to this program. This program has already had\n"
				"a compute shader attached to it, which prevents any further shaders that\n"
				"are not compute shaders from being attached!\n");
			return;
		}
		if (geom->markedAsSecondary()) {
			fprintf(ERRLOG, "\nERROR! Unable to attach the secondary Geometry shader\n"
				"%s as a primary Geometry shader!\nPlease use the 'attachSecondaryGeom' function to attach secondaries.\n",
				geom->getFilepath().c_str());
			return;
		}
		//One last extra check (that probably isnt necessary):
		if ( (geom->ID() == 0u) || (geom->type() != ShaderType::GEOMETRY) ) {
			fprintf(ERRLOG, "\nERROR! Attempting to attach ShaderID 0 as \"%s\" geom shader to this program!\n", geom->getFilepath().c_str());
			return;
		}
		glAttachShader(mProgramID, geom->ID());
		mState.mHasGeom = true;
	}

	void ShaderProgram::attachSecondaryGeom(const ShaderInterface::GeometryShader * secondaryGeom) {
		if (secondaryGeom == nullptr) { return; }
		if (!(secondaryGeom->readyToBeAttached())) {
			fprintf(ERRLOG, "\nError! Unable to attach secondary Geometry shader \"%s\"\n"
				"to this ShaderProgram because the secondary Geometry shader is not ready\n"
				"to be attached!\n", secondaryGeom->getFilepath().c_str());
			return;
		}
		if (mState.mLinked) {
			fprintf(ERRLOG, "\nError! Unable to attach secondary geometry shader \"%s\"\n"
				"to shaderProgram because ShaderProgram has already been "
				"linked!\n", secondaryGeom->getFilepath().c_str());
			return;
		}
		if ((mState.mHasCompute) || (mState.mAttachedSecondaryComputeCount > 0)) {
			fprintf(ERRLOG, "\nError! Unable to attach secondary Geometry shader \"%s\"\nto this "
				"ShaderProgram object because this ShaderProgram object has had a Compute shader\n"
				"attached to it already!\n", secondaryGeom->getFilepath().c_str());
			return;
		}
		if (!secondaryGeom->markedAsSecondary()) {
			fprintf(ERRLOG, "\nError attempting to attach the secondary Geometry shader \"%s\"\n"
				"as a secondary shader, but this shader was never marked as secondary!\n"
				"Please use the function 'makeSecondary()' on the geometry shader to allow it to\n"
				"be attached as a secondary. Please note that secondary shaders can not contain\n"
				"a 'main()' function!\n", secondaryGeom->getFilepath().c_str());
			return;
		}

		//Check to see if this shader has already been attach as a secondary
		if (checkAttachedSecondariesForMatch(secondaryGeom->shaderID())) {
			fprintf(ERRLOG, "\nERROR! ShaderProgram has determined that the Secondary Geometry Shader with ID %u\n"
				"has already been attached to this ShaderProgram!\n\n", secondaryGeom->shaderID().mID);
			return;
		}

		//If we made it this far, then it is safe to attach the shader as a secondary
		glAttachShader(mProgramID, secondaryGeom->ID()); //Attach the secondary to the shaderProgram
		addShaderToAttachedSecondaries(secondaryGeom); //Track the newly added secondary
		mState.mAttachedSecondaryGeomCount++; //Increment the number of attached secondary geometry shaders
	}

	



	void ShaderProgram::attachTess(const ShaderInterface::TessellationControlShader * tessc, const ShaderInterface::TessellationEvaluationShader * tesse) {
		//We can avoid validating each parameter object's state here because each of the following 2 functions
		//will handle their own parameter evaluation.
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
		if ((mState.mHasCompute) || (mState.mAttachedSecondaryComputeCount > 0)) {
			fprintf(WRNLOG, "\nWARNING! Unable to attach Tesselation Evaluation shader \"%s\"\nto this ShaderProgram "
				"because this ShaderProgram already has a Compute attached!\n", tesse);
			return false;
		}
		mTesselationEvaluationShader = std::make_unique<TessellationEvaluationShader>(tesse);
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

	void ShaderProgram::attachTesse(const ShaderInterface::TessellationEvaluationShader * tesse) {
		if (tesse == nullptr) { return; }
		if (mState.mLinked) {
			fprintf(ERRLOG, "\nERROR: Unable to attach Tesselation Evaluation shader %s\n" 
				"to shaderProgram because this ShaderProgram has already been linked!\n", tesse->getFilepath().c_str());
			return;
		}
		if (mState.mHasTesse) {
			fprintf(ERRLOG, "\nERROR: Unable to attach Tesselation Evaluation shader \"%s\" to this ShaderProgram\n"
				"because this ShaderProgram already has a tesse shader attached!\n", tesse->getFilepath().c_str());
			return;
		}
		if ((mState.mHasCompute) || (mState.mAttachedSecondaryComputeCount > 0)) {
			fprintf(ERRLOG, "\nError attaching Tessetlation Evaluation shader %s\n"
				"to this program. This program has already had a compute\n"
				"shader attached to it, which prevents any further shaders that\n"
				"are not compute shaders from being attached!\n", tesse->getFilepath().c_str());
			return;
		}
		if (tesse->markedAsSecondary()) {
			fprintf(ERRLOG, "\nERROR! Unable to attach secondary tesselation evaluation shader\n"
				"%s as a primary Tesselation shader!\nPlease use the 'attachSecondary* function\n",
				tesse->getFilepath().c_str());
			return;
		}
		if ((tesse->ID() == 0) || (tesse->type() != ShaderType::TESSELATION_EVALUATION)) {
			fprintf(ERRLOG, "\nERROR! Attempting to attach ShaderID 0 as \"%s\"\n"
				"Tesselation Evaluation shader to this program!\n", tesse->getFilepath().c_str());
			return;
		}
		glAttachShader(mProgramID, tesse->ID());
		mState.mHasTesse = true;
		mState.mReadyToLink = checkToSeeIfReadyToLinkAfterAttachingTesse();
	}

	void ShaderProgram::attachSecondaryTesse(const ShaderInterface::TessellationEvaluationShader * secondaryTesse) {
		if (secondaryTesse == nullptr) { return; }
		if (!(secondaryTesse->readyToBeAttached())) {
			fprintf(ERRLOG, "\nError! Unable to attach secondary Tesselation Evaluation shader \"%s\"\n"
				"to this ShaderProgram because the secondary Tess Eval shader shows it is not ready\n"
				"to be attached!\n", secondaryTesse->getFilepath().c_str());
			return;
		}
		if (mState.mLinked) {
			fprintf(ERRLOG, "\nError! Unable to attach secondary tesselation evaluation shader \"%s\"\n"
				"to shaderProgram because ShaderProgram has already been "
				"linked!\n", secondaryTesse->getFilepath().c_str());
			return;
		}
		if ((mState.mHasCompute) || (mState.mAttachedSecondaryComputeCount > 0)) {
			fprintf(ERRLOG, "\nError! Unable to attach secondary Tesselation Evaluation shader \"%s\"\nto this "
				"ShaderProgram object because this ShaderProgram object has had a Compute shader\n"
				"attached to it already!\n", secondaryTesse->getFilepath().c_str());
			return;
		}
		if (!secondaryTesse->markedAsSecondary()) {
			fprintf(ERRLOG, "\nError attempting to attach the secondary Tesselation Evaluation shader \"%s\"\n"
				"as a secondary shader, because this shader was never marked as secondary!\n"
				"Please use the function 'makeSecondary()' on this Tesse shader to allow it to\n"
				"be attached as a secondary. Please note that secondary shaders can not contain\n"
				"a 'main()' function!\n", secondaryTesse->getFilepath().c_str());
			return;
		}

		//Check to see if this shader has already been attach as a secondary
		if (checkAttachedSecondariesForMatch(secondaryTesse->shaderID())) {
			fprintf(ERRLOG, "\nERROR! ShaderProgram has determined that the Tesse SecondaryShader with ID %u\n"
				"has already been attached to this ShaderProgram!\n\n", secondaryTesse->shaderID().mID);
			return;
		}

		//If we made it this far, then it is safe to attach the shader as a secondary
		addShaderToAttachedSecondaries(secondaryTesse); //Track the newly added secondary
		glAttachShader(mProgramID, secondaryTesse->ID()); //Attach the secondary to the shaderProgram
		mState.mAttachedSecondaryTesseCount++; //Increment the number of attached secondary tess eval shaders
	}





	bool ShaderProgram::attachTessc(const char * tessc) {
		if (tessc == nullptr) { return false; }
		if (mState.mLinked) {
			fprintf(ERRLOG, "\nERROR: Unable to attach %s to shaderProgram because ShaderProgram has already been linked!\n", tessc);
			return false;
		}
		if (mState.mHasTessc) {
			fprintf(ERRLOG, "\nERROR: Unable to attach Tesselation Control shader \"%s\" to this ShaderProgram\n"
				"because this ShaderProgram already has a tessc shader attached!\n", tessc);
			return false;
		}
		if (mState.mHasCompute) {
			fprintf(ERRLOG, "\nERROR: Unable to attach Tesselation Control shader \"%s\"\nto this ShaderProgram "
				"because this ShaderProgram already has a Compute attached!\n", tessc);
			return false;
		}
		mTesselationControlShader = std::make_unique<TessellationControlShader>(tessc);
		if ((mTesselationControlShader->readyToBeAttached()) && (mTesselationControlShader->type() == ShaderType::TESSELLATION_CONTROL)) {
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
	
	void ShaderProgram::attachTessc(const ShaderInterface::TessellationControlShader * tessc) {
		if (tessc == nullptr) { return; }
		if (mState.mLinked) {
			fprintf(ERRLOG, "\nUnable to attach Tesselation Control shader %s\n"
				"to shaderProgram because this ShaderProgram has already been linked!\n", tessc->getFilepath().c_str());
			return;
		}
		if (mState.mHasTessc) {
			fprintf(ERRLOG, "\nWARNING! Unable to attach Tesselation Control shader \"%s\" to this ShaderProgram\n"
				"because this ShaderProgram already has a tesse shader attached!\n", tessc->getFilepath().c_str());
			return;
		}
		if ((mState.mHasCompute) || (mState.mAttachedSecondaryComputeCount > 0)) {
			fprintf(ERRLOG, "\nError attaching Tessetlation Control shader \"%s\"\n"
				"to this program. This program has already had a compute\n"
				"shader attached to it, which prevents any further shaders that\n"
				"are not compute shaders from being attached!\n", tessc->getFilepath().c_str());
			return;
		}
		if (tessc->markedAsSecondary()) {
			fprintf(ERRLOG, "\nERROR! Unable to attach secondary Tesselation Control shader\n"
				"\"%s\" as a primary Tesselation shader!\nPlease use the 'attachSecondary*()' function\n"
				"or provide a primary shader to this attachTessc function.\n",
				tessc->getFilepath().c_str());
			return;
		}
		if ((!tessc->readyToBeAttached())) {
			fprintf(ERRLOG, "Error! Attaching Tesselation Control shader \"%s\"\n"
				"failed because object is not ready to be attached\n", tessc->getFilepath().c_str());
		}
		if ((tessc->ID() == 0) || (tessc->type() != ShaderType::TESSELLATION_CONTROL)) {
			fprintf(ERRLOG, "\nERROR! Attempting to attach ShaderID 0 as \"%s\"\n"
				"Tesselation Control shader to this program!\n", tessc->getFilepath().c_str());
			return;
		}
		glAttachShader(mProgramID, tessc->ID());
		mState.mHasTessc = true;
		mState.mReadyToLink = checkToSeeIfReadyToLinkAfterAttachingTessc();
	}

	void ShaderProgram::attachSecondaryTessc(const ShaderInterface::TessellationControlShader * secondaryTessc) {
		if (secondaryTessc == nullptr) { return; }
		if (!(secondaryTessc->readyToBeAttached())) {
			fprintf(ERRLOG, "\nError! Unable to attach secondary Tesselation Control shader \"%s\"\n"
				"to this ShaderProgram because the secondary Tess Control shader shows it is not ready\n"
				"to be attached!\n", secondaryTessc->getFilepath().c_str());
			return;
		}
		if (mState.mLinked) {
			fprintf(ERRLOG, "\nError! Unable to attach secondary tesselation control shader \"%s\"\n"
				"to shaderProgram because ShaderProgram has already been "
				"linked!\n", secondaryTessc->getFilepath().c_str());
			return;
		}
		if ((mState.mHasCompute) || (mState.mAttachedSecondaryComputeCount > 0)) {
			fprintf(ERRLOG, "\nError! Unable to attach secondary Tesselation Control shader \"%s\"\nto this "
				"ShaderProgram object because this ShaderProgram object has had a Compute shader\n"
				"attached to it already!\n", secondaryTessc->getFilepath().c_str());
			return;
		}
		if ( !(secondaryTessc->markedAsSecondary()) ) {
			fprintf(ERRLOG, "\nError attempting to attach as secondary the requested Tesselation Control shader \"%s\"\n"
				"as a secondary shader, because this shader was never marked as secondary!\n"
				"Please use the function 'makeSecondary()' on this Tess Control shader to allow it to\n"
				"be attached as a secondary. Please note that secondary shaders can not contain\n"
				"a 'main()' function!\n", secondaryTessc->getFilepath().c_str());
			return;
		}

		//Check to see if this shader has already been attach as a secondary
		if (checkAttachedSecondariesForMatch(secondaryTessc->shaderID())) {
			fprintf(ERRLOG, "\nERROR! ShaderProgram has determined that the requested Tess control Secondary Shader with ID %u\n"
				"has already been attached to this ShaderProgram!\n\n", secondaryTessc->shaderID().mID);
			return;
		}

		//If we made it this far, then it is safe to attach the shader as a secondary
		glAttachShader(mProgramID, secondaryTessc->ID()); //Attach the secondary to the shaderProgram
		addShaderToAttachedSecondaries(secondaryTessc); //Track the newly added secondary
		mState.mAttachedSecondaryTesscCount++; //Increment the number of attached secondary tess control shaders
	}




	bool ShaderProgram::attachFrag(const char * frag) {
		if (frag == nullptr) { return false; }
		if (mState.mLinked) {
			fprintf(ERRLOG, "\nERROR: Unable to attach frag shader \"%s\"\nto this ShaderProgram "
				"because this ShaderProgram has already been linked!\n", frag);
			return false;
		}
		if (mState.mHasFrag) {
			fprintf(ERRLOG, "\nERROR: Unable to attach Fragment shader \"%s\"\nto this ShaderProgram "
				"because this shaderprogram already has a fragment shader attached!\n", frag);
			return false;
		}
		if (mState.mHasCompute) {
			fprintf(ERRLOG, "\nERROR: Unable to attach Fragment shader \"%s\"\nto this "
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
		//fprintf(WRNLOG, "\nWarning! This function to attach an already created primary Fragment shader has not yet been implemented!\n");
		if (frag == nullptr) { return; }
		if (mState.mLinked) {
			fprintf(ERRLOG, "\nERROR: Unable to attach %s to ShaderProgram because this ShaderProgram has already been linked!\n", frag->getFilepath().c_str());
			return;
		}
		if (mState.mHasFrag) {
			fprintf(ERRLOG, "\nError attaching Fragment shader \"%s\"\n"
				"to this ShaderProgram. This program already has a primary Fragment shader!\n",
				frag->getFilepath().c_str());
			return;
		}
		if ((mState.mHasCompute) || (mState.mAttachedSecondaryComputeCount > 0)) {
			fprintf(WRNLOG, "\nWarning! Unable to attach Fragment shader \"%s\"\nto this "
				"ShaderProgram object because this ShaderProgram object has a Compute shader attached to it!\n", frag->getFilepath().c_str());
			return;
		}
		if ( !(frag->readyToBeAttached()) ) {
			fprintf(ERRLOG, "\nError attaching Fragment shader \"%s\"\n"
				"to program, shader is not currently ready to be attached!\n",
				frag->getFilepath().c_str());
			return;
		}
		if (frag->markedAsSecondary()) {
			fprintf(ERRLOG, "\nError! The Fragment shader \"%s\"\n"
				"has been marked as a secondary shader, which prevents it from being\n"
				"able to be attached to the primary Fragment shader spot!\n"
				"Please use the 'attachSecondaryFrag()' function to attach this shader.\n", frag->getFilepath().c_str());
			return;
		}
		//One last extra check (that probably isnt necessary):
		if ((frag->ID() == 0u) || (frag->type() != ShaderType::FRAGMENT)) {
			fprintf(ERRLOG, "\nERROR! Attempting to attach ShaderID 0 as \"%s\"\n"
				"Fragment shader to this program!\n", frag->getFilepath().c_str());
			return;
		}

		glAttachShader(mProgramID, frag->ID());
		mState.mHasFrag = true;
		mState.mReadyToLink = checkToSeeIfReadyToLinkAfterAttachingFrag();
	}

	void ShaderProgram::attachSecondaryFrag(const ShaderInterface::FragmentShader * secondaryFrag) {
		if (secondaryFrag == nullptr) { return; }
		if (!(secondaryFrag->readyToBeAttached())) {
			fprintf(ERRLOG, "\nError! Unable to attach secondary Fragment shader \"%s\"\n"
				"to this ShaderProgram because the secondary Fragment shader is not ready\n"
				"to be attached!\n", secondaryFrag->getFilepath().c_str());
			return;
		}
		if (mState.mLinked) {
			fprintf(ERRLOG, "\nError! Unable to attach secondary fragment shader \"%s\"\n"
				"to shaderProgram because ShaderProgram has already been "
				"linked!\n", secondaryFrag->getFilepath().c_str());
			return;
		}
		if ((mState.mHasCompute) || (mState.mAttachedSecondaryComputeCount > 0)) {
			fprintf(ERRLOG, "\nError! Unable to attach secondary Fragment shader \"%s\"\nto this "
				"ShaderProgram object because this ShaderProgram object has had a Compute shader\n"
				"attached to it already!\n", secondaryFrag->getFilepath().c_str());
			return;
		}
		if ( !(secondaryFrag->markedAsSecondary()) ) {
			fprintf(ERRLOG, "\nError attempting to attach the secondary Fragment shader \"%s\"\n"
				"as a secondary shader, but this shader was never marked as secondary!\n"
				"Please use the function 'makeSecondary()' on this fragment shader to allow it to\n"
				"be attached as a secondary. Please note that secondary shaders can not contain\n"
				"a 'main()' function!\n", secondaryFrag->getFilepath().c_str());
			return;
		}

		//Check to see if this shader has already been attach as a secondary
		if (checkAttachedSecondariesForMatch(secondaryFrag->shaderID())) {
			fprintf(ERRLOG, "\nERROR! ShaderProgram has determined that the Secondary Fragment Shader with ID %u\n"
				"has already been attached to this ShaderProgram!\n\n", secondaryFrag->shaderID().mID);
			return;
		}

		//If we made it this far, then it is safe to attach the shader as a secondary
		glAttachShader(mProgramID, secondaryFrag->ID()); //Attach the secondary to the shaderProgram
		addShaderToAttachedSecondaries(secondaryFrag); //Track the newly added secondary
		mState.mAttachedSecondaryFragCount++; //Increment the number of attached secondary Fragment shaders
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
		//If any shader besides a compute shader has already been attached, then attaching a compute is an illegal operation.
		if ((mState.mHasVert || mState.mHasGeom || mState.mHasTesse || mState.mHasTessc || mState.mHasFrag) ||
			(((mState.mAttachedSecondaryVertCount + mState.mAttachedSecondaryGeomCount + mState.mAttachedSecondaryTesseCount +
				mState.mAttachedSecondaryTesscCount + mState.mAttachedSecondaryFragCount) > 0))) {
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
		if (compute == nullptr) { return; }
		if (mState.mLinked) {
			fprintf(ERRLOG, "\nUnable to attach \"%s\"\n"
				"to this ShaderProgram because this ShaderProgram has already been linked!\n", compute->getFilepath().c_str());
			return;
		}
		if (mState.mHasCompute) {
			fprintf(ERRLOG, "\nError attaching compute shader to program. This program already has a primary compute shader attached!\n");
			return;
		}
		if ((mState.mHasVert || mState.mHasGeom || mState.mHasTesse || mState.mHasTessc || mState.mHasFrag) ||
			(((mState.mAttachedSecondaryVertCount + mState.mAttachedSecondaryGeomCount + mState.mAttachedSecondaryTesseCount +
				mState.mAttachedSecondaryTesscCount + mState.mAttachedSecondaryFragCount) > 0))) {
			fprintf(WRNLOG, "\nWarning! Unable to attach Fragment shader \"%s\"\nto this "
				"ShaderProgram object because this ShaderProgram object has a non-Compute shader attached to it!\n", compute->getFilepath().c_str());
			return;
		}
		if ( !(compute->readyToBeAttached()) ) {
			fprintf(ERRLOG, "\nError attaching Compute shader \"%s\"\n"
				"to program, the compute shader is not currently ready to be attached!\n", compute->getFilepath().c_str());
			return;
		}
		if (compute->markedAsSecondary()) {
			fprintf(ERRLOG, "\nError! The compute shader \"%s\"\n"
				"has been marked as a secondary shader, which prevents it from being\n"
				"able to be attached to the primary compute shader spot!\n"
				"Please use the 'attachSecondaryCompute()' function to attach this shader.\n",
				compute->getFilepath().c_str());
			return;
		}
		//One last extra check (that probably isnt necessary):
		if ((compute->ID() == 0u) || (compute->type() != ShaderType::COMPUTE)) {
			fprintf(ERRLOG, "\nERROR! Attempting to attach ShaderID 0 as \"%s\"\n"
				"Compute shader to this program!\n", compute->getFilepath().c_str());
			return;
		}

		glAttachShader(mProgramID, compute->ID());
		mState.mHasVert = true;
		mState.mReadyToLink = true;
	}

	void ShaderProgram::attachSecondaryCompute(const ShaderInterface::ComputeShader * secondaryCompute) {
		if (secondaryCompute == nullptr) { return; }
		if (!(secondaryCompute->readyToBeAttached())) {
			fprintf(ERRLOG, "\nError! Unable to attach secondary Compute shader \"%s\"\n"
				"to this ShaderProgram because the secondary Compute shader is not ready\n"
				"to be attached!\n", secondaryCompute->getFilepath().c_str());
			return;
		}
		if (mState.mLinked) {
			fprintf(ERRLOG, "\nError! Unable to attach secondary Compute shader \"%s\"\n"
				"to shaderProgram because ShaderProgram has already been "
				"linked!\n", secondaryCompute->getFilepath().c_str());
			return;
		}
		if ((mState.mHasVert || mState.mHasGeom || mState.mHasTesse || mState.mHasTessc || mState.mHasFrag) ||
			(((mState.mAttachedSecondaryVertCount + mState.mAttachedSecondaryGeomCount + mState.mAttachedSecondaryTesseCount +
				mState.mAttachedSecondaryTesscCount + mState.mAttachedSecondaryFragCount) > 0))) {
			fprintf(ERRLOG, "\nError! Unable to attach secondary Compute shader \"%s\"\n"
				"to this ShaderProgram because this ShaderProgram has already had\n"
				"a shader of a different type attached!\n", mComputeShader->getFilepath().c_str());
			return;
		}
		if ( (!secondaryCompute->markedAsSecondary()) ) {
			fprintf(ERRLOG, "\nError attempting to attach the secondary Compute shader \"%s\"\n"
				"as a secondary shader, but this shader was never marked as secondary!\n"
				"Please use the function 'makeSecondary()' on this compute shader to allow it to\n"
				"be attached as a secondary. Please note that secondary shaders can not contain\n"
				"a 'main()' function!\n", secondaryCompute->getFilepath().c_str());
			return;
		}

		//Check to see if this shader has already been attach as a secondary
		if (checkAttachedSecondariesForMatch(secondaryCompute->shaderID())) {
			fprintf(ERRLOG, "\nERROR! ShaderProgram has determined that the Secondary Compute Shader with ID %u\n"
				"has already been attached to this ShaderProgram!\n\n", secondaryCompute->shaderID().mID);
			return;
		}

		//If we made it this far, then it is safe to attach the shader as a secondary
		addShaderToAttachedSecondaries(secondaryCompute); //Track the newly added secondary
		glAttachShader(mProgramID, secondaryCompute->ID()); //Attach the secondary to the shaderProgram
		mState.mAttachedSecondaryComputeCount++; //Increment the number of attached secondary compute shaders
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
					fprintf(WRNLOG, "Shader program is unable to link because program has both a Compute and a Tessellation Control shader attached!\n");
				}
				if (mState.mHasTesse) {
					fprintf(WRNLOG, "Shader program is unable to link because program has both a Compute and a Tessellation Evaluation shader attached!\n");
				}
				if (mState.mHasFrag) {
					fprintf(WRNLOG, "Shader program is unable to link because program has both a Compute and a Fragment shader attached!\n");
				}
			}
			else {
				if (mState.mHasTessc && !mState.mHasTesse) {
					fprintf(WRNLOG, "Shader program is unable to link because a Tessellation Control is attached but no Tessellation Evaluation is attached!\n");
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

				glGetProgramInfoLog(mProgramID, 768u, NULL, infoLog);
				fprintf(ERRLOG, "\nERROR OCCURED WHILE LINKING SHADERPROGRAM!\nError log:\n\t%s\n", infoLog);
				return;
			}

			mState.mReadyToLink = false;
			mState.mLinked = true;
			mState.mValid = true;
		}
	}
	
	void ShaderProgram::use() const {
		if (mState.mReleased) { //Make sure the program hasn't been deleted from the graphics context
			fprintf(ERRLOG, "\nERROR using shader program! This shader program has been deleted "
				"from the graphics context!\n");
			return;
		}
		else if (mState.mValid) { //If the program is valid, go ahead and use it
			glUseProgram(mProgramID);
			return;
		}
		else { //Else figure out what is wrong and print out a message
			if (mState.mError) {
				fprintf(ERRLOG, "\nError! Unable to use shader program because of an error encountered while configuring the program!\n");
				glUseProgram(0);
				return;
			}
			if (!mState.mLinked) {
				fprintf(ERRLOG, "\nError! Unable to use ShaderProgram because this program has not yet been linked!\n");
				glUseProgram(0);
				return;
			}
			/*if (!mState.mValid) {
				fprintf(ERRLOG, "\nError! Unable to use shader program because the ShaderProgram is not yet valid!\n");
				glUseProgram(0);
				return;
			}*/
		}
	}


	void ShaderProgram::generateGLProgramHandle() {
		mProgramID = glCreateProgram();
	}

	void ShaderProgram::initializeUniformLocationTracker() {
		uniforms.activateUniformLocationTracker(mProgramID);
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

	bool ShaderProgram::checkAttachedSecondariesForMatch(const ShaderID& secondaryShaderID) const {
		auto search = mState.mAttachedSecondaries.find(secondaryShaderID.mID);
		if (search != mState.mAttachedSecondaries.end()) 
			return true;
		else
			return false;
	}

	//Passing a nullptr to this function will cause a whole lot of problems
	void ShaderProgram::addShaderToAttachedSecondaries(const CompiledShader* secondaryShader) {
		mState.mAttachedSecondaries.emplace(secondaryShader->ID(), secondaryShader->getFilepath());
	}

	

//} //namespace ShaderInterface


