#version 450

vec2 hardcoded_positions[3] = vec2[](
    vec2(0.0, -0.5),
    vec2(0.5, 0.5),
    vec2(-0.5, 0.5)
);

void main() {
    vec2 current_position = hardcoded_positions[gl_VertexIndex];
    gl_Position = vec4(current_position, 0.0 , 1.0);

}