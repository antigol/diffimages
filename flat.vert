#version 130

in vec2 vertex;
in vec2 texcoord;

out vec2 coord;

void main(void)
{
    gl_Position = vec4(vertex, 0.0, 1.0);
    coord = texcoord;
}
