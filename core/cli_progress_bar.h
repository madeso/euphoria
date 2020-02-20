#ifndef EUPHORIA_CORE_CLI_PROGRESS_BAR_H
#define EUPHORIA_CORE_CLI_PROGRESS_BAR_H

#include "core/vec2.h"

namespace euphoria::core
{
    struct CliProgressBar
    {
        CliProgressBar();
        ~CliProgressBar();

        vec2i position;

        void Update(float percent, bool force=false);
    };

    struct CliProgressBarInfinite
    {
        CliProgressBar progress;
        float percent = 0.0f;
        int index = 0;

        void
        Step(int each);
    };
}

#endif  // EUPHORIA_CORE_CLI_PROGRESS_BAR_H
