#include "core/log.h"

#include <iostream>
#include <map>
#include <utility>

#include "core/assert.h"
#include "core/stringutils.h"


namespace euphoria::core
{
    namespace  // local
    {
        char const* const
        LevelToString(LogLevel level)
        {
            switch(level)
            {
            case LogLevel::Trace: return "trace";
            case LogLevel::Debug: return "debug";
            case LogLevel::Info: return "info";
            case LogLevel::Warning: return "warning";
            case LogLevel::Error: return "error";
            case LogLevel::Fatal: return "fatal";
            default:
                DIE("Unhandled LogLevel case");
                return "<UNHANDLED LOGLEVEL>";
            }
        }
    }  // namespace

    Logger::Logger(Logger*, std::string name)
        : name_(std::move(name)), level_(LogLevel::Info)
    {}

    bool
    Logger::IsEnabledForLevel(LogLevel level) const
    {
        return level_ <= level;
    }

    void
    Logger::AddLog(LogLevel level, const std::string& message)
    {
        ASSERT(IsEnabledForLevel(level));

        // ASSERT(sizeof(kLogLevelNames) == static_cast<int>(LogLevel::MAX_VALUE)+1);

        (level >= LogLevel::Warning ? std::cerr : std::cout)
                << "[" << name_ << " " << LevelToString(level) << "] "
                << message << "\n";
    }

    namespace  // local
    {
        std::map<std::string, std::shared_ptr<Logger>>&
        Storage()
        {
            static std::map<std::string, std::shared_ptr<Logger>> Storage;
            return Storage;
        }
    }  // namespace

    LoggerPtr
    Logger::GetLogger(const std::string& name)
    {
        auto& storage = Storage();

        ASSERT(ToLower(name) == name);  // name should be lower string

        auto result = storage.find(name);
        if(result != storage.end())
        {
            return result->second;
        }

        const auto parent_category = StripLastString(name, '.');
        auto       parent          = name.empty() ? nullptr
                                   : GetLogger(
                                             parent_category == name
                                                     ? parent_category
                                                     : "")
                                             .get();

        auto logger   = std::make_shared<Logger>(parent, name);
        storage[name] = logger;
        return logger;
    }

}  // namespace euphoria::core
