#version 130

in vec2 coord;

uniform sampler2D texture0;
uniform sampler2D texture1;

uniform float backgroundFactor; // facteur de proportionalité pour accorder la luminosité entre la texture0 et la texture1

out float color;

float grey(vec3 v)
{
    return (11.0 * v.r + 16.0 * v.g + 5.0 * v.b) / 32.0;
}

void main(void)
{
    vec3 v0 = texture2D(texture0, coord).rgb;
    vec3 v1 = texture2D(texture1, coord).rgb;
    float t0 = grey(v0);
    float t1 = grey(v1);

    float t0k = t0 * backgroundFactor;
    color = t0k - t1;
    return;
}
