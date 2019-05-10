#include "ViewTesselator.hpp"

ViewTesselator::ViewTesselator(const View &view,Delegate<void,LogLevel,string> logfunc) :
    view(view),
	zoom(1),
	spacing(.3),
	arrowSize(1.7),
	active(size_t(-1)),
	m_logFunc(logfunc)
{
    
}

void ViewTesselator::appendNodeMesh(vec2 p,float r,vec4 outline,vec4 fill,NodeShape shape,vec2 up)
{
	if (shape == NodeShape::None || (outline.a == 0 && fill.a == 0)) return;
	
	vec2 pt = transf * vec4(p,0,1);
	float rt = r * zoom;
	
	if (!rect2f(pt-vec2(rt),2*vec2(rt)).intersects(rect2f(vec2(),wsize))) return;
	
	int verts;
	Anglef baseA;
	
	if (shape == NodeShape::Circle) {
		verts = min<int>(6*rt, 20);
		baseA = 0;
	} else if (shape == NodeShape::Triangle) {
		verts = 3;
		baseA = deg(90);
	} else if (shape == NodeShape::Square) {
		verts = 4;
		baseA = deg(45);
	}
	
	baseA += pol2(up).angle - pol2(vec2(0,1)).angle;
	
	C(verts+1) {
		pol2 r0(r, baseA + deg(i*360.0/verts));
		pol2 r1(r, baseA + deg((i+1)*360.0/verts));
		
		if (outline.a != 0) {
			line_mesh.pts.push_back(vec2(r0) + p);
			line_mesh.pts.push_back(vec2(r1) + p);
			line_mesh.clr.push_back(outline);
			line_mesh.clr.push_back(outline);
		}
		
		if (fill.a != 0 && i < verts) {
			tris_mesh.pts.push_back(p);
			tris_mesh.pts.push_back(vec2(r0) + p);
			tris_mesh.pts.push_back(vec2(r1) + p);
			tris_mesh.clr.push_back(fill);
			tris_mesh.clr.push_back(fill);
			tris_mesh.clr.push_back(fill);
			tris_mesh.extras[0].push_back(vec4(1,1,1,1) * r * zoom);
			tris_mesh.extras[0].push_back(vec4(0,1,1,1) * r * zoom);
			tris_mesh.extras[0].push_back(vec4(0,1,1,1) * r * zoom);
		}
	}
}

void ViewTesselator::appendEdgeMesh(vec2 A,vec2 B,float w,vec4 outline,vec4 fill,NodeShape beg,NodeShape end)
{
	if (outline.a == 0 && fill.a == 0) return;
	
	vec2 v = B-A;
	vec2 n = v.perp().sgn() * w / 2;
	float l = v.length();
	
	if (outline.a != 0) {
		for (auto coef : {vec2(0,0),vec2(1,0),vec2(1,0),vec2(1,1),vec2(1,1),vec2(0,1),vec2(0,1),vec2(0,0)}) {
			line_mesh.pts.push_back(A + v*coef.x + n*(coef.y*2 - 1));
			line_mesh.clr.push_back(outline);
		}
	}
	
	if (fill.a != 0) {
		for (auto coef : {vec2(0,0),vec2(1,0),vec2(0,1),vec2(1,0),vec2(0,1),vec2(1,1)}) {
			tris_mesh.pts.push_back(A + v*coef.x + n*(coef.y*2 - 1));
			tris_mesh.clr.push_back(fill);
			tris_mesh.extras[0].push_back(vec4(coef.x,1-coef.x,coef.y,1-coef.y) * vec4(l,l,w,w) * zoom);
		}
	}
	
	appendNodeMesh(A,w*arrowSize,outline,fill,beg,-v);
	appendNodeMesh(B,w*arrowSize,outline,fill,end,v);
}

void ViewTesselator::tessNodes()
{
	vec2 up = vec2(transf * vec4(0,1,0,0)) * vec2(1,-1);
	
	C(view.size()) {
		auto &node  = view.graph[i];
        float r     = node.visuals.size;
        vec2  p     = node.body.pos;
		auto  shape = node.visuals.shape;
		
		appendNodeMesh(p, r, vec4::Black, vec4(0,0,0,0), shape, up);
		
		if (active == i) {
			vec4 inClr = vec4(249,57,79,128)/255;
			appendNodeMesh(p, r+1/zoom, vec4(0,0,0,0), inClr, shape, up);
		}
	}
}

void ViewTesselator::tessEdges()
{
    for (auto &node_a : view.graph) {
        float radius_a = node_a.visuals.size;
        
		for (auto &edge : node_a.edges) {
            auto &node_b = view.graph[edge.to];
            float radius_b = node_b.visuals.size;
            
			float w = edge.visuals.width / zoom;
			
			vec2 A = node_a.body.pos;
			vec2 B = node_b.body.pos;
            
			float l = (A-B).length();
			float mov_A = radius_a * (1 + spacing);
			float mov_B = radius_b * (1 + spacing);
			
			if (edge.visuals.end1 != NodeShape::None) mov_A += w*arrowSize;
			if (edge.visuals.end2 != NodeShape::None) mov_B += w*arrowSize;
			
			if (l > mov_A + mov_B + 0.0001) {
				vec2 d = (A-B) / l;
				A += -d * mov_A;
				B += +d * mov_B;
				
				appendEdgeMesh(A,B,w,vec4(0,0,0,0),edge.visuals.color,edge.visuals.end1,edge.visuals.end2);
			}
		}
	}
}

// namespace {
// 	template<class T>
// 	void concat(vector<T> &left,const vector<T> &right) {
// 		left.insert(left.end(),right.begin(),right.end());
// 	}
// }

void ViewTesselator::tess()
{
	line_mesh = Mesh();
	tris_mesh = Mesh();

	line_mesh.extras.resize(1);
	tris_mesh.extras.resize(1);
    
	tessNodes();
	tessEdges();
	
    tris_mesh.faces.push_back(Mesh::Face(fg::Triangles));
	
	if (line_mesh.pts.size())
		line_mesh.faces.push_back(Mesh::Face(fg::Lines));
}
