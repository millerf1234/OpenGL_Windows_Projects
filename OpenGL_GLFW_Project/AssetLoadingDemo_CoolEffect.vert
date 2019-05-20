//While playing around in the asset loading demo, I came across this cool effect. 
// To recreate this effect, the MVP matrix should be computed by the following steps:
//
//    Compute the model matrix:
//            [Model is just a 4x4 rotation matrix]
//    
//    Compute the view matrix:
//        glm::vec3 cameraPos(0.0f, 0.0f, 5.0f);
//        glm::vec3 lookAtOrgin(0.0f, 0.0f, -1.0f);
//        glm::vec3 upDirection(0.0f, 1.0f, 0.0f);
//        
//        glm::mat4 view = glm::lookAt(cameraPos, lookAtOrgin, upDirection);
//
//    Compute the Perspective Matrix:
//        float fov = 56.0f;
//        float screenWidth = 3840.0f;
//        float screenHeight = 2160.0f;
//        float zNear = 0.5f;
//        float zFar = 100.0f;
//
//        glm::mat4 perspective = perspective = glm::perspectiveFov(fov, screenWidth, screenHeight, zNear, zFar);
//
//   Putting the three together looks like:
// 
//        glm::mat4 MVP; //Model-View-Projection matrix 
//        MVP = perspective * (view * (rotation));
//        glm::mat4 userTranslation = glm::mat4(1.0f, 0.0f, 0.0f, 0.0f,             //Translation from user input
//                                           	0.0f, 1.0f, 0.0f, 0.0f,
//                                             	0.0f, 0.0f, 1.0f, 0.0f,
//                                             	xTranslation, yTranslation, 0.0f, 1.0f);
//        MVP *= userTranslation;
//
//
// Forrest Miller
// December 6, 2018


#version 450 core

layout(location = 0) in vec4 ModelPosition;
layout(location = 1) in vec2 ModelTexCoord;
layout(location = 2) in vec3 ModelNormal;

out vec4 position;
out vec2 texCoord;
out vec3 normal;

uniform float zoom;
uniform float time;
uniform mat4 rotation;

uniform mat4 MVP;   


#define vert float(gl_VertexID)
#define inst float(gl_InstanceID)


void main() {
	//Do the cool effect on each vertices position
	float cool = (sin(2.14*(time - (0.001*vert))));
	cool *= step(0.25, abs(cool)); //Cut off asymptotic behavior as 'cool' gets close to 0
	position = ModelPosition + vec4(cos(inst + cool), 1.0 / cool, (3.0*inst) / cool, zoom);

	//Texture Coordinates are just passed straight through unmodified
	texCoord = ModelTexCoord;

	//The normals are rotated to align with the model and then passed through
	normal = mat3(rotation) * ModelNormal;


	gl_Position = (MVP * position) + vec4(-2.0, -2.0, 2.0, 0.0); //Perform an optional translation at the end
}