#include "core/cli_progress_dots.h"

#include <iostream>

namespace euphoria::core
{
    void
    CliProgressDots::update()
    {
        const auto now = get_current_timepoint();
        if(has_printed)
        {
            if(get_seconds_between(last, now) < 0.2f)
            {
                return;
            }
        }
        std::cout << ".";
        std::cout.flush();
        dots_on_line += 1;

        last = now;
        has_printed = true;

        if(dots_on_line >= dots_per_line)
        {
            std::cout << "\n";
            dots_on_line = 0;
        }
    }
}
