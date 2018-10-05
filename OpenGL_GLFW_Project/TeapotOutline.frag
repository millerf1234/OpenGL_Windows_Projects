//This GLSL shader is intended to be used with TeapotExplosion.vert and 
//TeapotOutline.geom. This file determines how the teapot outline will be colored.

#version 450 core

in vec3 g2f_pos;  //Position data from geomtry shader
in vec3 g2f_vel; //Velocity data from geometry shader

out vec4 FragColor;


uniform float time;

void main() {

	FragColor = vec4(g2f_pos + g2f_vel, 1.0);
}