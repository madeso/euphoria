#pragma once

#include "core/timepoint.h"

namespace euphoria::core
{
    /** Prints dots on the commandline for long running processes.
     */
    struct cli_progress_dots
    {
        time_point last;
        int dots_on_line = 0;
        int dots_per_line = 80;
        bool has_printed = false;

        void
        update();
    };
}
