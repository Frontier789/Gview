#ifndef GVIEW_LOGGER_HPP
#define GVIEW_LOGGER_HPP
#include <Frontier.hpp>
#include <string>

class Logger : fm::NonCopyable
{
    Logger() = default;
public:
    void info(std::string module,std::string str);
    void error(std::string module,std::string str);
    
    static Logger log;
};

void log_info(std::string module,std::string str);
void log_error(std::string module,std::string str);

#endif // GVIEW_LOGGER_HPP