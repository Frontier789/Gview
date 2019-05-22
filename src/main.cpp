#include <fstream>
#include <iostream>
#include <Frontier.hpp>
using namespace std;

#include "GridTestApp.hpp"
#include "TreeTestApp.hpp"
#include "ErlApp.hpp"
#include "Tester.hpp"

/*

Background részletesebben (egész bekezdés a régiről, meddig jutott az előző TDK)
Átolvasni az egészet, hogy konzisztens-e
 - Dataflow előre (?Dataflow:reach_1st([Node],[{back, false}],true)) és írni róla
 - Billentyűzet kombináció a rotation-re
 H Layered lehessen rooted
 H Rotation-t tárolni
Conclusion merge

2)-ben hivatkozni a bckg-ra, és hogy tovább fejlesztjük

bin\referl -db kcmini
erlangból +button?


Layouts: StressMajoring
Constrain zooming

write a lot about:
    protocol between erl and cpp
    program source transform of compute shader
    performance comparison of gpu cpu impl
    memory caolescing

Add finished flag to layout cache
Label hiding (make faster?)
LayeredLayout

Dynamic switch of layout algo

*/

struct Gview {
    Gview(int argc,char *argv[]);
    
    void run();
    void create_app();
    
    bool use_gpu;
    bool use_cache;
    string input_type;
    bool test_mode;
    string test_name;
    LogLevel log_lvl;
    ofstream log_file;
    unique_ptr<App> app;
};

Gview::Gview(int argc,char *argv[]) : 
    use_gpu(true),
    use_cache(true),
    input_type("test_grid"),
    test_mode(false),
    log_lvl(LOG_INFO),
    log_file("gview.log"),
    app(nullptr)
{
    for (int i=1;i<argc;++i) {
        string arg = argv[i];
        if (arg == "-i") {
            ++i;
            
            if (i == argc) {
                log_file << "Error: method not specified after -i" << endl;
                exit(1);
            }
            string method = argv[i];
            
            if (method == "erlang" || method == "test_tree" || method == "test_grid") {
                input_type = method;
            } else {
                log_file << "Error: unknown method \"" << method << "\"" << endl;
                exit(1);
            }
        }
        else if (arg == "--test") {
            ++i;
            
            if (i == argc) {
                (input_type == "erlang" ? log_file : cout) << "Error: type of test not specified after --test" << endl;
                exit(1);
            }
            test_mode = true;
            test_name = argv[i];
        }
        else if (arg == "--cache") {
            ++i;
            
            if (i == argc) {
                (input_type == "erlang" ? log_file : cout) << "Error: on / off not specified after --cache" << endl;
                exit(1);
            }
            use_cache = string(argv[i]) == "on";
        }
        else if (arg == "--gpu") {
            ++i;
            
            if (i == argc) {
                (input_type == "erlang" ? log_file : cout) << "Error: on / off not specified after --gpu" << endl;
                exit(1);
            }
            use_gpu = string(argv[i]) == "on";
        }
        else if (arg == "--log") {
            ++i;
            
            if (i == argc) {
                (input_type == "erlang" ? log_file : cout) << "Error: level not specified after --log" << endl;
                exit(1);
            }
            string method = argv[i];
            
            if (method == "all") {
                log_lvl = LOG_DEBUG;
            } else if (method == "data") {
                log_lvl = LOG_DATA;
            } else if (method == "info") {
                log_lvl = LOG_INFO;
            } else if (method == "error") {
                log_lvl = LOG_ERROR;
            } else {
                (input_type == "erlang" ? log_file : cout) << "Error: unknown error level \"" << method << "\"" << endl;
                exit(1);
            }
        }
        else if (arg == "-h" || arg == "--help") {
                (input_type == "erlang" ? log_file : cout) 
                    << "Gview is a graph plotting tool for RefactorErl written in C++" << endl
                    << "\tswitches:" << endl
                    << "\t-i erlang : open gview for input from erlang" << endl
                    << "\t--test TEST : run specified test" << endl
                    << "\t--gpu off : force gpu acceleration off" << endl
                    << "\t--log [all|data|info|error] : set logging level" << endl
                    << endl;
                exit(0);
        }
        else {
            (input_type == "erlang" ? log_file : cout) << "Unrecognised command line argument " << arg << endl;
        }
    }
    
    if (test_mode && input_type == "erlang") {
        log_file << "Error: erlang input and testing mode collision" << endl;
        exit(1);
    }
}

string logLvlToStr(LogLevel lvl)
{
    if (lvl == LOG_DATA)    return "Data";
    if (lvl == LOG_DEBUG)   return "Debug";
    if (lvl == LOG_ERROR)   return "Error";
    if (lvl == LOG_INFO)    return "Info";
    if (lvl == LOG_WARNING) return "Warning";
    if (lvl == LOG_NOTHING) return "-";
    
    return "?";
}

void Gview::create_app() {
    string tag;
    if (input_type == "erlang") tag = "[Erl]";
    if (input_type == "test_tree" || input_type == "test_grid") tag = "[Test]";
    
    auto logger = [&,tag](LogLevel lvl,string s){
        if (lvl <= log_lvl) {
            (input_type == "erlang" ? log_file : cout) << (tag + "," + logLvlToStr(lvl) + ": " + s + "\n");
        }
    };
    
    if (input_type == "erlang") {
        app = unique_ptr<App>(new ErlApp(logger,use_gpu,use_cache));
    } else if (input_type == "test_grid") {
        app = unique_ptr<App>(new GridTestApp(logger,use_gpu,use_cache));
    } else if (input_type == "test_tree") {
        app = unique_ptr<App>(new TreeTestApp(logger,use_gpu,use_cache));
    }
}

void Gview::run() {
    if (test_mode) {
        run_tests(test_name);
    } else {
        create_app();
        app->run();
    }
}


int main(int argc,char *argv[]) {
    
    try {
        Gview gview(argc,argv);
        
        gview.run();
    } catch (exception &e) {
        ofstream out("shit.happened");
        out << e.what() << endl;
    }
}
