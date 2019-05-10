#ifndef GVIEW_FDGEN
#define GVIEW_FDGEN

#include <Frontier.hpp>
using namespace std;

#include "LayoutGen.hpp"
#include "RKDesc.hpp"

struct FDLayoutGen : public LayoutGen
{
    FDLayoutGen(RKDesc rk = RKDesc::fehlberg(),bool do_logging = false,Delegate<void,LogLevel,string> logfun = nullptr);
    
    void run() override;
    
    virtual fm::Result init_memory(const View &view) = 0;
    virtual void update_bodies() = 0;
    
    RKDesc rk() const {return m_rk;}
    
    struct Log {
        vector<float> h; ///< Stepsize of RK
        vector<float> e; ///< Error estimate of RK
        vector<float> s; ///< Maximum of delta of position of bodies
        vector<int> its; ///< Iterations of RK
        bool do_log;
    } log;

    string tag() const override {return "force_directed";}
    
protected:
    double m_h;
    double m_s;
    size_t m_its;
    double m_e;
    bool finished;
    float m_squeeze;
    
private:
    RKDesc m_rk;
    
    Random random;
    
    virtual void sync_layout() = 0;
    virtual void prepareRun() {};
    
    void append_log();
    
    virtual fm::Result init_layout(const View &view);
    fm::Result init(const View &view,bool cached) override;
};

#endif