#version 150

precision highp float;

in  vec3 o_vcol;
out vec4 o_fcol;

void main (void)
{
    o_fcol = vec4 (o_vcol, 1.0);
}

