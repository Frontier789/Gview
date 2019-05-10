#include "FDGpuMat.hpp"

fm::Result FDGpuMat::createEdgeBufData(const View &view)
{
    size_t s = view.size();
    m_edgeBufData.resize(s * s);
    
    C(s)
        for (auto e : view.graph[i].edges) {
            m_edgeBufData[i*s + e.to] += e.strength;
            m_edgeBufData[e.to*s + i] += e.strength;
        }
    
    return fm::Result();
}

fm::Result FDGpuMat::initEdgeBuf()
{
    auto res = m_edgebuf.setData(&m_edgeBufData[0],m_edgeBufData.size());
    m_edgeBufData.clear();
    return res;
}