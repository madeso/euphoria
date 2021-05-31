#pragma once

#include <string>


namespace euphoria::core
{


    enum class missing_in_alphabet
    {
        ignore, pass_through, replace
    };


    std::string
    ceasar_cypher
    (
        const std::string& input,
        int steps,
        const std::string& alphabet,
        bool case_sensitive,
        missing_in_alphabet missing_in_alphabet,
        char invalid
    );


    std::string
    rot13(const std::string& input);


}

