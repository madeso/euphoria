#include "core/stringmerger.h"

#include "core/str.h"

#include <string>
#include <sstream>
#include <vector>

namespace euphoria::core
{
    std::string
    StringMerger::Generate(const std::vector<std::string>& strings) const
    {
        if (strings.empty())
        {
            return Str() << start << empty << end;
        }

        std::ostringstream ss;

        ss << start;

        const auto count = strings.size();
        for (std::vector<std::string>::size_type index = 0; index < count;
             ++index)
        {
            ss << before_each << strings[index];

            if (index == count - 1)
            {
                ss << final_after_each;
            }
            else
            {
                ss << after_each;
            }

            if (count != index + 1)
            {  // if this item isn't the last one in the list
                if (count == index + 2)
                {
                    ss << final_separator;
                }
                else
                {
                    ss << separator;
                }
            }
        }

        ss << end;

        return ss.str();
    }

    const StringMerger&
    StringMerger::EnglishAnd()
    {
        static const StringMerger EnglishAnd
                = StringMerger().Separator(", ", " and ").Empty("<none>");
        return EnglishAnd;
    }

    const StringMerger&
    StringMerger::EnglishOr()
    {
        static const StringMerger EnglishOr
                = StringMerger().Separator(", ", " or ").Empty("<none>");
        return EnglishOr;
    }

    const StringMerger&
    StringMerger::Array()
    {
        static const StringMerger Array
                = StringMerger().Separator(", ").StartAndEnd("[", "]");
        return Array;
    }

    const StringMerger&
    StringMerger::FunctionCall()
    {
        static const StringMerger FunctionCall
                = StringMerger().Separator(", ").StartAndEnd("(", ")");
        return FunctionCall;
    }

    const StringMerger&
    StringMerger::Space()
    {
        static const StringMerger Space = StringMerger().Separator(" ");
        return Space;
    }

    const StringMerger&
    StringMerger::Comma()
    {
        static const StringMerger Comma = StringMerger().Separator(", ");
        return Comma;
    }

    StringMerger&
    StringMerger::Separator(
            const std::string& separator,
            const std::string& final_separator)
    {
        this->separator       = separator;
        this->final_separator = final_separator;
        return *this;
    }

    StringMerger&
    StringMerger::Separator(const std::string& separator)
    {
        this->separator       = separator;
        this->final_separator = separator;
        return *this;
    }

    StringMerger&
    StringMerger::Empty(const std::string& empty)
    {
        this->empty = empty;
        return *this;
    }

    StringMerger&
    StringMerger::StartAndEnd(const std::string& start, const std::string& end)
    {
        this->start = start;
        this->end   = end;
        return *this;
    }

    StringMerger&
    StringMerger::StartAndEnd(const std::string& same)
    {
        this->start = same;
        this->end   = same;
        return *this;
    }

    StringMerger&
    StringMerger::BeforeEach(const std::string& before_each)
    {
        this->before_each = before_each;
        return *this;
    }

    StringMerger&
    StringMerger::AfterEach(const std::string& same)
    {
        this->after_each       = same;
        this->final_after_each = same;
        return *this;
    }

    StringMerger&
    StringMerger::AfterEach(
            const std::string& after_each,
            const std::string& final_after_each)
    {
        this->after_each       = after_each;
        this->final_after_each = final_after_each;
        return *this;
    }

}  // namespace euphoria::core
