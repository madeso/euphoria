#include "core/cypher.h"

#include <sstream>

#include "core/stringutils.h"
#include "core/range.h"
#include "assert/assert.h"


namespace euphoria::core
{


std::string
ceasar_cypher
(
    const std::string& input,
    int steps,
    const std::string& alphabet,
    bool case_sensitive,
    ActionWhenMissingInAlphabet missing_in_alphabet,
    char invalid
)
{
    ASSERT(steps > 0); // todo(Gustav): ass support for negative steps
    const auto alph = case_sensitive ? alphabet : to_lower(alphabet);
    std::ostringstream ret;

    for(const auto c: input)
    {
        const bool is_upper = c >= 'A' && c <= 'Z';
        const auto fc = case_sensitive ? c : to_lower_char(c);

        const auto index = alph.find(fc);
        if(index != std::string::npos)
        {
            const auto new_index = (static_cast<int>(index) + steps) % static_cast<int>(alph.size());
            const auto new_char = alph[new_index];
            const auto transformed_char = case_sensitive == false && is_upper ? to_upper_char(new_char) : new_char;
            ret << transformed_char;
        }
        else
        {
            switch(missing_in_alphabet)
            {
            case ActionWhenMissingInAlphabet::ignore:
                continue;
            case ActionWhenMissingInAlphabet::pass_through:
                ret << c;
                break;
            case ActionWhenMissingInAlphabet::replace:
                ret << invalid;
                break;
            default:
                DIE("Unhandled case");
            }
        }
    }

    return ret.str();
}


std::string rot13(const std::string& input)
{
    return ceasar_cypher(input, 13, "abcdefghijklmnopqrstuvwxyz", false, ActionWhenMissingInAlphabet::pass_through, '?');
}


}
