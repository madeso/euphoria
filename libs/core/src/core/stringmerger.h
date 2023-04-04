#pragma once

#include <string_view>


namespace euphoria::core
{
    struct StringMerger
    {
        std::string_view separator;
        std::string_view final_separator;
        std::string_view empty;
        std::string_view start;
        std::string_view end;
        std::string_view before_each;
        std::string_view after_each;
        std::string_view final_after_each;

        [[nodiscard]] std::string
        merge(const std::vector<std::string>& strings) const;


        constexpr StringMerger&
        set_separator(const std::string_view& the_separator, const std::string_view& the_final_separator)
        {
            separator = the_separator;
            final_separator = the_final_separator;
            return *this;
        }

        constexpr StringMerger&
        set_separator(const std::string_view& both)
        {
            separator = both;
            final_separator = both;
            return *this;
        }

        constexpr StringMerger&
        set_empty(const std::string_view& v)
        {
            empty = v;
            return *this;
        }

        constexpr StringMerger&
        set_start_and_end(const std::string_view& both, const std::string_view& the_end)
        {
            start = both;
            end = the_end;
            return *this;
        }

        constexpr StringMerger&
        set_start_and_end(const std::string_view& v)
        {
            start = v;
            end = v;
            return *this;
        }

        constexpr StringMerger&
        set_before_each(const std::string_view& v)
        {
            before_each = v;
            return *this;
        }

        constexpr StringMerger&
        set_after_each(const std::string_view& both)
        {
            after_each = both;
            final_after_each = both;
            return *this;
        }

        constexpr StringMerger&
        set_after_each(const std::string_view& the_after, const std::string_view& the_final)
        {
            after_each = the_after;
            final_after_each = the_final;
            return *this;
        }
    };


    namespace string_mergers
    {
        constexpr StringMerger english_and = StringMerger{}.set_separator(", ", " and ").set_empty("<none>");
        constexpr StringMerger english_or = StringMerger{}.set_separator(", ", " or ").set_empty("<none>");
        constexpr StringMerger array = StringMerger{}.set_separator(", ").set_start_and_end("[", "]");
        constexpr StringMerger function_call = StringMerger{}.set_separator(", ").set_start_and_end("(", ")");
        constexpr StringMerger space = StringMerger{}.set_separator(" ");
        constexpr StringMerger comma = StringMerger{}.set_separator(", ");
    }
}

