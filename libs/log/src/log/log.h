#pragma once

#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

#include "undef_windows/undef_windows.h"

namespace euphoria::log
{

#define LOG_TRACE(...) spdlog::trace(__VA_ARGS__)
#define LOG_DEBUG(...) spdlog::debug(__VA_ARGS__)
#define LOG_INFO(...) spdlog::info(__VA_ARGS__)
#define LOG_WARN(...) spdlog::warn(__VA_ARGS__)
#define LOG_ERROR(...) spdlog::error(__VA_ARGS__)
#define LOG_FATAL(...) spdlog::critical(__VA_ARGS__)

    void setup_logging(bool force_console);
}

#ifdef EUPHORIA_LOG_FORCE_CONSOLE
#define EUPH_INIT_LOGGING() ::euphoria::log::setup_logging(EUPHORIA_LOG_FORCE_CONSOLE)
#else
#define EUPH_INIT_LOGGING() it seems you forgot to to enable logging in cmake with set_win32_defines
#endif
