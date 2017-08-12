#ifndef EUPHORIA_TEXTFILEPARSER_H
#define EUPHORIA_TEXTFILEPARSER_H

#include <string>

bool
IsIdentStart(const char c);

class TextFileParser
{
 public:
  TextFileParser(const std::string& str);

  char
  PeekChar(unsigned int advance = 0);

  void
  AdvanceChar();
  char
  ReadChar();
  std::string
  ReadIdent();
  std::string
  ReadString();
  std::string
  ReadToEndOfLine();

  void
  SkipSpaces(bool include_newline);
  bool
  HasMore() const;

  unsigned int
  GetLine();
  unsigned int
  GetColumn();

 private:
  std::string  string_;
  unsigned int length_;
  unsigned int position_;

  unsigned int line_;
  unsigned int column_;
};


#endif  // EUPHORIA_TEXTFILEPARSER_H
