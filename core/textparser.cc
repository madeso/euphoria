#include "core/textparser.h"

#include "core/log.h"

#include <sstream>

LOG_SPECIFY_DEFAULT_LOGGER("core.textparser")

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace textparser
{
  TextNode::TextNode(const std::string& t)
      : text(t)
  {
  }

  void
  TextNode::Visit(Visitor* visitor) const
  {
    visitor->OnText(text);
  }

  ImageNode::ImageNode(const std::string& t)
      : image(t)
  {
  }

  void
  ImageNode::Visit(Visitor* visitor) const
  {
    visitor->OnImage(image);
  }

  BeginEndNode::BeginEndNode(bool b)
      : begin(b)
  {
  }

  void
  BeginEndNode::Visit(Visitor* visitor) const
  {
    if(begin)
    {
      visitor->OnBegin();
    }
    else
    {
      visitor->OnEnd();
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace textparser
{
  void
  VisitorDebugString::OnText(const std::string& text)
  {
    ss << "{text " << text << "}";
  }

  void
  VisitorDebugString::OnImage(const std::string& img)
  {
    ss << "{image " << img << "}";
  }

  void
  VisitorDebugString::OnBegin()
  {
    ss << "{begin}";
  }

  void
  VisitorDebugString::OnEnd()
  {
    ss << "{end}";
  }

  std::string
  VisitorDebugString::Visit(ParsedText* visitor)
  {
    VisitorDebugString str;
    visitor->Visit(&str);
    return str.ss.str();
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
ParsedText::Clear()
{
  nodes.resize(0);
}

void
ParsedText::AddText(const std::string& str)
{
  nodes.emplace_back(std::make_shared<textparser::TextNode>(str));
}

void
ParsedText::AddImage(const std::string& img)
{
  nodes.emplace_back(std::make_shared<textparser::ImageNode>(img));
}

void
ParsedText::AddBegin()
{
  nodes.emplace_back(std::make_shared<textparser::BeginEndNode>(true));
}

void
ParsedText::AddEnd()
{
  nodes.emplace_back(std::make_shared<textparser::BeginEndNode>(false));
}

void
ParsedText::CreateText(const std::string& str)
{
  Clear();
  AddText(str);
}

namespace
{
  enum class State
  {
    TEXT,
    IMAGE
  };

  struct Parser
  {
    ParsedText* nodes = nullptr;

    State             state  = State::TEXT;
    bool              escape = false;
    std::stringstream buff;
    bool              ok = true;

    void
    Close()
    {
      const std::string data = buff.str();
      buff.str("");
      switch(state)
      {
        case State::TEXT:
          if(!data.empty())
          {
            nodes->AddText(data);
          }
          break;
        case State::IMAGE:
          if(!data.empty())
          {
            nodes->AddImage(data);
          }
          else
          {
            LOG_ERROR("Tried to create a empty @image");
            ok = false;
          }
          break;
      }
    }

    void
    OnChar(char c)
    {
      switch(state)
      {
        case State::TEXT:
          if(escape)
          {
            buff << c;
            escape = false;
          }
          else
          {
            switch(c)
            {
              case '@':
                Close();
                state = State::IMAGE;
                break;
              case '{':
                Close();
                nodes->AddBegin();
                break;
              case '}':
                Close();
                nodes->AddEnd();
                break;
              case '\\':
                escape = true;
                break;
              default:
                buff << c;
                break;
            }
          }
          break;
        case State::IMAGE:
          if(('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '-' ||
             c == '_')
          {
            buff << c;
          }
          else
          {
            Close();
            state = State::TEXT;
            if(c == ' ')
            {
              // nop
            }
            else
            {
              OnChar(c);
              return;
            }
          }
          break;
      }
    }
  };
}

bool
ParsedText::CreateParse(const std::string& str)
{
  nodes.resize(0);
  Parser parser;
  parser.nodes = this;

  for(char c : str)
  {
    parser.OnChar(c);
    if(!parser.ok)
    {
      return false;
    }
  }

  parser.Close();
  if(!parser.ok)
  {
    return false;
  }
  if(parser.escape)
  {
    LOG_ERROR("Ending with escape");
    return false;
  }
  return true;
}

ParsedText
ParsedText::FromText(const std::string& str)
{
  ParsedText text;
  text.CreateText(str);
  return text;
}

void
ParsedText::Visit(textparser::Visitor* visitor)
{
  for(auto node : nodes)
  {
    node->Visit(visitor);
  }
}

void
ParsedText::Visit(textparser::Visitor* visitor) const
{
  for(auto node : nodes)
  {
    node->Visit(visitor);
  }
}
