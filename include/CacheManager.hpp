#ifndef GVIEW_CHACHEMANAGER
#define GVIEW_CHACHEMANAGER

#include <Frontier.hpp>
#include "Layout.hpp"

#include <string>
#include <map>
using std::map;
using std::string;

typedef string CacheId;

struct CacheManager
{
    CacheManager() = default;
    CacheManager(const string &file);
    
    ~CacheManager();
    
    fm::Result loadFromFile(const string &file,bool nonexistError = false);
    fm::Result saveToFile(const string &file);
    
    void setOutFile(const string &file);
    
    Layout load(CacheId id) const;
    bool check(CacheId id) const;
    void save(CacheId id,Layout layout);

public:
    map<CacheId,Layout> m_cache;
    string m_outFile;
};

#endif