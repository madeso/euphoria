#include "core/default_parse.h"

#include "core/stringmerger.h"
#include "core/functional.h"


namespace euphoria::core::argparse
{
    std::string
    add_quotes_and_combine_with_english_or(const std::vector<std::string>& matches)
    {
        const auto quoted_names =
            map<std::string>(matches, [](const std::string& s)
            {
                // todo(Gustav): improve quote function
                return fmt::format("'{}'", s);
            });

        return string_mergers::english_or.merge(quoted_names);
    }
}

