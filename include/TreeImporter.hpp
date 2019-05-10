#ifndef GVIEW_TREEIMPORTER
#define GVIEW_TREEIMPORTER

#include "Importer.hpp"

struct TreeImporter : public Importer {
    
    TreeImporter(size_t n = 3,size_t k = 4);
    
    View view() override;
    
    size_t getN() const {return n;}
    void setN(size_t new_n) {n = new_n;}
    
    size_t getK() const {return n;}
    void setK(size_t new_n) {n = new_n;}
    
private:
    size_t n,k;
};

#endif