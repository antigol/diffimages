#version 130

in vec2 coord;

uniform sampler2D texture0;

uniform float mul;
uniform float add;

out vec4 color;

float grey(vec3 v)
{
    return (11.0 * v.r + 16.0 * v.g + 5.0 * v.b) / 32.0;
}

void main(void)
{
    vec3 v0 = texture2D(texture0, coord).rgb;

    float t0 = grey(v0);

    float t0k = t0 * mul + add;

    color = vec4(t0k, t0k, t0k, 1.0);
}
