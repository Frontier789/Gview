#version 130

#define FRONTIER_MVP
#define FRONTIER_POS
#define FRONTIER_CLR
#define FRONTIER_CUSTOM0
#define FRONTIER_CLRMAT

uniform mat4 FRONTIER_MVP    u_mvpMat;
uniform mat4 FRONTIER_CLRMAT u_colorMat;

in vec3 FRONTIER_POS     in_pos;
in vec4 FRONTIER_CUSTOM0 in_distf;
in vec4 FRONTIER_CLR     in_color;

out vec4 va_color;
out vec4 va_distf;

void main()
{
    gl_Position = u_mvpMat * vec4(in_pos,1.0);
    
    va_color = u_colorMat * in_color;
    va_distf = in_distf;
}