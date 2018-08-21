#version 450 core

uniform float zoom;

out vec4 color;

//Use an output interface block  (see OpenGL Superbible 7e page 31)
in VS_Out{
	vec2 data;
} FS_in;

void main() {
	color = vec4(0.751 - abs(0.22 * cos(zoom)),
				 0.852 - max(zoom, 0.645),
		         0.796 + sin(FS_in.data.x) + cos(FS_in.data.y),
		         1.0);
}