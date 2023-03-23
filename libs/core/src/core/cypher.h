#pragma once




namespace euphoria::core
{


    enum class ActionWhenMissingInAlphabet
    {
        ignore, pass_through, replace
    };


    std::string
    get_ceasar_cypher
    (
        const std::string& input,
        int steps,
        const std::string& alphabet,
        bool case_sensitive,
        ActionWhenMissingInAlphabet missing_in_alphabet,
        char invalid
    );


    std::string
    get_rot13(const std::string& input);


}

