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
        if(strings.empty())
        {
            return Str() << start << empty << end;
        }

        std::ostringstream ss;

        ss << start;

        const auto count = strings.size();
        for(std::vector<std::string>::size_type index = 0; index < count; ++index)
        {
            ss << before_each << strings[index];

            if(index == count - 1)
            {
                ss << final_after_each;
            }
            else
            {
                ss << after_each;
            }

            if(count != index + 1)
            { // if this item isn't the last one in the list
                if(count == index + 2)
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

    StringMerger
    StringMerger::EnglishAnd()
    {
        return StringMerger().Separator(", ", " and ").Empty("<none>");
    }

    StringMerger
    StringMerger::EnglishOr()
    {
        return StringMerger().Separator(", ", " or ").Empty("<none>");
    }

    StringMerger
    StringMerger::Array()
    {
        return StringMerger().Separator(", ").StartAndEnd("[", "]");
    }

    StringMerger
    StringMerger::FunctionCall()
    {
        return StringMerger().Separator(", ").StartAndEnd("(", ")");
    }

    StringMerger
    StringMerger::Space()
    {
        return StringMerger().Separator(" ");
    }

    StringMerger
    StringMerger::Comma()
    {
        return StringMerger().Separator(", ");
    }

    StringMerger&
    StringMerger::Separator(const std::string& the_separator, const std::string& the_final_separator)
    {
        this->separator = the_separator;
        this->final_separator = the_final_separator;
        return *this;
    }

    StringMerger&
    StringMerger::Separator(const std::string& the_separator)
    {
        this->separator = the_separator;
        this->final_separator = the_separator;
        return *this;
    }

    StringMerger&
    StringMerger::Empty(const std::string& the_empty)
    {
        this->empty = the_empty;
        return *this;
    }

    StringMerger&
    StringMerger::StartAndEnd(const std::string& the_start, const std::string& the_end)
    {
        this->start = the_start;
        this->end = the_end;
        return *this;
    }

    StringMerger&
    StringMerger::StartAndEnd(const std::string& start_and_end)
    {
        this->start = start_and_end;
        this->end = start_and_end;
        return *this;
    }

    StringMerger&
    StringMerger::BeforeEach(const std::string& the_before_each)
    {
        this->before_each = the_before_each;
        return *this;
    }

    StringMerger&
    StringMerger::AfterEach(const std::string& after_each_and_final)
    {
        this->after_each = after_each_and_final;
        this->final_after_each = after_each_and_final;
        return *this;
    }

    StringMerger&
    StringMerger::AfterEach(const std::string& the_after_each, const std::string& the_final_after_each)
    {
        this->after_each = the_after_each;
        this->final_after_each = the_final_after_each;
        return *this;
    }

}
