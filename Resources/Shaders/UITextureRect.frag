#version 440 core

layout(location = 0) in vec2 i_TextureCoords;

uniform sampler2D u_Texture;

layout(location = 0) out vec4 o_Color;

void main()
{
    o_Color = texture(u_Texture, i_TextureCoords);
}
