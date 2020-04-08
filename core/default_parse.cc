#include "core/default_parse.h"

#include "core/stringmerger.h"
#include "core/functional.h"


namespace euphoria::core::argparse
{
    const std::string
    QuoteAndCombineEnglishOr(const std::vector<std::string>& matches)
    {
        const auto quoted_names =
            Map<std::string>(matches, [](const std::string& s)
            {
                return static_cast<std::string>
                (
                    Str() << '\'' << s << '\''
                );
            });

        return StringMerger::EnglishOr().Generate(quoted_names);
    }
}

