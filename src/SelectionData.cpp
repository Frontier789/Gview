#include "SelectionData.hpp"
    
SelectionData::SelectionData() :
    selected(Empty),
    node_id(size_t(-1)),
    selector_id(size_t(-1))
{}
    
SelectionData SelectionData::node(size_t id)
{
    SelectionData d;
    d.selected = Node;
    d.node_id = id;
    return d;
}

SelectionData SelectionData::selector(size_t node,size_t id)
{
    SelectionData d;
    d.selected = Selector;
    d.node_id = node;
    d.selector_id = id;
    return d;
}
