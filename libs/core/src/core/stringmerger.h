#pragma once

#include <string_view>
#include <vector>
#include <string_view>


namespace euphoria::core
{
    struct string_merger
    {
        [[nodiscard]] std::string
        merge(const std::vector<std::string>& strings) const;


        constexpr string_merger&
        set_separator(const std::string_view& the_separator, const std::string_view& the_final_separator)
        {
            separator = the_separator;
            final_separator = the_final_separator;
            return *this;
        }

        constexpr string_merger&
        set_separator(const std::string_view& both)
        {
            separator = both;
            final_separator = both;
            return *this;
        }

        constexpr string_merger&
        set_empty(const std::string_view& v)
        {
            empty = v;
            return *this;
        }

        constexpr string_merger&
        set_start_and_end(const std::string_view& both, const std::string_view& the_end)
        {
            start = both;
            end = the_end;
            return *this;
        }

        constexpr string_merger&
        set_start_and_end(const std::string_view& v)
        {
            start = v;
            end = v;
            return *this;
        }

        constexpr string_merger&
        set_before_each(const std::string_view& v)
        {
            before_each = v;
            return *this;
        }

        constexpr string_merger&
        set_after_each(const std::string_view& both)
        {
            after_each = both;
            final_after_each = both;
            return *this;
        }

        constexpr string_merger&
        set_after_each(const std::string_view& the_after, const std::string_view& the_final)
        {
            after_each = the_after;
            final_after_each = the_final;
            return *this;
        }

        std::string_view separator;
        std::string_view final_separator;
        std::string_view empty;
        std::string_view start;
        std::string_view end;
        std::string_view before_each;
        std::string_view after_each;
        std::string_view final_after_each;
    };

    namespace string_mergers
    {
        constexpr string_merger english_and = string_merger{}.set_separator(", ", " and ").set_empty("<none>");
        constexpr string_merger english_or = string_merger{}.set_separator(", ", " or ").set_empty("<none>");
        constexpr string_merger array = string_merger{}.set_separator(", ").set_start_and_end("[", "]");
        constexpr string_merger function_call = string_merger{}.set_separator(", ").set_start_and_end("(", ")");
        constexpr string_merger space = string_merger{}.set_separator(" ");
        constexpr string_merger comma = string_merger{}.set_separator(", ");
    }
}

