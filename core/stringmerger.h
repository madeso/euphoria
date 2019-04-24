#ifndef EUPHORIA_STRINGMERGER_H
#define EUPHORIA_STRINGMERGER_H


#include <string>
#include <vector>
#include <map>

struct StringMerger
{
  ////////////////////////////////////////////
  // public api

  std::string
  Generate(const std::vector<std::string>& strings) const;


  ////////////////////////////////////////////
  // Named constructors

  static const StringMerger&
  EnglishAnd();

  static const StringMerger&
  EnglishOr();

  static const StringMerger&
  Array();

  static const StringMerger&
  FunctionCall();

  static const StringMerger&
  Space();

  static const StringMerger&
  Comma();


  ////////////////////////////////////////////
  // constructor functions

  StringMerger&
  SetSeparator(const std::string& separator, const std::string& final_separator);

  StringMerger&
  SetSeparator(const std::string& separator);

  StringMerger&
  SetEmpty(const std::string& empty);

  StringMerger&
  SetStartAndEnd(const std::string& start, const std::string& end);
  
  StringMerger&
  SetStartAndEnd(const std::string& same);

  StringMerger&
  SetBeforeEach(const std::string& before_each);

  StringMerger&
  SetAfterEach(const std::string& same);

  StringMerger&
  SetAfterEach(const std::string& after_each, const std::string& final_after_each);

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

#endif  // EUPHORIA_STRINGMERGER_H
