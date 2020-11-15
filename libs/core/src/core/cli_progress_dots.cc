#include "core/cli_progress_dots.h"

#include <iostream>

namespace euphoria::core
{
    void
    CliProgressDots::Update()
    {
        const auto now = Now();
        if(has_printed)
        {
            if(SecondsBetween(last, now) < 0.2f)
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
}  // namespace euphoria::core
