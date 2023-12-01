#version 440

layout(location = 0) in vec2 i_TextureCoordinates;

layout(location = 0) uniform sampler2D u_Atlas;
layout(location = 1) uniform vec3 u_Color;
layout(location = 2) uniform float u_ScreenPxRange;

layout(location = 0) out vec4 o_Color;

float Median(float R, float G, float B)
{
    return max(min(R, G), min(max(R, G), B));
}

void main()
{
    vec3 MSD = textureLod(u_Atlas, vec2(i_TextureCoordinates.x, 1.0 - i_TextureCoordinates.y), 0.0).rgb;
    float SD = Median(MSD.r, MSD.g, MSD.b);
    
    float ScreenPxDistance = u_ScreenPxRange * (SD - 0.5);

    float Opacity = clamp(ScreenPxDistance + 0.5, 0.0, 1.0);

    vec4 ForegroundColor = vec4(u_Color, 1.0);
    vec4 BackgroundColor = vec4(0.0);
    o_Color = mix(BackgroundColor, ForegroundColor, Opacity);
}

