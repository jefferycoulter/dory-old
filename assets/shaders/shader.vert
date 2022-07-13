#version 450

layout(location = 0) in vec2 a_position;

layout(push_constant) uniform Push
{
    vec2 offset;
    vec3 color;
} push;

void main() {
    gl_Position = vec4(a_position + push.offset, 0.0, 1.0);
}