#pragma once

#include <string>


namespace euphoria::core
{


enum class MissingInAlphabet
{
    Ignore, PassThrough, Replace
};


std::string
CeasarCypher
(
    const std::string& input,
    int steps,
    const std::string& alphabet,
    bool case_sensitive,
    MissingInAlphabet missing_in_alphabet,
    char invalid
);


std::string Rot13(const std::string& input);


}

