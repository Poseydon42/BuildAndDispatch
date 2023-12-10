#version 440 core

layout(location = 0) in vec2 i_Position;
layout(location = 1) in vec2 i_TextureCoordinates;

layout(location = 0) out vec2 o_TextureCoordinates;

void main()
{
    o_TextureCoordinates = i_TextureCoordinates;
    gl_Position = vec4(i_Position * 2.0 - 1.0, 0.0, 1.0);
}

