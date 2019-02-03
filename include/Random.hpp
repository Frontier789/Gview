#ifndef GVIEW_RANDOM
#define GVIEW_RANDOM

#include <random>

class Random
{
    std::mt19937_64 m_gen;

public:
    Random(int seed) : m_gen(seed) {}
    
    double real(double beg = 0,double end = 1);
    size_t integer(size_t beg = 0,size_t end = size_t(-1));
};

#endif