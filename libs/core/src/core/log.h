#pragma once

#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

#include "core/undef_windows.h"

namespace euphoria::core
{

  // todo(Gustav): implement category
#define LOG_SPECIFY_DEFAULT_LOGGER(NAME)                                       \

#define LOG_TRACE(...) spdlog::trace(__VA_ARGS__)
#define LOG_DEBUG(...) spdlog::debug(__VA_ARGS__)
#define LOG_INFO(...) spdlog::info(__VA_ARGS__)
#define LOG_WARN(...) spdlog::warn(__VA_ARGS__)
#define LOG_ERROR(...) spdlog::error(__VA_ARGS__)
#define LOG_FATAL(...) spdlog::critical(__VA_ARGS__)

}

