#include "log/log.h"

#include "spdlog/sinks/stdout_color_sinks.h"

#ifdef _WIN32
#include "spdlog/sinks/msvc_sink.h"
#endif


namespace
{
    std::shared_ptr<spdlog::logger>
    create_console_logger(const std::string& name)
    {
        return spdlog::stdout_color_mt(name);
    }


    std::shared_ptr<spdlog::logger>
    create_msvc_logger([[maybe_unused]] const std::string& name)
    {
#ifdef _WIN32
        auto sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
        auto logger = std::make_shared<spdlog::logger>(name, sink);
        return logger;
#else
        return nullptr;
#endif
    }


    std::shared_ptr<spdlog::logger>
    create_logger(const std::string& name, bool force_console)
    {
        if(force_console == false)
        {
            const auto msvc = create_msvc_logger(name);
            if(msvc != nullptr)
            {
                return msvc;
            }
        }
        
        return create_console_logger(name);
    }
}


namespace euphoria::log
{
    void
    setup_logging(bool force_console)
    {
        auto logger = create_logger("logger", force_console);
        spdlog::set_default_logger(logger);
        spdlog::set_pattern("[%H:%M:%S] %v");
    }
}
