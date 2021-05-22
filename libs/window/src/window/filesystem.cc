#include "window/filesystem.h"

#include "SDL_filesystem.h"

namespace euphoria::window
{
    std::string
    get_base_path()
    {
        char* base_path = SDL_GetBasePath();
        if(base_path)
        {
            std::string r = base_path;
            SDL_free(base_path);
            return r;
        }
        else
        {
            return "./";
        }
    }

    std::string
    get_preference_path()
    {
        char* base_path = SDL_GetPrefPath("madeso", "euphoria demo");
        if(base_path)
        {
            std::string pref_path = base_path;
            SDL_free(base_path);
            return pref_path;
        }
        else
        {
            return "";
        }
    }
}
