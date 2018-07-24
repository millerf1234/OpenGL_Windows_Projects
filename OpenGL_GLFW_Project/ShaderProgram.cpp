#include "ShaderProgram.h"


namespace ShaderInterface {


	ShaderProgram::ShaderProgram() {
		mID = glCreateProgram();
		uniformLocations = UniformLocationTracker(mID);

	}


	ShaderProgram::~ShaderProgram() {

	}


} //namespace ShaderInterface