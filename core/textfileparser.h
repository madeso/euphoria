#ifndef EUPHORIA_TEXTFILEPARSER_H
#define EUPHORIA_TEXTFILEPARSER_H

#include <string>

bool IsIdentStart(const char c);

class TextFileParser {
 public:
  TextFileParser(const std::string& str);

  char PeekChar(unsigned int advance=0);

  void AdvanceChar();
  char ReadChar();
  std::string ReadIdent();
  std::string ReadString();
  std::string ReadToEndOfLine();

  void SkipSpaces(bool include_newline);
  bool HasMore() const;

 private:
  std::string string_;
  unsigned int length_;
  unsigned int position_;
};


#endif //EUPHORIA_TEXTFILEPARSER_H
