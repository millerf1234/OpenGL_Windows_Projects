//File:           AssetLoadingDemo_WithoutNormalWithoutTex.vert
//Description:    Performs basic vertex processing, relying on uniforms 
//                passed in from the Application to dictate the per-vertex
//                operations. This version of the shader expects for there to
//                be 4 position components per each vertex.
//
// Programmer:    Forrest Miller
// Date:          November 6, 2018


#version 450 core

#pragma debug(on)  //Comment this line out when not debugging


////////////////// INPUT //////////////////////
layout(location = 0) in vec4 position;


///////////////// OUTPUT //////////////////////
out vec3 vertPos;


//////////////// UNIFORMS /////////////////////
uniform float zoom;
uniform float time;
uniform mat4 ModelViewProjection; //Model-View-Projection matrix as calculated by app


void main() {
	//Compute the position 
	vec4 projectedPosition = (ModelViewProjection * position) + vec4(0.0, 0.0, 0.0, zoom);
	//Write the computed position to the appropriate places
	vertPos = projectedPosition;
	gl_Position = projectedPosition;

}


