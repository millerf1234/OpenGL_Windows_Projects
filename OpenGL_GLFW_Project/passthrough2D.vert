#version 450 core

layout(location = 0) in vec2 xyCoords;

uniform float zoom;

void main() {
	gl_Position = vec4(xyCoords, 0.1f, max(1.0, zoom));
}