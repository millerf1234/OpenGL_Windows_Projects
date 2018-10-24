//This GLSL shader is intended to be used with TeapotExplosion.vert and 
//TeapotOutline.geom. This file determines how the teapot outline will be colored.

#version 450 core

in vec3 vertPos; //Vertex position from vertex shader
in vec3 vertNormal; //Vertex normal from the vertex shader

out vec4 FragColor;


uniform float time;

void main() {

	FragColor = vec4(0.5, 0.5, 0.5, 1.0);
}