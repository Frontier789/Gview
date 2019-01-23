#include "RKDesc.hpp"

RKDesc RKDesc::fehlberg()
{
    return RKDesc{
        .9,
        5,
        4,
        1e-2,
        {0,0.25,0.375,0.923077,1,0.5},
        {0.118519,0,0.518986,0.506131,-0.18,0.0363636},
        {0.115741,0,0.548928,0.535331,-0.2,0}
    };
}

RKDesc RKDesc::heun_euler()
{
    return RKDesc{
        .9,
        2,
        1,
        1e-2,
        {0,1},
        {.5,.5},
        {1,0}
    };
}
