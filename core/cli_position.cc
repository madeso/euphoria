#include "core/cli_position.h"

#ifdef WIN32
#include <windows.h>
#include "undef_windows.h"
#endif


namespace euphoria::core
{
#ifdef WIN32
    std::optional<vec2i>
    GetConsolePosition()
    {
        HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
        if (output == INVALID_HANDLE_VALUE)
        {
            return std::nullopt;
        }

        CONSOLE_SCREEN_BUFFER_INFO info;
        if (!GetConsoleScreenBufferInfo(output, &info))
        {
            return std::nullopt;
        }
        
        return vec2i{ info.dwCursorPosition.X, info.dwCursorPosition.Y };
    }

    bool
        SetConsolePosition(const vec2i& pos)
    {
        HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
        if (output == INVALID_HANDLE_VALUE)
        {
            return false;
        }

        COORD coord;
        coord.X = pos.x;
        coord.Y = pos.y;

        if (!SetConsoleCursorPosition(output, coord))
        {
            return false;
        }

        return true;
    }
#else
    std::optional<vec2i>
    GetConsolePosition()
    {
        return std::nullopt;
    }

    bool
    SetConsolePosition(const vec2i& pos)
    {
        return false;
    }
#endif
}
