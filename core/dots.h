#ifndef EUPHORIA_CORE_DOTS_H
#define EUPHORIA_CORE_DOTS_H

namespace euphoria::core
{
    /** Prints dots on the commandline for long running processes.
     */
    struct Dots
    {
        int index        = 0;
        int dots_on_line = 0;

        void
        Dot(int dots_per_line);
    };
}

#endif  // EUPHORIA_CORE_DOTS_H

