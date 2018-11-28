#version 450 core

layout(location = 0) in vec2 xyCoords;

uniform float zoom;

//Use an output interface block  (see OpenGL Superbible 7e page 31)
out VS_Out{
	vec2 data;
} VS_out;


void main() {
	gl_Position = vec4(xyCoords, 0.1f, max(1.0, zoom));
	VS_out.data = vec2(max(zoom, 1.0), min(zoom, -1.0));
}