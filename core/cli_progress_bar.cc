#include "core/cli_progress_bar.h"

#include "core/cli_position.h"
#include "core/range.h"

#include <iostream>
#include <string>
#include <string_view>
#include <array>

namespace euphoria::core
{
    CliProgressBar::CliProgressBar()
        : position(GetConsolePosition().value_or(vec2i::Zero()))
        , last(Now())
    {
    }

    CliProgressBar::~CliProgressBar()
    {
        Update(1.0f, true);
    }

    void
    CliProgressBar::Update(float apercent, bool force)
    {
        constexpr float min_interval_seconds = 0.1f;
        constexpr int total_number_of_characters = 30;
        /*constexpr std::array animation_chars
        {
            std::string_view{"|"},
            std::string_view{"/"},
            std::string_view{"-"},
            std::string_view{"\\"}
        };*/
        constexpr std::string_view filled_char = "#";
        constexpr std::string_view empty_char = "-";
        constexpr std::string_view start_char = "[";
        constexpr std::string_view end_char = "]";

        const auto percent = KeepWithin(R01(), apercent);

        const auto number_of_characters = static_cast<int>
        (
            percent * total_number_of_characters
        );

        const auto now = Now();
        if (!force && SecondsBetween(last, now) < min_interval_seconds)
        {
            return;
        }
        last = now;

        SetConsolePosition(position);
        std::cout << start_char;
        for (int i = 0; i < number_of_characters; i += 1)
        {
            std::cout << filled_char;
        }
        for
        (
            int i = number_of_characters;
            i < total_number_of_characters;
            i += 1
        )
        {
            std::cout << empty_char;
        }
        std::cout << end_char;

        std::cout << "\n";
    }

    void
    CliProgressBarInfinite::Step(int each)
    {
        index += 1;
        if (index < each)
        {
            return;
        }

        index -= each;

        percent += 0.01f;
        if (percent > 1)
        {
            percent -= 1;
        }

        progress.Update(percent);
    }
}  // namespace euphoria::core
