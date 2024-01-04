#version 440

layout(location = 0) in vec2 i_Position;

void main() 
{
    gl_Position = vec4(i_Position * 2.0 - 1.0, 0.0, 1.0);	
}
