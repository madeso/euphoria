#ifndef EUPHORIA_TEXTFILEPARSER_H
#define EUPHORIA_TEXTFILEPARSER_H

#include <string>

class TextFileParser {
 public:
  TextFileParser(const std::string& str);

  char PeekChar();

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
