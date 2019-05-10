#include "CacheManager.hpp"
#include <fstream>

CacheManager::CacheManager(const string &file) : m_file(file)
{
    loadFromFile(file);
}

CacheManager::~CacheManager()
{
    saveToFile();
}

namespace
{
    template<class T>
    void writeRaw(std::ofstream &out,const T &t)
    {
        out.write((const char *)&t,sizeof(t));
    }
    
    template<class T>
    void readRaw(std::ifstream &in,T &t)
    {
        in.read((char*)&t,sizeof(t));
    }
    
    template<class T>
    T readRaw(std::ifstream &in)
    {
        T t;
        in.read((char*)&t,sizeof(t));
        return t;
    }
    
    Layout readLayout(std::ifstream &in)
    {
        size_t size = readRaw<size_t>(in);
        Layout l;
        
        l.positions.resize(size);
        in.read((char*)&l.positions[0],sizeof(l.positions[0]) * size);
        
        return l;
    }
    
    CacheId readId(std::ifstream &in)
    {
        size_t len = readRaw<size_t>(in);
        string id(len,'a');
        in.read(&id[0],len);
        
        return id;
    }
    
    void writeLayout(std::ofstream &out,const Layout &l)
    {
        writeRaw<size_t>(out,l.positions.size());
        
        out.write((char*)&l.positions[0],sizeof(l.positions[0]) * l.positions.size());
    }
    
    void writeId(std::ofstream &out,CacheId id)
    {
        writeRaw<size_t>(out,id.size());
        
        out.write(id.c_str(),id.size());
    }
}

fm::Result CacheManager::setFile(const string &file,bool load,bool nonexistError)
{
    if (!load) {
        m_file = file;
        return {};
    } else {
        saveToFile();
        m_file = file;
        return loadFromFile(nonexistError);
    }
}

void CacheManager::clear()
{
    m_cache.clear();
}

fm::Result CacheManager::loadFromFile(bool nonexistError)
{
    return loadFromFile(m_file,nonexistError);
}

fm::Result CacheManager::loadFromFile(const string &file,bool nonexistError)
{
    if (file.empty()) return fm::Result();
    
    std::ifstream in(file.c_str(),std::fstream::binary);
    if (!in)
    {
        if (!nonexistError) return fm::Result();
        
        return fm::Result("IOError",fm::Result::OPFailed,"FileNotReadable","loadFromFile",__FILE__,__LINE__);
    }
    
    size_t cacheCount = readRaw<size_t>(in);
    
    m_cache.clear();
    for (size_t i=0;i<cacheCount;++i)
    {
        CacheId id = readId(in);
        m_cache[id] = readLayout(in);
    }
    
    if (!in) return fm::Result("CacheError",fm::Result::OPFailed,"InvalidCache","loadFromFile",__FILE__,__LINE__,"Cache file appears to be incorrect");
    
    return fm::Result();
}

fm::Result CacheManager::saveToFile()
{
    return saveToFile(m_file);
}

fm::Result CacheManager::saveToFile(const string &file)
{
    if (file == "") return fm::Result();
    
    std::ofstream out(file.c_str(),std::fstream::binary);
    if (!out)
    {
        return fm::Result("IOError",fm::Result::OPFailed,"FileNotWritable","saveToFile",__FILE__,__LINE__);
    }
    
    writeRaw<size_t>(out,m_cache.size());
    
    for (auto &pr : m_cache)
    {
        writeId(out,pr.first);
        writeLayout(out,pr.second);
    }
    
    return fm::Result();
}

bool CacheManager::check(CacheId id) const
{
    return m_cache.count(id);
}

Layout CacheManager::load(CacheId id) const
{
    auto it = m_cache.find(id);
    
    if (it == m_cache.end()) return Layout();
    
    return it->second;
}

void CacheManager::save(CacheId id,Layout layout)
{
    if (!id.empty())
        m_cache[id] = std::move(layout);
}
