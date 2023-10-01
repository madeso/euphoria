#include "base/default_parse.h"

#include "base/stringmerger.h"
#include "base/functional.h"


namespace eu
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

