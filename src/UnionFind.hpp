#include <map>
using std::map;

template<class T>
struct UnionFind {
    map<T,T> m_parents;
    map<T,int> m_ranks;
    
    T find(T i) 
    {
        auto it = m_parents.find(i);
        if (it == m_parents.end()) {
            m_parents[i] = i;
            m_ranks[i] = 0;
            it = m_parents.find(i);
        }
        
        if (it->second != i)
            it->second = find(it->second); 
    
        return it->second; 
    } 
  
    void unite(T x, T y) 
    {
        T xroot = find(x);
        T yroot = find(y);
        
        int &rx = m_ranks[xroot];
        int &ry = m_ranks[yroot];
        if (rx < ry)
            m_parents[xroot] = yroot;
        else if (rx > ry)
            m_parents[yroot] = xroot;
        else
        {
            m_parents[yroot] = xroot;
            ry++;
        }
    }
};
