#ifndef GVIEW_GRIDIMPORTER
#define GVIEW_GRIDIMPORTER

#include "Importer.hpp"

struct GridImporter : public Importer {
    
    GridImporter(size_t n = 5);
    
    View view() override;
    
private:
    size_t n;
};

#endif