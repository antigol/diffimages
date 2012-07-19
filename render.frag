#version 130

in vec2 coord;

uniform sampler2D texture0;
uniform float cmend;
uniform float cmbeg;

out vec4 color;

void main(void)
{
    float t0 = texture2D(texture0, coord).r;

    if (cmend != cmbeg)
        t0 = (t0 - cmbeg) / (cmend - cmbeg);
    else
        t0 = 0.0;

    vec3 c;

    if (t0 < 0.0) {
        c = vec3(0,0,0);
    } else if (t0 < 0.5) {
        float f = (t0 - 0.0) / 0.5;
        c = vec3(0,0,0) * (1.0 - f) + vec3(1,0.8,0) * f;
    } else if (t0 < 1.0) {
        float f = (t0 - 0.5) / 0.5;
        c = vec3(1,0.8,0) * (1.0 - f) + vec3(1,1,1) * f;
    } else {
        c = vec3(1,1,1);
    }

    color = vec4(c, 1.0);
}
