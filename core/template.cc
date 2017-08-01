#include "core/template.h"

#include <vector>
#include <sstream>
#include "core/textfileparser.h"
#include "core/assert.h"
#include "core/str.h"

////////////////////////////////////////////////////////////////////////////////

Defines::Defines()
{}

bool Defines::IsDefined(const std::string& name) const
{
  const auto found = values_.find(name);
  return found != values_.end();
}

std::string Defines::GetValue(const std::string& name) const
{
  const auto found = values_.find(name);
  if(found == values_.end() ) {
    return "";
  }

  return found->second;
}

void Defines::Undefine(const std::string& name) {
  values_.erase(name);
}

void Defines::Define(const std::string& name, const std::string& value) {
  values_[name] = value;
}

////////////////////////////////////////////////////////////////////////////////

TemplateError::TemplateError()
{}

bool TemplateError::HasErrors() const
{
  return errors_.empty() == false;
}

bool TemplateError::AddError(const std::string& file, int line, int column, const std::string& error) {
  const std::string message = Str() << file << ":" << line << ":" << " " << error;
  errors_.push_back(message);
}

std::string TemplateError::GetCombinedErrors() const {
  std::ostringstream ss;

  for(const auto& mess : errors_) {
    ss << mess << "\n";
  }

  return ss.str();
}

////////////////////////////////////////////////////////////////////////////////

class TemplateNode {
 public:
  TemplateNode() {}
  virtual ~TemplateNode() {}

  virtual void Eval(const Defines& defines, std::ostringstream* out) = 0;
};

// -----------------------------------------------------------------------------

class TemplateNodeString : public TemplateNode {
 public:
  TemplateNodeString(const std::string& text)
      : text_(text) {}
  void Eval(const Defines& defines, std::ostringstream* out) override
  {
    Assert(out);
    *out << text_;
  }

 private:
  std::string text_;
};

// -----------------------------------------------------------------------------

class TemplateNodeList : public TemplateNode {
 public:
  TemplateNodeList()
  { }

  void Eval(const Defines& defines, std::ostringstream* out) override
  {
    for(auto node : nodes_) {
      node->Eval(defines, out);
    }
  }

  void Add(std::shared_ptr<TemplateNode> node) {
    nodes_.push_back(node);
  }

 private:
  std::vector<std::shared_ptr<TemplateNode>> nodes_;
};

// -----------------------------------------------------------------------------

class TemplateNodeIfdef : public TemplateNode {
 public:
  TemplateNodeIfdef(const std::string& name, std::shared_ptr<TemplateNode> node)
      : name_(name)
      , node_(node)
  { }

  void Eval(const Defines& defines, std::ostringstream* out) override
  {
    if(defines.IsDefined(name_)) {
      node_->Eval(defines, out);
    }
  }

 private:
  std::string name_;
  std::shared_ptr<TemplateNode> node_;
};

// -----------------------------------------------------------------------------

class TemplateNodeEval : public TemplateNode {
 public:
  TemplateNodeEval(const std::string& name)
      : name_(name)
  { }

  void Eval(const Defines& defines, std::ostringstream* out) override
  {
    Assert(out);
    *out << defines.GetValue(name_);
  }

 private:
  std::string name_;
};

////////////////////////////////////////////////////////////////////////////////

enum class LexType {
  TEXT, IFDEF, EVAL, IDENT, END, END_OF_FILE
};

class Lex {
 public:
  Lex(LexType t, const std::string& v = "")
      : type(t), value(v)
  {}
  LexType type;
  std::string value;
};

std::vector<Lex> Lexer(const std::string& str)
{
  TextFileParser parser(str);
  std::vector<Lex> r;

  bool inside = false;

  std::ostringstream buffer;
  while(parser.HasMore()) {
    if(inside) {
      parser.SkipSpaces(true);
      if( IsIdentStart(parser.PeekChar()) ) {
        const std::string ident = parser.ReadIdent();
        if(ident == "ifdef") {
          r.push_back(Lex{ LexType::IFDEF });
        }
        else if(ident == "end") {
          r.push_back(Lex{ LexType::END });
        }
        else if(ident == "eval") {
          r.push_back(Lex{ LexType::EVAL });
        }
        else {
          r.push_back(Lex{ LexType::IDENT, ident});
        }
      }
      else if( parser.PeekChar() == '@' ) {
        parser.AdvanceChar();
        r.push_back(Lex{ LexType::EVAL});
      }
      else if( parser.PeekChar(0) == '}' && parser.PeekChar(1) == '}') {
        parser.AdvanceChar();
        parser.AdvanceChar();
      }
      else {
        // parser error
        const std::vector<Lex> empty_lexer;
        return empty_lexer;
      }
    }
    else {
      // outside of the {{ }}
      if( parser.PeekChar(0) == '{' && parser.PeekChar(1) == '{') {
        const std::string b = buffer.str();
        if(b.empty() == false) {
          buffer.str("");
          r.push_back(Lex{LexType::TEXT, b});
        }
        parser.AdvanceChar();
        parser.AdvanceChar();
        parser.SkipSpaces(true);
        inside = true;
      }
      else {
        buffer << parser.ReadChar();
      }
    }
  }

  Assert(inside);
  return r;
}

////////////////////////////////////////////////////////////////////////////////

class LexReader {
 public:
  LexReader(const std::vector<Lex>& input)
  : lex_(input)
  , pos_(0)
  , size_(input.size())
  {
  }

  bool HasMore() const {
    return pos_ < size_;
  }

  const Lex& Peek() const {
    if(HasMore())
    {
      return lex_[pos_];
    }
    static const Lex end_of_file { LexType::END_OF_FILE};
    return end_of_file;
  }

  void Advance() {
    pos_ += 1;
  }

  const Lex& Read() {
    const Lex& lex = Peek();
    Advance();
    return lex;
  }

 private:
  std::vector<Lex> lex_;
  unsigned int pos_;
  unsigned int size_;
};

////////////////////////////////////////////////////////////////////////////////

std::shared_ptr<TemplateNodeList> ReadTemplateList(LexReader* reader);

std::shared_ptr<TemplateNodeString> ReadText(LexReader* reader) {
  Assert(reader);
  const Lex& lex = reader->Read();
  Assert(lex.type == LexType::TEXT);

  std::shared_ptr<TemplateNodeString> ret{ new TemplateNodeString { lex.value } };
  return ret;
}

std::shared_ptr<TemplateNodeEval> ReadEval(LexReader* reader) {
  Assert(reader);
  const Lex& lex = reader->Read();

  if(lex.type == LexType::IDENT) {
    std::shared_ptr<TemplateNodeEval> ret { new TemplateNodeEval { lex.value } };
    return ret;
  }
  // todo: handle parse error
  std::shared_ptr<TemplateNodeEval> ret { new TemplateNodeEval { "parse_error" } };
  return ret;
}

std::shared_ptr<TemplateNodeIfdef> ReadIfdef(LexReader* reader) {
  Assert(reader);
  const Lex& lex = reader->Read();

  if(lex.type == LexType::IDENT) {
    std::shared_ptr<TemplateNode> children = ReadTemplateList(reader);
    std::shared_ptr<TemplateNodeIfdef> ret { new TemplateNodeIfdef { lex.value, children} };
    return ret;
  }
  // todo: handle parse error
  std::shared_ptr<TemplateNodeString> dummy { new TemplateNodeString{"parse_error"} };
  std::shared_ptr<TemplateNodeIfdef> ret { new TemplateNodeIfdef { "parse_error", dummy } };
  return ret;
}

std::shared_ptr<TemplateNodeList> ReadTemplateList(LexReader* reader)
{
  Assert(reader);

  std::shared_ptr<TemplateNodeList> list { new TemplateNodeList{} };

  while(reader->HasMore() && reader->Peek().type != LexType::END) {
    switch(reader->Peek().type) {
      case LexType::TEXT:
        list->Add(ReadText(reader));
        break;
      case LexType::EVAL:
        reader->Advance();
        list->Add(ReadEval(reader));
        break;
      case LexType::IFDEF:
        reader->Advance();
        list->Add(ReadIfdef(reader));
        break;
      default:
        // todo: handle parser error
        return list;
    }
  }

  reader->Advance(); // skip end

  return list;
}

////////////////////////////////////////////////////////////////////////////////

Template::~Template()
{ }

Template::Template(const std::string& text)
{
  LexReader reader(Lexer(text));
  nodes_ = ReadTemplateList(&reader);
}

std::string Template::Evaluate(const Defines& defines) {
  std::ostringstream ss;
  if(nodes_) {
    nodes_->Eval(defines, &ss);
  }
  return ss.str();
}

const TemplateError& Template::GetErrors() const {
  return errors_;
}
