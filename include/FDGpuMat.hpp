#ifndef GVIEW_FDGPUMAT
#define GVIEW_FDGPUMAT

#include <Frontier.hpp>
using namespace std;

#include "FDGpu.hpp"

struct FDGpuMat : public FDGpu
{
    FDGpuMat(RKDesc rk = RKDesc::fehlberg(),bool do_logging = false,Delegate<void,LogLevel,string> logfun = nullptr) : FDGpu(rk,do_logging,logfun) {}

protected:
    fm::Result createEdgeBufData(const View &view) override;
    fm::Result initEdgeBuf() override;
    
    vector<float> m_edgeBufData;
};

#endif