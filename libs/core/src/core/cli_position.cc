#include "core/cli_position.h"

#ifdef WIN32
#include <windows.h>
#include "undef_windows.h"
#else
#include <cstdio>

#include <unistd.h>
#include <termios.h>
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
    namespace
    {
        // https://stackoverflow.com/a/9547954/180307
        struct UnbufferedInput
        {
            struct termios old_tio;

            UnbufferedInput()
            {
                // get the terminal settings for stdin
                tcgetattr(STDIN_FILENO,&old_tio);

                // we want to keep the old setting to restore them a the end
                auto new_tio=old_tio;

                // disable canonical mode (buffered i/o) and local echo
                new_tio.c_lflag &=(~ICANON & ~ECHO);

                // set the new settings immediately
                tcsetattr(STDIN_FILENO,TCSANOW,&new_tio);
            }

            ~UnbufferedInput()
            {
                // restore the former settings
                tcsetattr(STDIN_FILENO,TCSANOW,&old_tio);
            }
        };
    }

    // linux implementation heavily inspired by this blog entry
    // https://thoughtsordiscoveries.wordpress.com/2017/04/26/set-and-read-cursor-position-in-terminal-windows-and-linux/

    std::optional<vec2i>
    GetConsolePosition()
    {
        // This escape sequence writes the current coordinates to the terminal.
        // We then have to read it from there

        // Step 1 to read cursor pos using printf/scanf
        // http://stackoverflow.com/questions/16026858/reading-the-device-status-report-ansi-escape-sequence-reply
        printf("\033[6n");
        fflush(stdout);

        // Step 2: make scanf non-blocking so it doesnt wait for newline after the response
        // http://stackoverflow.com/questions/8101079/making-stdin-non-blocking
        int x =0;
        int y = 0;
        // scanf("\033[%d;%dR", &y, &x);
        std::ostringstream ss;
        char c = ' ';
        {
            UnbufferedInput unbuffered_input;
            do
            {
                c = getchar();
                ss << c;
            } while(c != 'R');
        }
        std::string s = ss.str();
        sscanf(s.c_str(), "\033[%d;%dR", &y, &x);

        return vec2i{x-1, y-1};
    }

    bool
    SetConsolePosition(const vec2i& pos)
    {
        printf("\033[%d;%dH", pos.y+1, pos.x+1);
        return true;
    }
#endif
}
