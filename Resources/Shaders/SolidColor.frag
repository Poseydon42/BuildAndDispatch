#version 440

layout(location = 0) uniform vec4 u_Color;

layout(location = 0) out vec4 o_Color;

void main()
{
    o_Color = u_Color;
}

