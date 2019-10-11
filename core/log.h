#ifndef EUPHORIA_LOG_H
#define EUPHORIA_LOG_H

#include <string>
#include <memory>
#include <sstream>

namespace euphoria::core
{
    enum class LogLevel
    {
        Trace,
        Debug,
        Info,
        Warning,
        Error,
        Fatal
    };

    struct Logger;

    typedef std::shared_ptr<Logger> LoggerPtr;

    struct Logger
    {
        Logger(Logger* parent, std::string name);
        bool
        IsEnabledForLevel(LogLevel level) const;
        void
        AddLog(LogLevel level, const std::string& message);

        static LoggerPtr
        GetLogger(const std::string& name);

    private:
        std::string name_;
        LogLevel    level_;
    };

#define LOG_BASE(LOGGER, MESSAGE, LEVEL)                                       \
    do                                                                         \
    {                                                                          \
        if(LOGGER->IsEnabledForLevel(LEVEL))                                   \
        {                                                                      \
            std::ostringstream log_message_stream;                             \
            log_message_stream << MESSAGE;                                     \
            LOGGER->AddLog(LEVEL, log_message_stream.str());                   \
        }                                                                      \
    } while(false)

#define LOG_SPECIFY_DEFAULT_LOGGER(NAME)                                       \
    namespace                                                                  \
    {                                                                          \
        ::euphoria::core::LoggerPtr                                            \
        GetDefaultLogger()                                                     \
        {                                                                      \
            ::euphoria::core::LoggerPtr logger                                 \
                    = euphoria::core::Logger::GetLogger(NAME);                 \
            return logger;                                                     \
        }                                                                      \
    }

#define LOGX_TRACE(LOGGER, MESSAGE) LOG_BASE(LOGGER, MESSAGE, LogLevel::Trace)
#define LOGX_DEBUG(LOGGER, MESSAGE) LOG_BASE(LOGGER, MESSAGE, LogLevel::Debug)
#define LOGX_INFO (LOGGER, MESSAGE) LOG_BASE(LOGGER, MESSAGE, LogLevel::Info)
#define LOGX_WARN (LOGGER, MESSAGE) LOG_BASE(LOGGER, MESSAGE, LogLevel::Warning)
#define LOGX_ERROR(LOGGER, MESSAGE) LOG_BASE(LOGGER, MESSAGE, LogLevel::Error)
#define LOGX_FATAL(LOGGER, MESSAGE) LOG_BASE(LOGGER, MESSAGE, LogLevel::Fatal)

#define LOG_TRACE(MESSAGE)                                                     \
    LOG_BASE(GetDefaultLogger(), MESSAGE, ::euphoria::core::LogLevel::Trace)
#define LOG_DEBUG(MESSAGE)                                                     \
    LOG_BASE(GetDefaultLogger(), MESSAGE, ::euphoria::core::LogLevel::Debug)
#define LOG_INFO(MESSAGE)                                                      \
    LOG_BASE(GetDefaultLogger(), MESSAGE, ::euphoria::core::LogLevel::Info)
#define LOG_WARN(MESSAGE)                                                      \
    LOG_BASE(GetDefaultLogger(), MESSAGE, ::euphoria::core::LogLevel::Warning)
#define LOG_ERROR(MESSAGE)                                                     \
    LOG_BASE(GetDefaultLogger(), MESSAGE, ::euphoria::core::LogLevel::Error)
#define LOG_FATAL(MESSAGE)                                                     \
    LOG_BASE(GetDefaultLogger(), MESSAGE, ::euphoria::core::LogLevel::Fatal)

}  // namespace euphoria::core

#endif  // EUPHORIA_LOG_H
