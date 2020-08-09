#ifndef EUPHORIA_TESTS_VECTOR_TO_STRING_H
#define EUPHORIA_TESTS_VECTOR_TO_STRING_H

#include <string>
#include <sstream>
#include <vector>

namespace euphoria::tests
{
    template
    <
        typename T,
        typename TConverter
    >
    std::string
    VectorToString
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
        for(const auto s: v)
        {
            if(first) first = false;
            else ss << ", " << newline;
            if(one_line == false) { ss << "  " << index << ": "; }
            ss << '\'' << converter(s) << '\'';
            index += 1;
        }
        if(!v.empty()) ss << newline;
        ss << "}" << newline;

        return ss.str();
    }


    template
    <
        typename T,
        typename TConverter
    >
    std::pair<std::string, bool>
    VectorToStringEx(const std::vector<T>& v, TConverter converter)
    {
        const auto oneline = VectorToString(v, true, converter);
        if( oneline.size() <20)
        {
            return std::make_pair(oneline, true);
        }
        else
        {
            return std::make_pair
            (
                VectorToString(v, false, converter),
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
    VectorToString(const std::vector<T>& v, TConverter converter)
    {
        return VectorToStringEx(v, converter).first;
    }
}

#endif  // EUPHORIA_TESTS_VECTOR_TO_STRING_H

