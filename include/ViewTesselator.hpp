#ifndef GVIEW_VIEWTESSELATOR
#define GVIEW_VIEWTESSELATOR

#include <Frontier.hpp>
using namespace std;

#include "View.hpp"
#include "LogLevel.hpp"

class ViewTesselator
{
public:
    ViewTesselator(const View &view,Delegate<void,LogLevel,string> logfunc);
    
    void tess();
    void tessNodes();
    void tessEdges();
    
    Mesh line_mesh;
    Mesh tris_mesh;
    mat4 transf;
    vec2 wsize;
    float zoom;
    float spacing;
    float arrowSize;
    size_t active;

private:
    const View &view;
    Delegate<void,LogLevel,string> m_logFunc;
    
    void appendNodeMesh(vec2 p,float r,vec4 outline,vec4 fill,NodeShape shape,vec2 up = vec2(0,1));
    void appendEdgeMesh(vec2 A,vec2 B,float w,vec4 outline,vec4 fill,NodeShape beg,NodeShape end);
};

#endif