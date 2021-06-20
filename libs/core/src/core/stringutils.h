/** @file
String utility functions
 */

#pragma once

#include <string>
#include <vector>
#include <map>

#include "core/str.h"

namespace euphoria::core
{
    /** @defgroup string String utility functions.
    @{
    */


   bool
   is_number(char b);


    /** Space characters.
    @returns the space characters.
    */
    const std::string kSpaceCharacters = " \n\r\t";


    // for a hello.dog and . gets hello
    std::string
    strip_last_string(const std::string& str, char sep);


    std::pair<std::string, std::string>
    last_strings(const std::string& str, char sep);


    std::string
    first_chars(const std::string& str, std::size_t count);

    std::string
    first_chars_with_ellipsis(const std::string& str, unsigned int count = 10);


    /** Remove characters from the right, stops at a invalid character.
    @param string_to_trim the string to remove characters from.
    @param trim_characters the characters to remove.
    @returns the trimmed string.
    */
    std::string
    trim_right(const std::string& string_to_trim, const std::string& trim_characters = kSpaceCharacters);


    /** Remove characters from the left, stops at invalid character.
    @param string_to_trim the string to remove characters from.
    @param trim_characters the characters to remove.
    @returns the trimmed string.
    */
    std::string
    trim_left(const std::string& string_to_trim, const std::string& trim_characters = kSpaceCharacters);


    /** Remove characters from both the start and the end.
    @param string_to_trim the string to remove characters from.
    @param trim_characters the characters to remove.
    @returns the trimmed string.
    */
    std::string
    trim(const std::string& string_to_trim, const std::string& trim_characters = kSpaceCharacters);


    /** Tests if a string starts with another string.
    @param string_to_test the string to test.
    @param start the start of the string.
    @returns true if the start match, false if not.
    */
    bool
    starts_with(const std::string& string_to_test, const std::string& start);


    /** Tests if a string ends with another string.
    @param string_to_test the string to test.
    @param end the end of the string.
    @returns true if the end match, false if not.
    */
    bool
    ends_with(const std::string& string_to_test, const std::string& end);


    char
    to_lower_char(char b);

    char
    to_upper_char(char b);


    /** Generate a string containing only lower characters.
    @param str the string to lower.
    @returns the lowered string.
    */
    std::string
    to_lower(const std::string& str);


    std::vector<std::string>
    to_lower(const std::vector<std::string>& str);


    std::string
    to_upper(const std::string& str);


    enum class char_to_string_style
    {
        smart, include_hex
    };


    std::string
    char_to_string(char c, char_to_string_style style=char_to_string_style::smart);


    std::string::size_type
    find_first_index_of_mismatch(const std::string& lhs, const std::string& rhs);


    /** Replace all occurrences in a string.
    @param string the source string.
    @param to_find the string to find.
    @param to_replace the string to replace it.
    */
    void
    replace_all(std::string* string, const std::string& to_find, const std::string& to_replace);


    /** Replace all occurrences in a string.
    @param string the source string.
    @param to_find the string to find.
    @param to_replace the string to replace it.
    @returns the modified string.
    */
    std::string
    replace_all(const std::string& string, const std::string& to_find, const std::string& to_replace);


    std::string
    replace_with_character(const std::string& string, const std::string& to_find, char to_replace);


    /** Copy a string to a character buffer, adding null terminator at the end.
    @param dst the destination buffer.
    @param src the source string.
    @param count the maximum number of characters to copy (including null
    terminator)
    */
    void
    copy(char* dst, const std::string& src, const std::string::size_type& count);

    std::string
    remove_from_end(const std::string& str, const std::string& end);


    template <typename Key, typename Value>
    std::vector<std::string>
    to_string_vector(const std::map<Key, Value>& map)
    {
        std::vector<std::string> ret;
        for(const auto& m: map)
        {
            const std::string str = string_builder()
                                    << "(" << m.first << ":" << m.second << ")";
            ret.emplace_back(str);
        }
        return ret;
    }


    template <typename Key, typename Value>
    std::vector<std::string>
    key_to_string_vector(const std::map<Key, Value>& map)
    {
        std::vector<std::string> ret;
        for(const auto& m: map)
        {
            const std::string str = string_builder() << m.first;
            ret.emplace_back(str);
        }
        return ret;
    }


    template <typename T>
    std::vector<std::string>
    to_string_vector(const std::vector<T>& data)
    {
        std::vector<std::string> ret;
        ret.reserve(data.size());
        for(const auto& d: data)
        {
            const std::string str = string_builder() << d;
            ret.emplace_back(str);
        }
        return ret;
    }


    template <typename T, typename C>
    std::vector<std::string>
    to_string_vector(const std::vector<T>& data, C callback)
    {
        std::vector<std::string> ret;
        ret.reserve(data.size());
        for(const auto& d: data)
        {
            ret.emplace_back(callback(d));
        }
        return ret;
    }


    // remove all characters in ch from str
    std::string
    strip(const std::string& str, const std::string& ch);


    // remove all characters in ch except the first one in a chain from str
    std::string
    remove_consecutive(const std::string& str, const std::string& ch);


    std::vector<std::string>
    split(const std::string& string, char delim);


    std::vector<std::string>
    split_on_spaces(const std::string& string);


    std::string
    optional_string(bool b, const std::string& str);


    // http://stereopsis.com/strcmp4humans.html
    int
    string_compare(const std::string& lhs, const std::string& rhs);
    /** @} */

}

