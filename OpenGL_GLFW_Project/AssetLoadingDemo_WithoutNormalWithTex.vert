//File:           AssetLoadingDemo_WithoutNormalWithTex.vert
//Description:    Performs basic vertex processing, relying on uniforms 
//                passed in from the Application to dictate the per-vertex
//                operations. This version of the shader expects for there to
//                be 4 position and 2 texture components per each vertex.
//
// Programmer:    Forrest Miller
// Date:          November 6, 2018


#version 450 core

#pragma debug(on)  //Comment this line out when not debugging


////////////////// INPUT //////////////////////
layout(location = 0) in vec4 position;
layout(location = 1) in vec2 tex;


///////////////// OUTPUT //////////////////////
out vec3 vertPos;
out vec2 texCoord;


//////////////// UNIFORMS /////////////////////
uniform float zoom;
uniform float time;
uniform mat4 ModelViewProjection; //Model-View-Projection matrix as calculated by app



void main() {
	//Pass through the texture coords with no change (for now...)
	texCoord = tex;

	//Compute the position 
	vec4 projectedPosition = (ModelViewProjection * position) + vec4(0.0, 0.0, 0.0, zoom);
	//Write the computed position to the appropriate places
	vertPos = projectedPosition;
	gl_Position = projectedPosition;

}