#pragma once

// todo(Gustav): move logging to app layer and use sdl as log base, with fmt formatting instead

namespace eu::log
{
/// Writes a plain string to the info log.
/// @see \ref LOG_INFO for a fmt info log
/// \hideinitializer
#define LOG_INFOS(...) eu::log::get_global_logger()->info(__VA_ARGS__)

/// Writes a plain string to the warning log.
/// @see \ref LOG_WARN for a fmt warning log
/// \hideinitializer
#define LOG_WARNS(...) eu::log::get_global_logger()->warn(__VA_ARGS__)

/// Writes a plain string to the error log.
/// @see \ref LOG_ERR for a fmt error log
/// \hideinitializer
#define LOG_ERRS(...) eu::log::get_global_logger()->error(__VA_ARGS__)

/// Writes a info log using fmt.
/// @see \ref LOG_INFOS for just a plain string info log
/// \hideinitializer
#define LOG_INFO(...) LOG_INFOS(fmt::format(__VA_ARGS__))

/// Writes a warning log using fm.
/// @see \ref LOG_WARNS for a plain string warning log
/// \hideinitializer
#define LOG_WARN(...) LOG_WARNS(fmt::format(__VA_ARGS__))

/// Writes a error log using fm.
/// @see \ref LOG_ERRS for a plain string error log
/// \hideinitializer
#define LOG_ERR(...) LOG_ERRS(fmt::format(__VA_ARGS__))


    /// Logger output class.
    /// Inherit from this to add a a custom output.
    struct Logger
    {
        Logger() = default;
        virtual ~Logger() = default;

        Logger(const Logger&) = delete;
        Logger(Logger&&) = delete;
        void operator=(const Logger&) = delete;
        void operator=(Logger&&) = delete;

        /// Called when printing a info message.
        virtual void info(const std::string& str) = 0;
        
        /// Called when printing a warning message.
        virtual void warn(const std::string& str) = 0;

        /// Called when printing a error message.
        virtual void error(const std::string& str) = 0;
    };

    /// RAII handler to change the current \ref Logger class.
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

