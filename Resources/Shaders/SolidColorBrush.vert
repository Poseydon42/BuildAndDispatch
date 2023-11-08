#version 440

layout(location = 0) in vec2 i_Position;

void main()
{
    // NOTE: the coordinates we get from the host are in [0.0, 1.0] range, OpenGL's NDC however is [-1.0, 1.0]
    vec2 Position = i_Position * 2.0 - 1.0;
    gl_Position = vec4(Position, 0.0, 1.0);
}

