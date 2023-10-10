#include "log/log.h"

#include <iostream>
#include <cassert>


namespace
{
    struct ConsoleLogger : eu::log::Logger
    {
        void info(const std::string& str) override
        {
            std::cout << str << "\n";
        }

        void warn(const std::string & str) override
        {
            std::cerr << str << "\n";
        }

        void error(const std::string & str) override
        {
            std::cerr << str << "\n";
        }
    };

    eu::log::Logger* global_logger = nullptr;
}


namespace eu::log
{
    Logger*
    get_global_logger()
    {
        static ConsoleLogger console_logger;
        Logger* logger = global_logger;

        if (logger != nullptr) { return logger; }
        else { return &console_logger; }
    }

    
    ScopedLogger::ScopedLogger(Logger* new_log)
    {
        assert(global_logger);
        global_logger = new_log;
    }

    ScopedLogger::~ScopedLogger()
    {
        global_logger = nullptr;
    }
}
