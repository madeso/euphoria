#pragma once


namespace euphoria::tests
{
    template
    <
        typename T,
        typename TConverter
    >
    std::string
    vector_to_string_impl
    (
        const std::vector<T>& v,
        bool one_line,
        TConverter converter
    )
    {
        std::ostringstream ss;
        bool first = true;

        std::string newline = one_line ? "" : "\n";

        ss << newline << "{" << newline;
        int index = 0;
        for(const auto& s: v)
        {
            if(first) { first = false; }
            else { ss << ", " << newline; }
            if(one_line == false) { ss << "  " << index << ": "; }
            ss << '\'' << converter(s) << '\'';
            index += 1;
        }
        if(!v.empty()) { ss << newline; }
        ss << "}" << newline;

        return ss.str();
    }


    template
    <
        typename T,
        typename TConverter
    >
    std::pair<std::string, bool>
    vector_to_string_ex(const std::vector<T>& v, TConverter converter)
    {
        const auto oneline = vector_to_string_impl(v, true, converter);
        if( oneline.size() <20)
        {
            return std::make_pair(oneline, true);
        }
        else
        {
            return std::make_pair
            (
                    vector_to_string_impl(v, false, converter),
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
    vector_to_string(const std::vector<T>& v, TConverter converter)
    {
        return vector_to_string_ex(v, converter).first;
    }
}
