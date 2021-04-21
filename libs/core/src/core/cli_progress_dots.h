#ifndef EUPHORIA_CORE_CLI_PROGRESS_DOTS_H
#define EUPHORIA_CORE_CLI_PROGRESS_DOTS_H

#include "core/timepoint.h"

namespace euphoria::core
{
    /** Prints dots on the commandline for long running processes.
     */
    struct cli_progress_dots
    {
        TimePoint last;
        int dots_on_line = 0;
        int dots_per_line = 80;
        bool has_printed = false;

        void
        update();
    };
}  // namespace euphoria::core

#endif  // EUPHORIA_CORE_CLI_PROGRESS_DOTS_H

