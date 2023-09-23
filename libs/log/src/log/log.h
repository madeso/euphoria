#pragma once

// todo(Gustav): move logging to app layer and use sdl as log base, with fmt formatting instead

namespace eu::log
{
#define LOG_INFO(...) eu::log::get_global_logger()->info(fmt::format(__VA_ARGS__))
#define LOG_WARN(...) eu::log::get_global_logger()->warn(fmt::format(__VA_ARGS__))
#define LOG_ERROR(...) eu::log::get_global_logger()->error(fmt::format(__VA_ARGS__))

#define LOG_INFOS(...) eu::log::get_global_logger()->info(__VA_ARGS__)
#define LOG_WARNS(...) eu::log::get_global_logger()->warn(__VA_ARGS__)
#define LOG_ERRORS(...) eu::log::get_global_logger()->error(__VA_ARGS__)

    struct Logger
    {
        virtual ~Logger() = default;
        virtual void info(const std::string& str) = 0;
        virtual void warn(const std::string& str) = 0;
        virtual void error(const std::string& str) = 0;
    };

    struct ScopedLogger
    {
        explicit ScopedLogger(Logger* new_log);
        ~ScopedLogger();

        ScopedLogger(const ScopedLogger&) = delete;
        ScopedLogger(ScopedLogger&&) = delete;
        void operator=(const ScopedLogger&) = delete;
        void operator=(ScopedLogger&&) = delete;
    };

    Logger* get_global_logger();
}

