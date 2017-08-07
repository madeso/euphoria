#ifndef EUPHORIA_STRINGMERGER_H
#define EUPHORIA_STRINGMERGER_H


#include <string>
#include <vector>
#include <map>

/** Util class for merging strings.
 */
class StringMerger {
public:
  /** English merger. Combines with a and.
  @returns the merger.
   */
  static const StringMerger& EnglishAnd();

  /** English merger. Combines with a or.
  @returns the merger.
   */
  static const StringMerger& EnglishOr();

  /** Array merger. Generate strings that looks like [a, b, c].
  @returns the merger.
   */
  static const StringMerger& Array();

  /** Sets the separator arguments.
  @param separator the separator to use.
  @param final_separator the separator to use at the last element.
  @returns this for easy chaining.
   */
  StringMerger& set_separator(const std::string& separator,
                              const std::string& final_separator);

  /** Sets the separator arguments.
  @param separator the separator to use.
  @returns this for easy chaining.
   */
  StringMerger& set_separator(const std::string& separator);

  /** Sets the empty string.
  This string is returned when a request is made on a a empty set.
  @param empty the empty string.
  @returns this for easy chaining.
   */
  StringMerger& set_empty(const std::string& empty);

  /** Sets the start and end text. This data is always added.
  @param start the start string.
  @param end the end string.
  @returns this for easy chaining.
   */
  StringMerger& SetStartEnd(const std::string& start, const std::string& end);

  /** Generate a string representation based on the current settings.
  @param strings the strings.
  @returns the combined string.
   */
  std::string Generate(const std::vector<std::string>& strings) const;

private:
  std::string separator_;
  std::string final_separator_;
  std::string empty_;
  std::string start_;
  std::string end_;
};

#endif //EUPHORIA_STRINGMERGER_H
