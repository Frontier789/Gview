#include "Importer.hpp"
#include "Gview.hpp"
#include <iostream>
using namespace std;

class avec2i
{
public:
    int x,y;
    bool operator==(avec2i p) const {return x==p.x && y==p.y;}
    bool operator<(avec2i v) const {if (x != v.x) return x < v.x; return y < v.y;}
};

class MyImporter : public Importer<avec2i>
{
public:
    vector<NodeId> outEdges(NodeId id) const {
        int h = id.x ^ id.y + id.x*id.y;
        srand(h);
        
        int treshold = 50;
        vector<NodeId> ret;
        if (rand()%100 < treshold) ret.push_back(avec2i{id.x-1,id.y});
        if (rand()%100 < treshold) ret.push_back(avec2i{id.x+1,id.y});
        if (rand()%100 < treshold) ret.push_back(avec2i{id.x,id.y-1});
        if (rand()%100 < treshold) ret.push_back(avec2i{id.x,id.y+1});
        if (rand()%100 < treshold) ret.push_back(avec2i{id.x+1,id.y+1});
        if (rand()%100 < treshold) ret.push_back(avec2i{id.x-1,id.y-1});
        
        return ret;
    }
    
    String label(NodeId id) const override {
        return "{" + toString(id.x) + "," + toString(id.y) + "}";
    }
};


// TODO:
//     latice drawing
//     graphml importer
//     animated view change

#include <tinyxml.h>

struct GraphmlImporter : public Importer<string>
{
    GraphmlImporter(string file)
    {
        TiXmlDocument doc(file.c_str());
        bool loadOkay = doc.LoadFile();
        if (loadOkay) {
            loadDoc(doc);
        }
        else
        {
            cout << "Failed to load file " << file << endl;
            exit(1);
        }
    }
    
    void dump(ostream &out)
    {
        for (auto &it : m_nodes) {
            out << it.second.id << " node" << endl;
            for (auto &edge : it.second.out_edges)
                out << "  --" << edge.id << "--> " << edge.target << endl;
        }
    }
    
    vector<NodeId> outEdges(NodeId id) const {
        // cout << "requested out edges for " << id << endl;
        vector<NodeId> ret;
        for (auto &e : m_nodes.find(id)->second.out_edges) {
            ret.push_back(e.target);
            // cout << " " << e.target << endl;
        }
        return ret;
    }
    
    String label(NodeId id) const override {
        return id;
    }
    
private:
    struct Key {string meaning, type, def;};  
    struct OutEdge {
        string id;
        string target;
        map<string,string> data;
    };
    struct Node {
        string id;
        map<string,string> data;
        vector<OutEdge> out_edges;
    };
    
    map<pair<string,bool>,Key> m_keys;
    map<string,Node> m_nodes;

    void loadDoc(TiXmlDocument &doc)
    {
        TiXmlNode *node = &doc;
        load(node);
    }
    
    void load(TiXmlNode *node)
    {
        if (!node) return;
        
        if (node->Type() == TiXmlNode::TINYXML_ELEMENT)
        {
            string ntag = tag(node);
            if (ntag == "key") addKey(node);
            if (ntag == "graph") loadGraph(node);
        }

        for (auto child = node->FirstChild();child;child = child->NextSibling()) 
            load(child);
    }
    
    void addKey(TiXmlNode *node)
    {
        string keyid;
        bool isnode = false;
        string meaning;
        string type;
        
        forEachAttrib(node, [&](string attr,string val){
            if (attr == "id") keyid = val;
            if (attr == "for") isnode = val == "node";
            if (attr == "attr.name") meaning = val;
            if (attr == "attr.type") type = val;
        });
        
        string def = keyDefault(node);
        
        // cout << "(" << keyid << "," << isnode << ") -> (" << meaning << "," << type << "," << def << ")" << endl;
        
        m_keys[make_pair(keyid,isnode)] = {meaning,type,def};
    }
    
    string keyDefault(TiXmlNode *node)
    {
        for (auto child = node->FirstChild();child;child = child->NextSibling())
        {
            if (tag(child) == "default")
                return child->ToElement()->FirstChild()->Value();
        }
        return "";
    }
    
    string tag(TiXmlNode *node)
    {
        if (!node) return "";
        
        return node->Value();
    }
    
    void loadGraph(TiXmlNode *node)
    {
        for (auto child = node->FirstChild();child;child = child->NextSibling()) 
        {
            string t = tag(child);
            if (t == "node") {
                Node node = loadNode(child);
                m_nodes[node.id].id = node.id;
                m_nodes[node.id].data = std::move(node.data);
            }
            if (t == "edge") {
                loadEdge(child);
            }
        }
    }
    
    Node loadNode(TiXmlNode *node)
    {
        Node ret;
        ret.id = attrib(node, "id");
        
        for (auto child = node->FirstChild();child;child = child->NextSibling())
        {
            string t = tag(child);
            if (t == "data") {
                ret.data[attrib(child, "key")] = child->ToElement()->FirstChild()->Value();
                // cout << attrib(child, "key") << " ---> " << child->ToElement()->FirstChild()->Value() << endl;
            }
        }
        
        return ret;
    }
    
    void loadEdge(TiXmlNode *node)
    {
        string id = attrib(node, "id");
        string source = attrib(node, "source");
        string target = attrib(node, "target");
        
        auto &node1 = m_nodes[source];
        node1.id = source;
        node1.out_edges.push_back(OutEdge{id, target});
        auto &oe1 = node1.out_edges.back();
        
        auto &node2 = m_nodes[target];
        node2.id = source;
        node2.out_edges.push_back(OutEdge{id, source});
        auto &oe2 = node2.out_edges.back();
        
        for (auto child = node->FirstChild();child;child = child->NextSibling())
        {
            string t = tag(child);
            if (t == "data") {
                oe1.data[attrib(child, "key")] = child->ToElement()->FirstChild()->Value();
                oe2.data[attrib(child, "key")] = child->ToElement()->FirstChild()->Value();
                // cout << attrib(child, "key") << " -|-> " << child->ToElement()->FirstChild()->Value() << endl;
            }
        }
    }
    
    void forEachAttrib(TiXmlNode *node, Delegate<void,string,string,TiXmlAttribute*> f)
    {
        if (!node) return;
        
        auto elem = node->ToElement();

        for (TiXmlAttribute *attr = elem->FirstAttribute();attr;attr = attr->Next())
            f(attr->Name(), attr->Value(), attr);
    }
    
    string attrib(TiXmlNode *node, string attr)
    {
        string ret;
        forEachAttrib(node, [&](string a,string v){
            if (a == attr) ret = v;
        });
        return ret;
    }
};

int main() {
    {
        Gview<GraphmlImporter> gview("hello.xml");
        gview.start("n0",ViewParams{3});
        gview.wait();
        return 0;
    }
    
    Gview<MyImporter> gview;
    gview.start(avec2i{1,1},ViewParams{3});
    gview.wait();
    return 0;
}
