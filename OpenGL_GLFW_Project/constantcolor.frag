#version 450 core

uniform float zoom;

out vec4 color;

void main() {
	color = vec4(0.751 - abs(0.22 * cos(zoom)),
				 0.852 - max(zoom, 0.645),
		         0.796,
		         1.0);
}