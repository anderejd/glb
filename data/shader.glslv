#version 150

in  vec3 i_vpos;
in  vec3 i_vcol;
out vec3 o_vcol;

void main (void)
{
    gl_Position = vec4 (i_vpos, 1.0);
    o_vcol = i_vcol;
}

