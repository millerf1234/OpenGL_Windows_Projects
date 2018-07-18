#version 450 core

layout(location = 0) in vec3 xyzCoords;

uniform float zoom;

void main() {
	gl_Position = vec4(xyzCoords, max(1.0, zoom);
}