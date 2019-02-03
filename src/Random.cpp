#include "Random.hpp"

double Random::real(double beg,double end) {
    std::uniform_real_distribution<double> distribution(beg, end);
    return distribution(m_gen);
}

size_t Random::integer(size_t beg,size_t end) {
    std::uniform_int_distribution<size_t> distribution(beg, end);
    return distribution(m_gen);
}
