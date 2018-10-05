//This geometry shader is intended to be used in conjunction with
//the TeaponExplosion.vert and TeapotOutline.frag GLSL code bits
//

#version 450 core

layout ( triangles, invocations = 1) in; 
layout ( line_strip, max_vertices = 8) out;

in vec3 v2g_position[];

out vec3 g2f_pos;
out vec3 g2f_vel;

uniform int level;
uniform float gravity;
uniform float time;
uniform float velocityScale;
uniform float zoom;

uniform float yRotation;


void main() {
	//Calculate adjustedZoom in the same way it is calculated in the TeapotExplosion geom shader
	float adjustedZoom = zoom + 65.0 * time * (0.5 * time);
	adjustedZoom *= 01.25;

	vec3 V0 = gl_in[0].gl_Position.xyz;
	vec3 V1 = gl_in[1].gl_Position.xyz; 
	vec3 V2 = gl_in[2].gl_Position.xyz;

	vec3 cg = (V0 + V1 + V2) / 3.0f;

	//Time to outline the triangle
	g2f_pos = vec3(0.0, 0.0, 0.0);
	g2f_vel = vec3(0.65, 0.8, 1.0);
	
	vec3 someNoise = noise3(cg);

	gl_Position = vec4(V0, adjustedZoom);
	EmitVertex();
	
	gl_Position = vec4(V1, adjustedZoom);
	EmitVertex(); 

	gl_Position = vec4(V2, adjustedZoom);
	EmitVertex();

	gl_Position=vec4(V0, adjustedZoom);
	EmitVertex();

	//Now calculate a good length for a z-direction 
	float zScale = (length(V0-V1) + length(V0-V2) + length(V2-V1)) / 3.0;

	gl_Position = vec4(cg + zScale * normalize(cross(V0, V1)) + someNoise, adjustedZoom);
	EmitVertex();

	gl_Position = vec4(V1, adjustedZoom);
	EmitVertex(); 

	gl_Position = vec4(cg + zScale * normalize(cross(V0, V1)) - someNoise, adjustedZoom);
	EmitVertex();

	gl_Position = vec4(V2, adjustedZoom);
	EmitVertex();



	EndPrimitive();


	//g2f_pos = vec3(0.9, 0.7, 0.35);
	//g2f_vel = vec3(0.65, 0.8, 1.0);




}
