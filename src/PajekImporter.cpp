#include "PajekImporter.hpp"
#include "Logger.hpp"
#include <fstream>
#include <sstream>
#include <map>
using namespace std;

vector<size_t> PajekImporter::outEdges(NodeId id) const
{
    return m_graph[id];
}

String PajekImporter::label(NodeId id) const
{
    return m_labels[id];
}

PajekImporter::PajekImporter(string file)
{
    std::ifstream in(file.c_str());
    if (!in) {
        log_error("PajekImporter","Failed to load file " + file);
        exit(1);
    }
    
    string state = "";
    map<size_t,size_t> idToIndex;
    
    while (!in.eof()) {
        string line;
        getline(in, line);
        bool cmd = line[0] == '*';
        
        stringstream ss(std::move(line));
        
        if (cmd) {
            string s;
            ss >> s;
            state = s.substr(1);
        } else {
            if (state == "Vertices") {
                size_t id;
                string label;
                ss >> id >> label;
                size_t index = idToIndex.size();
                idToIndex[id] = index;
                m_labels.push_back(label);
                m_graph.push_back(vector<size_t>());
            }
            if (state == "Arcs") {
                size_t a,b;
                double r;
                in >> a >> b >> r;
                
                m_graph[idToIndex[a]].push_back(idToIndex[b]);
                m_graph[idToIndex[b]].push_back(idToIndex[a]);
            }
        }
    }
}
