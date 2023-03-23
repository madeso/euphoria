#include "core/cypher.h"

#include "assert/assert.h"

#include "core/stringutils.h"
#include "core/range.h"
#include "core/stringbuilder.h"



namespace euphoria::core
{


std::string
get_ceasar_cypher
(
    const std::string& input,
    int steps,
    const std::string& alphabet,
    bool case_sensitive,
    ActionWhenMissingInAlphabet missing_in_alphabet,
    char invalid
)
{
    ASSERT(steps > 0); // todo(Gustav): add support for negative steps
    const auto alph = case_sensitive ? alphabet : to_lower(alphabet);
    auto ret = StringBuilder{};

    for(const auto c: input)
    {
        const bool is_upper = c >= 'A' && c <= 'Z';
        const auto fc = case_sensitive ? c : to_lower_char(c);

        const auto index = alph.find(fc);
        if(index != std::string::npos)
        {
            const auto new_index = (static_cast<int>(index) + steps) % static_cast<int>(alph.size());
            const auto new_char = alph[new_index];
            const auto transformed_char = case_sensitive == false && is_upper
                ? to_upper_char(new_char) : new_char;
            ret.add_char(transformed_char);
        }
        else
        {
            switch(missing_in_alphabet)
            {
            case ActionWhenMissingInAlphabet::ignore:
                continue;
            case ActionWhenMissingInAlphabet::pass_through:
                ret.add_char(c);
                break;
            case ActionWhenMissingInAlphabet::replace:
                ret.add_char(invalid);
                break;
            default:
                DIE("Unhandled case");
            }
        }
    }

    return ret.to_string();
}


std::string get_rot13(const std::string& input)
{
    return get_ceasar_cypher(input, 13, "abcdefghijklmnopqrstuvwxyz", false, ActionWhenMissingInAlphabet::pass_through, '?');
}


}
