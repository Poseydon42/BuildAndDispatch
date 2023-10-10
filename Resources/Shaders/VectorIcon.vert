#version 440

layout(location = 0) in vec2 i_Position;
layout(location = 1) in vec3 i_Color;

layout(location = 0) out vec3 o_Color;

layout(location = 1) uniform mat4 u_MVP;

void main()
{
    o_Color = i_Color;

    gl_Position = u_MVP * vec4(i_Position, 0.0, 1.0);
}

