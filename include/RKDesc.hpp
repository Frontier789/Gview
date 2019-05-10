#ifndef GVIEW_RKDESC
#define GVIEW_RKDESC

#include <Frontier.hpp>
using namespace std;

#include "LayoutGen.hpp"

struct RKDesc
{
    double h0;  //< Starting step-size
    double N_h; //< Rate of convergence of the higher order method
    double N_e; //< Rate of convergence of the embedded method
    double eps; //< Allowed error in pixels
    
    vector<double> A;
    vector<double> C_H;
    vector<double> C_E;
    
    static RKDesc fehlberg();
    static RKDesc bogacki_shampine();
    static RKDesc heun_euler();
};

#endif