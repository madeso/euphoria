#pragma once

// todo(Gustav): move logging to app layer and use sdl as log base, with fmt formatting instead

#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

#include "undef_windows/undef_windows.h"

namespace eu::log
{
#define LOG_INFO(...) spdlog::info(__VA_ARGS__)
#define LOG_WARN(...) spdlog::warn(__VA_ARGS__)
#define LOG_ERROR(...) spdlog::error(__VA_ARGS__)

    void setup_logging(bool force_console);
}

#ifdef EU_LOG_FORCE_CONSOLE
#define EU_INIT_LOGGING() ::eu::log::setup_logging(EU_LOG_FORCE_CONSOLE)
#else
#define EU_INIT_LOGGING() it seems you forgot to to enable logging in cmake with set_win32_defines
#endif
