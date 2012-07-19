#version 130

in vec2 coord;

uniform sampler2D texture0;
uniform vec2 dPixel; // delta pixel

out float color;

// Savitzky-Golay Filter
// c.f. : http://www.statistics4u.info/fundstat_eng/cc_filter_savgolay.html

float sgfilter(vec2 delta)
{
    float tm3 = texture2D(texture0, max(coord - 3.0 * delta, 0.0)).r;
    float tm2 = texture2D(texture0, max(coord - 2.0 * delta, 0.0)).r;
    float tm1 = texture2D(texture0, max(coord - 1.0 * delta, 0.0)).r;
    float t0 = texture2D(texture0, coord).r;
    float tp1 = texture2D(texture0, min(coord + 1.0 * delta, 1.0)).r;
    float tp2 = texture2D(texture0, min(coord + 2.0 * delta, 1.0)).r;
    float tp3 = texture2D(texture0, min(coord + 3.0 * delta, 1.0)).r;

    return (-2.0 * tm3 + 3.0 * tm2 + 6.0 * tm1 + 7.0 * t0 + 6.0 * tp1 + 3.0 * tp2 - 2.0 * tp3) / 21.0;
}

void main(void)
{
    float sgx = sgfilter(vec2(dPixel.x, 0.0));
    float sgy = sgfilter(vec2(0.0, dPixel.y));

    color = (sgx + sgy) / 2.0;
}
