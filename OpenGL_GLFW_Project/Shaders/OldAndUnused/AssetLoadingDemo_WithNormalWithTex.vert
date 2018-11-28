//File:           AssetLoadingDemo_WithNormalWithTex.vert
//Description:    Performs basic vertex processing, relying on uniforms 
//                passed in from the Application to dictate the per-vertex
//                operations. This version of the shader expects for there to
//                be 4 position, 2 texture and 3 normal components per each \
//                vertex.
//
// Programmer:    Forrest Miller
// Date:          November 6, 2018
//
// Reference:     http://www.opengl-tutorial.org/beginners-tutorials/tutorial-3-matrices/
//                https://www.cs.utexas.edu/users/fussell/courses/cs354/lectures/lecture11.pdf
//                Textbook: "Real Time Rendering 4e" by Akenine-Moller, Haines, Hoffman, Pesce, Iwanicki, and Hillaire.
//                   Chapter 4 Pages 57-75 (with emphasis on p.68-69 for transforming normals)
//                   and 92-102 (which cover projections)

#version 450 core

#pragma debug(on)  //Comment this line out when not debugging


////////////////// INPUT //////////////////////
layout(location = 0) in vec4 position;
layout(location = 1) in vec2 tex;
layout(location = 2) in vec3 normal;


///////////////// OUTPUT //////////////////////
out vec3 vertPos;
out vec2 texCoord;
out vec3 vertNormal;


//////////////// UNIFORMS /////////////////////
uniform float zoom;
uniform float time;
uniform mat4 ModelViewProjection; //Model-View-Projection matrix as calculated by app
uniform mat3 NormalModelView;     //This matrix should contain a reciprical scaling factor to counter any scaling performed on the model
//uniform mat4 normalModelViewProjection; //Hmm not sure about this one tho...


void main() {
	//Pass through the texture coords with no change 
	texCoord = tex;

	//Compute the position 
	vec4 projectedPosition = (ModelViewProjection * position) + vec4(0.0, 0.0, 0.0, zoom);
	//Write the computed position to the appropriate places
	vertPos = projectedPosition;
	gl_Position = projectedPosition;

	
	//Compute the normal
	vertNormal = NormalModelView * normal;
}
