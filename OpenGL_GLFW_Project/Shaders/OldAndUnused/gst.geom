
//Based off GLSL cookbook page 222

#version 450 core


layout ( points ) in;
layout ( triangle_strip, max_vertices = 5 ) out;

in vec3 pos1[];
in vec3 Color1[];

out vec3 pos;
out vec3 Color;


void main() {
	 pos = pos1[0];
	 Color = Color1[0];

	 //Origin
	gl_Position = vec4(0.1, -0.32, 0.0, 0.0) + gl_in[0].gl_Position;
	EmitVertex();

	gl_Position = vec4(0.1, 0.2, 0.0, 0.0) + gl_in[0].gl_Position;
	EmitVertex();

	gl_Position = vec4(0.0, 0.4, 0.0, 0.0) + gl_in[0].gl_Position;
	EmitVertex();
	
	gl_Position = vec4(-0.2, -0.0, 0.0, 0.0) + gl_in[0].gl_Position;
	EmitVertex();

	gl_Position = vec4(-0.1, -0.32, 0.0, 0.0) + gl_in[0].gl_Position;
	EmitVertex();

	EndPrimitive();
}



/*
layout ( points ) in;
layout ( triangle_strip, max_vertices = 4 ) out;

in vec3 pos1[];
in vec3 Color1[];

out vec3 pos;
out vec3 Color;


void main() {
	 pos = pos1[0];
	 Color = Color1[0];

	gl_Position = vec4(-0.0995, -0.025, 0.0, 0.0) + gl_in[0].gl_Position;
	EmitVertex();

	gl_Position = vec4(-0.225, 0.325, 0.0, 0.0) + gl_in[0].gl_Position;
	EmitVertex();

	gl_Position = vec4(0.025, -0.025, 0.0, 0.0) + gl_in[0].gl_Position;
	EmitVertex();
	
	gl_Position = vec4(0.025, 0.925, 0.0, 0.0) + gl_in[0].gl_Position;
	EmitVertex();

	EndPrimitive();
}

*/