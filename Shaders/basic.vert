#version 450
#include "common.glsl"

vec2 hardcoded_positions[3] = vec2[](
    vec2(0.0, -0.5),
    vec2(0.5, 0.5),
    vec2(-0.5, 0.5)
);

vec4 hardcoded_colors[3] =
    vec4[](vec4(1.0f, 0.0f, 0.0f, 1.0f), vec4(0.0f, 1.0f, 0.0f, 1.0f),
           vec4(0.0f, 0.0f, 1.0f, 1.0f));

layout(location = 0) out vec4 vertex_color;

void main() {
    vec2 current_position = hardcoded_positions[gl_VertexIndex];
    gl_Position = vec4(current_position, 0.0 , 1.0);
    vertex_color = hardcoded_colors[gl_VertexIndex];
}