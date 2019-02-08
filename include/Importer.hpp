#ifndef GVIEW_IMPORTER
#define GVIEW_IMPORTER

#include "View.hpp"

struct Importer {
    virtual View view()=0;
};

#endif