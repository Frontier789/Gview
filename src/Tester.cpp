#include "Tester.hpp"
#include "FDCpu.hpp"
#include "FDGpuMat.hpp"
#include "GridImporter.hpp"
#include "FDLayoutGen.hpp"
#include "GridTestApp.hpp"
#include <FRONTIER/OpenGL.hpp>
#include <FRONTIER/System/util/file_content.hpp>
#include <iostream>
#include <fstream>

using namespace std;

#define DO_TEST 1

#if DO_TEST == 1
 #ifndef GVIEW_ENABLE_TESTS
  #define GVIEW_ENABLE_TESTS
 #endif
#else
 #ifdef GVIEW_ENABLE_TESTS
  #undef GVIEW_ENABLE_TESTS
 #endif
#endif

#ifdef GVIEW_ENABLE_TESTS

namespace {
    void fdcpu_incremental()
    {
        FDCpu gen;
        gen.startWorkerThread();
        
        for (size_t n=2;n<10;++n) {
            cout << "N: " << n << " ";
            Clock clk;
            GridImporter imp(n);
            gen.setTarget(imp.view());
            gen.wait();
            cout << "t: " << int(clk.ms()) << "ms" << endl;
        }
    }
        
    struct TreeImporter : public Importer {
        
        TreeImporter(size_t n = 5) : n(n) {}
        
        View view() override
        {
            typedef View::Node Node;
            typedef View::Edge Edge;
            View v;
            v.graph.push_back({{15,NodeShape::Circle}, {"ROOT"}, {vec2(),1}, {}});
            C(n)
                v.graph.push_back({{10,NodeShape::Triangle}, {"lvl1"}, {vec2(),1}, {}});
            C(n*n)
                v.graph.push_back({{5,NodeShape::Square}, {"..."}, {vec2(),1}, {}});
            
            C(n)
                v.graph[0].edges.push_back({{4.1,NodeShape::None,NodeShape::Triangle,vec4::Gray},i+1,1});
            
            Cxy(n,n)
                v.graph[1+x].edges.push_back({{3.1,NodeShape::None,NodeShape::None,vec4::Gray},1+n+x*n+y,1});
            
            return v;
        }
        
        size_t getN() const {return n;}
        void setN(size_t new_n) {n = new_n;}
        
    private:
        size_t n;
    };

    // cls && make -j4 && gview.exe --test fdgpu_incremental
    void fdgpu_incremental()
    {
        GLContext cont;
        cont.create();
        cont.setActive();
        
        int beg = 10;
        for (auto rkdesc : {RKDesc::bogacki_shampine(), RKDesc::fehlberg()}) {
            for (size_t n=beg;n<12;++n) {
                beg = 1;
                FDGpuMat gen(rkdesc);
                gen.startWorkerThread();
                
                cout << "N: " << n << " ";
                float avg = 0;
                C(100) {
                    Clock clk;
                    TreeImporter imp(n);
                    gen.setTarget(imp.view());
                    gen.wait();
                    avg += clk.ms();
                }
                
                cout << "t: " << avg/100 << "ms" << endl;
            }
            cout << endl;
        }
        
    }
}

void run_tests(string test)
{
    if (test == "fdcpu_incremental") fdcpu_incremental();
    if (test == "fdgpu_incremental") fdgpu_incremental();
    else {
        cout << "Unknown test \'" << test << "\'" << endl;
    }
}

#else
void run_tests(string test)
{
    cout << "Test \'" << test << "\' could not be run because gview was built testing turned off" << endl;
    cout << "To enable tests, compile with -DGVIEW_ENABLE_TESTS" << endl;
}
#endif