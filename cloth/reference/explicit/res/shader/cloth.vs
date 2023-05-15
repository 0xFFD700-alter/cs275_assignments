#version 430 core

// You may need some other layouts.
layout (location = 0) in vec3 Pos;
layout (location = 1) in vec3 Normal;

uniform mat4 Projection;
uniform mat4 View;
// uniform mat4 model;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
} vs_out;

void main() {
    vs_out.FragPos = Pos;
    vs_out.Normal = Normal;

    gl_Position =  Projection * View * vec4(Pos, 1.0);
}