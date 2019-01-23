#ifndef GVIEW_GRAPHMLIMPORTER_HPP
#define GVIEW_GRAPHMLIMPORTER_HPP

#include "Importer.hpp"
#include <string>
#include <vector>
#include <iosfwd>
#include <map>
using std::string;
using std::vector;
using std::pair;
using std::map;

class TiXmlAttribute;
class TiXmlDocument;
class TiXmlNode;

struct GraphmlImporter : public Importer<string>
{
    GraphmlImporter(string file);
    
    void dump(std::ostream &out);
    
    vector<NodeId> outEdges(NodeId id) const override;
    String label(NodeId id) const override;
    
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

    void loadDoc(TiXmlDocument *doc);
    void load(TiXmlNode *node);
    void addKey(TiXmlNode *node);
    string keyDefault(TiXmlNode *node);
    string tag(TiXmlNode *node);
    void loadGraph(TiXmlNode *node);
    Node loadNode(TiXmlNode *node);
    void loadEdge(TiXmlNode *node);
    void forEachAttrib(TiXmlNode *node, Delegate<void,string,string,TiXmlAttribute*> f);
    string attrib(TiXmlNode *node, string attr);
};

#endif // GVIEW_GRAPHMLIMPORTER_HPP
