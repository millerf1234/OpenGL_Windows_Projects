//Fragment shader for the asset loading demo
//
// Programmer:  Forrest Miller
// Date:        November 28, 2018

#version 450 core


in VERTEX_SHADER_OUTPUT {  
    float vertID;
    float vertIDMod;
    float instanceID;
    vec4 position;
    vec2 texCoord;
    vec3 normal;
} processed_vertex;

out vec4 color;

uniform sampler2D tex_object;
uniform float time;
uniform mat4 rotation;

uniform uint customParameter1, customParameter2, customParameter3;
const uint lightingVersion = customParameter1 % 3U;



void main() {
    color = vec4(0.1, 0.1, 0.6, 1.0) + texture(tex_object, processed_vertex.texCoord);
}
