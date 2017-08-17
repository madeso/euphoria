/** @file
String utility functions
 */

#ifndef EUPHORIA_STRINGUTILS_H_
#define EUPHORIA_STRINGUTILS_H_

#include <string>
#include <vector>
#include <map>

#include "core/str.h"

/** @defgroup string String utility functions.
@{
 */

/** Space characters.
@returns the space characters.
 */
const std::string kSpaceCharacters = " \n\r\t";

// for a hello.dog and . gets hello
std::string
StripLastString(const std::string& str, char sep);

/** Remove characters from the right, stops at a invalid character.
@param string_to_trim the string to remove characters from.
@param trim_characters the characters to remove.
@returns the trimmed string.
 */
std::string
TrimRight(const std::string& string_to_trim,
          const std::string& trim_characters = kSpaceCharacters);

/** Remove characters from the left, stops at invalid character.
@param string_to_trim the string to remove characters from.
@param trim_characters the characters to remove.
@returns the trimmed string.
 */
std::string
TrimLeft(const std::string& string_to_trim,
         const std::string& trim_characters = kSpaceCharacters);

/** Remove characters from both the start and the end.
@param string_to_trim the string to remove characters from.
@param trim_characters the characters to remove.
@returns the trimmed string.
 */
std::string
Trim(const std::string& string_to_trim,
     const std::string& trim_characters = kSpaceCharacters);

/** Tests if a string starts with another string.
@param string_to_test the string to test.
@param start the start of the string.
@returns true if the start match, false if not.
 */
bool
StartsWith(const std::string& string_to_test, const std::string& start);

/** Tests if a string ends with another string.
@param string_to_test the string to test.
@param end the end of the string.
@returns true if the end match, false if not.
 */
bool
EndsWith(const std::string& string_to_test, const std::string& end);

/** Generate a string containing only lower characters.
@param str the string to lower.
@returns the lowered string.
 */
std::string
ToLower(const std::string& str);

/** Replace all occurrences in a string.
@param string the source string.
@param to_find the string to find.
@param to_replace the string to replace it.
 */
void
StringReplace(std::string* string, const std::string& to_find,
              const std::string& to_replace);

/** Replace all occurrences in a string.
@param string the source string.
@param to_find the string to find.
@param to_replace the string to replace it.
@returns the modified string.
 */
const std::string
StringReplace(const std::string& string, const std::string& to_find,
              const std::string& to_replace);

/** Copy a string to a character buffer, adding null terminator at the end.
@param dst the destination buffer.
@param src the source string.
@param count the maximum number of characters to copy (including null
terminator)
 */
void
Copy(char* dst, const std::string& src, const std::string::size_type& count);

std::string
RemoveFromEnd(const std::string& str, const std::string& end);

template <typename Key, typename Value>
std::vector<std::string>
MapToStringVector(const std::map<Key, Value>& map)
{
  std::vector<std::string> ret;
  for(const auto& m : map)
  {
    const std::string str = Str() << "(" << m.first << ":" << m.second << ")";
    ret.emplace_back(str);
  }
  return ret;
}

/** @} */

#endif  // EUPHORIA_STRINGUTILS_H_
