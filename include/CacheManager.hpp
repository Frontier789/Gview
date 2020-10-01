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
    fm::Result loadFromFile(bool nonexistError = false);
    fm::Result saveToFile(const string &file);
    fm::Result saveToFile();
    fm::Result setFile(const string &file,bool load = true,bool nonexistError = false);
    
    Layout load(CacheId id) const;
    bool check(CacheId id) const;
    void save(CacheId id,Layout layout);
    void clear();

private:
    map<CacheId,Layout> m_cache;
    string m_file;
};

#endif