#pragma once

#include "base/stringbuilder.h"


namespace eu::tests
{
    template
    <
        typename T,
        typename TConverter
    >
    std::string
    from_vector_to_string_impl
    (
        const std::vector<T>& v,
        bool one_line,
        TConverter converter
    )
    {
        auto ss = StringBuilder{};
        bool first = true;

        const std::string_view newline = one_line ? "" : "\n";

        ss.add_view(newline).add_char('{').add_view(newline);
        int index = 0;
        for(const auto& s: v)
        {
            if(first) { first = false; }
            else { ss.add_view(", ").add_view(newline); }
            if (one_line == false) { ss.add_string(fmt::to_string(index)); }
            ss.add_char('\'').add_string(converter(s)).add_char('\'');
            index += 1;
        }
        if(!v.empty()) { ss.add_view(newline); }
        ss.add_char('}').add_view(newline);

        return ss.to_string();
    }


    template
    <
        typename T,
        typename TConverter
    >
    std::pair<std::string, bool>
    from_vector_to_string_ex(const std::vector<T>& v, TConverter converter)
    {
        const auto oneline = from_vector_to_string_impl(v, true, converter);
        if( oneline.size() <20)
        {
            return std::make_pair(oneline, true);
        }
        else
        {
            return std::make_pair
            (
                    from_vector_to_string_impl(v, false, converter),
                    false
            );
        }
    }

    template
    <
        typename T,
        typename TConverter
    >
    std::string
    from_vector_to_string(const std::vector<T>& v, TConverter converter)
    {
        return from_vector_to_string_ex(v, converter).first;
    }
}
