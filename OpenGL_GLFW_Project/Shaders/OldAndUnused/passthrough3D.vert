#version 450 core

layout(location = 0) in vec3 xyzCoords;

uniform float zoom;

//Use an output interface block  (see OpenGL Superbible 7e page 31)
out VS_Out{
	vec2 data;
} VS_out;

void main() {
	gl_Position = vec4(xyzCoords, max(1.0, zoom);
}