#include "core/commandlineparser.h"

#include "core/stringutils.h"

namespace
{
  bool
  IsSpace(char c)
  {
    switch(c)
    {
      case '\t':
      case ' ':
        return true;
      default:
        return false;
    }
  }

  bool
  IsQuote(char c)
  {
    switch(c)
    {
      case '\"':
      case '\'':
        return true;
      default:
        return false;
    }
  }

  char
  HandleEscapeCharacter(char c)
  {
    switch(c)
    {
      case 'n':
        return '\n';
      case 't':
        return '\t';
      default:
        return c;
    }
  }
}

std::vector<std::string>
ParseCommandLine(const std::string& str)
{
  std::vector<std::string> ret;

  bool               escape                   = false;
  char               current_string_character = 0;
  std::ostringstream buffer;

  for(char c : str)
  {
    if(escape)
    {
      buffer << HandleEscapeCharacter(c);
      escape = false;
    }
    else
    {
      if(c == '\\')
      {
        escape = true;
      }
      else
      {
        if(current_string_character != 0)
        {
          if(c == current_string_character)
          {
            current_string_character = 0;
            ret.emplace_back(buffer.str());
            buffer.str("");
          }
          else
          {
            buffer << c;
          }
        }
        else
        {
          // not within string
          if(IsQuote(c) || IsSpace(c))
          {
            if(IsQuote(c))
            {
              current_string_character = c;
            }
            const auto b = buffer.str();
            if(!b.empty())
            {
              ret.emplace_back(b);
              buffer.str("");
            }
          }
          else
          {
            // neither quote nor space
            buffer << c;
          }
        }
      }
    }
  }

  const auto rest = buffer.str();
  if(!rest.empty() || current_string_character != 0)
  {
    ret.emplace_back(rest);
  }

  return ret;
}
