#pragma once

#include <string>
#include <vector>
#include <map>


namespace euphoria::core
{
    struct StringMerger
    {
        ////////////////////////////////////////////
        // public api

        [[nodiscard]] std::string
        Generate(const std::vector<std::string>& strings) const;


        ////////////////////////////////////////////
        // Named constructors

        static StringMerger
        EnglishAnd();

        static StringMerger
        EnglishOr();

        static StringMerger
        Array();

        static StringMerger
        FunctionCall();

        static StringMerger
        Space();

        static StringMerger
        Comma();


        ////////////////////////////////////////////
        // constructor functions

        StringMerger&
        Separator(const std::string& separator, const std::string& final_separator);

        StringMerger&
        Separator(const std::string& separator);

        StringMerger&
        Empty(const std::string& empty);

        StringMerger&
        StartAndEnd(const std::string& start, const std::string& end);

        StringMerger&
        StartAndEnd(const std::string& same);

        StringMerger&
        BeforeEach(const std::string& before_each);

        StringMerger&
        AfterEach(const std::string& same);

        StringMerger&
        AfterEach(const std::string& after_each, const std::string& final_after_each);


        ////////////////////////////////////////////
        // member variables
        std::string separator;
        std::string final_separator;
        std::string empty;
        std::string start;
        std::string end;
        std::string before_each;
        std::string after_each;
        std::string final_after_each;
    };

}  // namespace euphoria::core
