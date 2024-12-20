#include "window/log.h"

#include "SDL.h"


namespace eu::window
{

// todo(Gustav) : extend api to supply categories?
/*
SDL_LOG_CATEGORY_APPLICATION,
SDL_LOG_CATEGORY_ERROR,
SDL_LOG_CATEGORY_ASSERT,
SDL_LOG_CATEGORY_SYSTEM,
SDL_LOG_CATEGORY_AUDIO,
SDL_LOG_CATEGORY_VIDEO,
SDL_LOG_CATEGORY_RENDER,
SDL_LOG_CATEGORY_INPUT,
SDL_LOG_CATEGORY_TEST

SDL_LOG_CATEGORY_CUSTOM...
*/

void generic_log(SDL_LogPriority prio, const std::string& str)
{
    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, prio, "%s", str.c_str());
}

void SdlLogger::info(const std::string& str)
{
    generic_log(SDL_LOG_PRIORITY_INFO, str);
}

void SdlLogger::warn(const std::string& str)
{
    generic_log(SDL_LOG_PRIORITY_WARN, str);
}

void SdlLogger::error(const std::string& str)
{
    generic_log(SDL_LOG_PRIORITY_ERROR, str);
}


}

