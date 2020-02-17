#include "core/cli_progress_dots.h"

#include <iostream>

namespace euphoria::core
{
    void
    Dots::Dot(int dots_per_line)
    {
        if(index == 0)
        {
            std::cout << ".";
            dots_on_line += 1;

            if(dots_on_line >= dots_per_line)
            {
                std::cout << "\n";
                dots_on_line = 0;
            }
        }
        index += 1;
        if(index > 100)
        {
            index = 0;
        }
    }
}  // namespace euphoria::core
