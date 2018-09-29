#version 450 core

in vec3 pos;
in vec3 Color;

out vec4 FragColor;

void main() {
	vec3 clampedPos = vec3(max(pos.x, 0.0), max(pos.y, 0.0), max(pos.z, 0.0));
	FragColor = vec4(Color + clampedPos, 1.0);

}

