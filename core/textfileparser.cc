#include "core/textfileparser.h"

#include <sstream>

#include "core/numeric.h"

TextFileParser::TextFileParser(const std::string& str)
    : string_(str)
    ,length_(str.length())
    ,position_(0)
{ }

char TextFileParser::PeekChar(unsigned int advance) {
  const auto p = position_ + advance;
  if(p < length_) {
    return string_[p];
  }
  else {
    return 0;
  }
}

char TextFileParser::ReadChar() {
  const char r = PeekChar();
  AdvanceChar();
  return r;
}

void TextFileParser::AdvanceChar() {
  ++position_;
}

namespace // local
{
bool IsIdentChar(char c, bool first_char) {
  if(IsWithinInclusivei('a', c, 'z')) return true;
  if(IsWithinInclusivei('A', c, 'Z')) return true;
  if(IsWithinInclusivei('0', c, '9')) return !first_char;
  if(c == '_') return true;

  return false;
}
}

std::string TextFileParser::ReadIdent() {
  std::ostringstream ss;
  bool first = true;
  while(IsIdentChar(PeekChar(), first)) {
    first = false;
    ss << ReadChar();
  }
  return ss.str();
}

std::string TextFileParser::ReadString() {
  std::ostringstream ss;
  const char quote = '\"';
  if( PeekChar() != quote) return "";
  AdvanceChar(); // skip " char
  while(PeekChar() != quote) {
    const char c = ReadChar();
    if( c == '\\') {
      const char nc = ReadChar();
      if( nc == '\\') {
        ss << '\\';
      }
      else if( nc == 't') {
        ss << '\t';
      }
      else if( nc == '\n') {
        ss << '\n';
      }
      else if( nc == '\"') {
        ss << '\"';
      }
      else {
        ss << nc;
      }
    }
    else {
      ss << c;
    }
  }
  const char c = ReadChar();
  if( c != quote) {
    return "";
  }
  return ss.str();
}

namespace  // local
{
bool IsNewline(const char c) {
  if( c == '\n') return true;
  else if( c == '\r') return true;
  else return false;
}
}

std::string TextFileParser::ReadToEndOfLine() {
  std::ostringstream ss;
  while(IsNewline(PeekChar()) == false) {
    const char c = ReadChar();
    if(c == 0) return ss.str();
    ss << c;
  }
  AdvanceChar(); // skip the newline
  return ss.str();
}

namespace // local
{
bool IsSpaceCharacter(char c, bool include_newline){
  switch(c) {
    case ' ':
    case '\t':
      return true;
    case '\n':
    case '\r':
      return include_newline;
    default:
      return false;
  }
}
}

void TextFileParser::SkipSpaces(bool include_newline) {
  while( IsSpaceCharacter(PeekChar(), include_newline)) {
    ReadChar();
  }
}

bool TextFileParser::HasMore() const {
  return position_ < length_;
}
