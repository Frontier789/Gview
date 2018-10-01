#include "Logger.hpp"
#include <iostream>
using namespace std;

void Logger::info(std::string module,std::string str)
{
    cout << "[info]{" << module << "}: " << str << endl;
}

void Logger::error(std::string module,std::string str)
{
    cout << "[error]{" << module << "}: " << str << endl;
}

void log_info(std::string module,std::string str)
{
    Logger::log.info(module,str);
}

void log_error(std::string module,std::string str)
{
    Logger::log.error(module,str);
}

Logger Logger::log;
