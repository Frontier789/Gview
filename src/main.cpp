#include <fstream>
#include <iostream>
#include <Frontier.hpp>
using namespace std;

#include "App.hpp"
#include "ErlApp.hpp"

struct Gview {
    Gview(int argc,char *argv[]);
    
    void run();
    void create_app();
    
    bool erl_input;
    ofstream log_file;
    unique_ptr<App> app;
};

Gview::Gview(int argc,char *argv[]) : 
    erl_input(false),
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
            
            if (method == "erlang") {
                erl_input = true;
            } else {
                log_file << "Error: unknown method \"" << method << "\"" << endl;
                exit(1);
            }
        }
    }
}

void Gview::create_app() {
    if (erl_input) {
        app = unique_ptr<App>(new ErlApp([&](string s){log_file << "[Erl]: " << s << endl;}));
        
    } else {
        app = unique_ptr<App>(new App([&](string s){log_file << "[App]: " << s << endl;}));
    }
}

void Gview::run() {
    create_app();
    app->run();
}


int main(int argc,char *argv[]) {
    
    Gview gview(argc,argv);
    
    gview.run();
}
