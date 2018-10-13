#include "GraphmlImporter.hpp"

#include <tinyxml.h>
#include <iostream>
#include "Logger.hpp"

GraphmlImporter::GraphmlImporter(string file)
{
    TiXmlDocument doc(file.c_str());
    bool loadOkay = doc.LoadFile();
    if (loadOkay) {
        loadDoc(&doc);
    }
    else
    {
        log_error("Graphml", "Failed to load file " + file);
        exit(1);
    }
}

void GraphmlImporter::dump(std::ostream &out)
{
    for (auto &it : m_nodes) {
        out << it.second.id << " node" << std::endl;
        for (auto &edge : it.second.out_edges)
            out << "  --" << edge.id << "--> " << edge.target << std::endl;
    }
}

vector<GraphmlImporter::NodeId> GraphmlImporter::outEdges(NodeId id) const {
    // cout << "requested out edges for " << id << endl;
    vector<NodeId> ret;
    for (auto &e : m_nodes.find(id)->second.out_edges) {
        ret.push_back(e.target);
        // cout << " " << e.target << endl;
    }
    return ret;
}

String GraphmlImporter::label(NodeId id) const {
    return id;
}
    
void GraphmlImporter::loadDoc(TiXmlDocument *doc)
{
    TiXmlNode *node = doc;
    load(node);
}

void GraphmlImporter::load(TiXmlNode *node)
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

void GraphmlImporter::addKey(TiXmlNode *node)
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

string GraphmlImporter::keyDefault(TiXmlNode *node)
{
    for (auto child = node->FirstChild();child;child = child->NextSibling())
    {
        if (tag(child) == "default")
            return child->ToElement()->FirstChild()->Value();
    }
    return "";
}

string GraphmlImporter::tag(TiXmlNode *node)
{
    if (!node) return "";
    
    return node->Value();
}

void GraphmlImporter::loadGraph(TiXmlNode *node)
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

GraphmlImporter::Node GraphmlImporter::loadNode(TiXmlNode *node)
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

void GraphmlImporter::loadEdge(TiXmlNode *node)
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

void GraphmlImporter::forEachAttrib(TiXmlNode *node, Delegate<void,string,string,TiXmlAttribute*> f)
{
    if (!node) return;
    
    auto elem = node->ToElement();

    for (TiXmlAttribute *attr = elem->FirstAttribute();attr;attr = attr->Next())
        f(attr->Name(), attr->Value(), attr);
}

string GraphmlImporter::attrib(TiXmlNode *node, string attr)
{
    string ret;
    forEachAttrib(node, [&](string a,string v){
        if (a == attr) ret = v;
    });
    return ret;
}
