#ifndef GVIEW_GRIDIMPORTER
#define GVIEW_GRIDIMPORTER

#include "Importer.hpp"

struct GridImporter : public Importer {
    
    GridImporter(size_t n = 5);
    
    View view() override;
    
    size_t getN() const {return n;}
    void setN(size_t new_n) {n = new_n;}
    
private:
    size_t n;
};

#endif