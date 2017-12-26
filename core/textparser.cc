#include "core/textparser.h"

#include "core/log.h"

#include <sstream>

LOG_SPECIFY_DEFAULT_LOGGER("core.textparser")

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace textparser {
  TextNode::TextNode(const std::string &t) : text(t) {}

  void TextNode::Visit(Visitor *visitor) { visitor->OnText(this); }

  ImageNode::ImageNode(const std::string &t) : image(t) {}

  void ImageNode::Visit(Visitor *visitor) { visitor->OnImage(this); }

  VarNode::VarNode(const std::string &t) : var(t) {}

  void VarNode::Visit(Visitor *visitor) { visitor->OnVar(this); }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace textparser
{
  void VisitorDebugString::OnText(TextNode* text)
  {
    ss << "{text " << text->text << "}";
  }

  void VisitorDebugString::OnImage(ImageNode* img)
  {
    ss << "{image " << img->image << "}";
  }

  void VisitorDebugString::OnVar(VarNode* var)
  {
    ss << "{var " << var->var << "}";
  }

  std::string VisitorDebugString::Visit(TextParser* visitor)
  {
    VisitorDebugString str;
    visitor->Visit(&str);
    return str.ss.str();
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace textparser
{
  std::shared_ptr<TextNode> NodeCreatorBasic::CreateTextNode(const std::string& text)
  {
    return std::make_shared<TextNode>(text);
  }

  std::shared_ptr<ImageNode> NodeCreatorBasic::CreateImageNode(const std::string& image)
  {
    return std::make_shared<ImageNode>(image);
  }

  std::shared_ptr<VarNode> NodeCreatorBasic::CreateVarNode(const std::string& var)
  {
    return std::make_shared<VarNode>(var);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void TextParser::CreateText(const std::string& str, textparser::NodeCreator* creator)
{
  nodes.resize(0);
  nodes.emplace_back( creator->CreateTextNode(str) );
}

namespace
{
  enum class State {
    TEXT, IMAGE, VAR
  };

  struct Parser
  {
    std::vector<std::shared_ptr<textparser::Node>>* nodes = nullptr;
    textparser::NodeCreator* creator = nullptr;

    State state = State::TEXT;
    bool escape = false;
    std::stringstream buff;
    bool ok = true;

    void Close()
    {
      const std::string data = buff.str();
      buff.str("");
      switch (state)
      {
        case State::TEXT:
          if(!data.empty())
          {
            nodes->emplace_back(creator->CreateTextNode(data));
          }
          break;
        case State::IMAGE:
          if(!data.empty())
          {
            nodes->emplace_back(creator->CreateImageNode(data));
          }
          else
          {
            LOG_ERROR("Tried to create a empty @image");
            ok = false;
          }
          break;
        case State::VAR:
          if(!data.empty())
          {
            nodes->emplace_back(creator->CreateVarNode(data));
          }
          else
          {
            LOG_ERROR("Tried to create a empty {var}");
            ok = false;
          }
          break;
      }
    }

    void OnChar(char c)
    {
      switch (state)
      {
        case State::TEXT:
          if(escape)
          {
            buff << c;
            escape = false;
          }
          else
          {
            switch (c)
            {
              case '@':
                Close();
                state = State::IMAGE;
                break;
              case '{':
                Close();
                state = State::VAR;
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
          if( ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '-'  || c == '_' )
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
        case State::VAR:
          if(c != '}')
          {
            buff << c;
          }
          else
          {
            Close();
            state = State::TEXT;
          }
          break;
      }
    }
  };
}

bool TextParser::CreateParse(const std::string& str, textparser::NodeCreator* creator)
{
  nodes.resize(0);
  Parser parser;
  parser.creator = creator;
  parser.nodes = &nodes;

  for(char c : str)
  {
    parser.OnChar(c);
    if(!parser.ok) return false;
  }

  parser.Close();
  if(!parser.ok) return false;
  if(parser.escape)
  {
    LOG_ERROR("Ending with escape");
    return false;
  }
  return true;
}

void TextParser::Visit(textparser::Visitor *visitor)
{
  for(auto node : nodes)
  {
    node->Visit(visitor);
  }
}
