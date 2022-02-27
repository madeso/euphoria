#include "log/log.h"

#include "spdlog/sinks/stdout_color_sinks.h"

#ifdef _WIN32
#include "spdlog/sinks/msvc_sink.h"
#endif

namespace euphoria::log
{

void setup_logging()
{
#ifdef _WIN32
    auto sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
    auto logger = std::make_shared<spdlog::logger>("msvc_logger", sink);
#else
    auto logger = spdlog::stdout_color_mt("console");
#endif
    spdlog::set_default_logger(logger);
    spdlog::set_pattern("[%H:%M:%S %z] [%n] [%^---%L---%$] [thread %t] %v");
}

}
