#version 130

in vec4 color_vert;

uniform sampler2D texture0;
uniform float cmend;
uniform float cmbeg;

out vec4 color;

void main(void)
{
    color = color_vert;
}
