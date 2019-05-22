#ifndef GVIEW_TREEIMPORTER
#define GVIEW_TREEIMPORTER

#include "Importer.hpp"

struct TreeImporter : public Importer {
    
    TreeImporter(size_t n = 3,size_t k = 4,bool two_way = true);
    
    View view() override;
    
    size_t getN() const {return n;}
    void setN(size_t new_n) {n = new_n;}
    
    size_t getK() const {return k;}
    void setK(size_t new_k) {k = new_k;}
    
private:
    size_t n,k;
    bool two_way;
};

#endif