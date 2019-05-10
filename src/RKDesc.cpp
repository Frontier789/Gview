#include "RKDesc.hpp"

RKDesc RKDesc::fehlberg()
{
    return RKDesc{
        .9,
        5,
        4,
        1e-1,
        {0,0.25,0.375,0.923077,1,0.5},
        {0.118519,0,0.518986,0.506131,-0.18,0.0363636},
        {0.115741,0,0.548928,0.535331,-0.2,0}
    };
}

RKDesc RKDesc::bogacki_shampine()
{
    return RKDesc{
        .9,
        3,
        2,
        1e-1,
        {0,0.5,0.75,1},
        {2.f/9,1.f/3,4.f/9,0},
        {7.f/24,1.f/4,1.f/3,1.f/8}
    };
}

RKDesc RKDesc::heun_euler()
{
    return RKDesc{
        .9,
        2,
        1,
        1e-1,
        {0,1},
        {.5,.5},
        {1,0}
    };
}
