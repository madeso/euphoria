#include "core/stringutils.h"

#include <cassert>
#include <string>
#include <algorithm>
#include <cstring>
#include <vector>

std::string StripLastString(const std::string& str, char sep)
{
  auto result = str.find(sep);
  if(result == std::string::npos) return str;

  return str.substr(result);
}

std::string TrimRight(const std::string& stringToTrim,
                      const std::string& trimCharacters) {
  return std::string(stringToTrim)
      .erase(stringToTrim.find_last_not_of(trimCharacters) + 1);
}

std::string TrimLeft(const std::string& stringToTrim,
                     const std::string& trimCharacters) {
  return std::string(stringToTrim)
      .erase(0, stringToTrim.find_first_not_of(trimCharacters));
}

std::string Trim(const std::string& stringToTrim,
                 const std::string& trimCharacters) {
  return TrimRight(TrimLeft(stringToTrim, trimCharacters), trimCharacters);
}

bool StartsWith(const std::string& stringToTest, const std::string& start) {
  const std::string::size_type length = start.length();
  const std::string::size_type otherLength = stringToTest.length();
  if (otherLength < length) {
    return false;
  }
  const std::string actualStart = stringToTest.substr(0, length);
  return start == actualStart;
}

bool EndsWith(const std::string& stringToTest, const std::string& end) {
  const std::string::size_type length = end.length();
  const std::string::size_type otherLength = stringToTest.length();
  if (otherLength < length) {
    return false;
  }
  const std::string actualEnd =
      stringToTest.substr(otherLength - length, length);
  return end == actualEnd;
}

std::string ToLower(const std::string& string) {
  std::string result = string;
  std::transform(result.begin(), result.end(), result.begin(), tolower);
  return result;
}

void StringReplace(std::string* string, const std::string& toFind,
                   const std::string& toReplace) {
  std::size_t index = string->find(toFind);
  const std::size_t findLength = toFind.length();
  while (index != std::string::npos) {
    string->erase(index, findLength);
    string->insert(index, toReplace);
    index = string->find(toFind, index);
  }
}

const std::string StringReplace(const std::string& string,
                                const std::string& toFind,
                                const std::string& toReplace) {
  std::string temp = string;
  StringReplace(&temp, toFind, toReplace);
  return temp;
}

void Copy(char* dst, const std::string& src,
          const std::string::size_type& count) {
  strncpy(dst, src.c_str(), count - 1);
  dst[count - 1] = 0;
}

std::string RemoveFromEnd(const std::string& str, const std::string& end) {
  if (EndsWith(str, end)) {
    const auto new_length = str.length() - end.length();
    if (new_length == 0) return "";
    assert(new_length > 0);
    return str.substr(0, new_length);
  } else {
    return str;
  }
}
