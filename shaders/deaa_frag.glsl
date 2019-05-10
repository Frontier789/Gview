#version 130

uniform float u_antia;

in vec4 va_color;
in vec4 va_distf;

out vec4 out_clr;

void main()
{
    float d = min(min(va_distf.x,va_distf.y),min(va_distf.z,va_distf.w));
    
    d = min(d/u_antia,1);
    
    out_clr = vec4(va_color.rgb,va_color.a * pow(d,2));
}