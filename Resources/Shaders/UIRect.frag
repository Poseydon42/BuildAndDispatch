#version 440 core

layout(location = 0) in vec2 i_TextureCoordinates;

uniform vec4 u_Color;
uniform vec4 u_BorderColor;
uniform vec2 u_FramebufferDimensions;
uniform vec2 u_RectDimensions;
uniform float u_CornerRadius;
uniform float u_BorderThickness;

layout(location = 0) out vec4 o_Color;

float RectangleSDF(vec2 Location, vec2 HalfSize, float CornerRadius)
{
    vec2 A = abs(Location) - (HalfSize - CornerRadius);
    return length(max(A, 0.0)) + min(max(A.x, A.y), 0.0) - CornerRadius;
}

void main()
{
    vec2 HalfRectDimensions = u_RectDimensions / 2.0;
    vec2 FragmentLocation = (i_TextureCoordinates * 2.0 - 1.0) * HalfRectDimensions;
    float SDF = RectangleSDF(FragmentLocation, HalfRectDimensions, u_CornerRadius);

    float IsBorder = step(-u_BorderThickness, SDF);
    o_Color = mix(u_Color, u_BorderColor, IsBorder) * step(0.0, -SDF);
}
