#include "core/log.h"

#include <iostream>
#include <map>
#include <utility>

#include "core/assert.h"
#include "core/stringutils.h"

const char* const kLogLevelNames[] = {"trace",   "debug", "info",
                                      "warning", "error", "fatal"};

namespace  // local
{
}

Logger::Logger(Logger* parent, std::string name)
    : parent_(parent)
    , name_(std::move(name))
    , level_(LogLevel::Info)
{
}

bool
Logger::IsEnabledForLevel(LogLevel level) const
{
  return level_ <= level;
}

void
Logger::AddLog(LogLevel level, const std::string& message)
{
  Assert(IsEnabledForLevel(level));

  // Assert(sizeof(kLogLevelNames) == static_cast<int>(LogLevel::MAX_VALUE)+1);

  (level >= LogLevel::Warning ? std::cerr : std::cout)
      << "[" << name_ << " " << kLogLevelNames[static_cast<int>(level)] << "] "
      << message << "\n";
}

namespace  // local
{
  std::map<std::string, std::shared_ptr<Logger>>&
  Storage()
  {
    static std::map<std::string, std::shared_ptr<Logger>> s_Storage;
    return s_Storage;
  }
}

LoggerPtr
Logger::GetLogger(const std::string& name)
{
  auto& storage = Storage();

  Assert(ToLower(name) == name);  // name should be lower string

  auto result = storage.find(name);
  if(result != storage.end())
  {
    return result->second;
  }

  const auto parent_category = StripLastString(name, '.');
  auto       parent =
      name.empty()
          ? nullptr
          : GetLogger(parent_category == name ? parent_category : "").get();

  auto logger   = std::make_shared<Logger>(parent, name);
  storage[name] = logger;
  return logger;
}
