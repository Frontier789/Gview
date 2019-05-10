#ifndef GVIEW_SELECTIONDATA
#define GVIEW_SELECTIONDATA

#include <Frontier.hpp>

struct SelectionData
{
    enum {
        Empty = -1,
        Node = 0,
        Selector
    } selected;
    
    size_t node_id;
    size_t selector_id;
    
    SelectionData();
    
    static SelectionData node(size_t id);
    static SelectionData selector(size_t node,size_t id);
};

#endif