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

void TemplateError::AddError(const std::string& file, int line, int column, const std::string& error) {
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

  virtual void Eval(Defines* defines, std::ostringstream* out, TemplateError* error) = 0;
};

// -----------------------------------------------------------------------------

class TemplateNodeString : public TemplateNode {
 public:
  TemplateNodeString(const std::string& text)
      : text_(text) {}
  void Eval(Defines* defines, std::ostringstream* out, TemplateError* error) override
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

  void Eval(Defines* defines, std::ostringstream* out, TemplateError* error) override
  {
    for(auto node : nodes_) {
      node->Eval(defines, out, error);
    }
  }

  void Add(std::shared_ptr<TemplateNode> node) {
    nodes_.push_back(node);
  }

 private:
  std::vector<std::shared_ptr<TemplateNode>> nodes_;
};

// -----------------------------------------------------------------------------

class TemplateNodeScopedList : public TemplateNodeList {
 public:
  TemplateNodeScopedList()
  { }

  void Eval(Defines* defines, std::ostringstream* out, TemplateError* error) override
  {
    Assert(defines);
    Defines my_defines = *defines;
    TemplateNodeList::Eval(&my_defines, out, error);
  }
};

// -----------------------------------------------------------------------------

class TemplateNodeIfdef : public TemplateNode {
 public:
  TemplateNodeIfdef(const std::string& name, std::shared_ptr<TemplateNode> node)
      : name_(name)
      , node_(node)
  { }

  void Eval(Defines* defines, std::ostringstream* out, TemplateError* error) override
  {
    Assert(defines);
    if(defines->IsDefined(name_)) {
      node_->Eval(defines, out, error);
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

  void Eval(Defines* defines, std::ostringstream* out, TemplateError* error) override
  {
    Assert(out);
    Assert(defines);

    if(error && !defines->IsDefined(name_)) {
      // todo: add file, line and column
      error->AddError("", 0, 0, Str() << name_ << " is not defined");
    }

    *out << defines->GetValue(name_);
  }

 private:
  std::string name_;
};

// -----------------------------------------------------------------------------

class TemplateNodeSet : public TemplateNode {
 public:
  TemplateNodeSet(const std::string& name, const std::string& value)
      : name_(name)
      , value_(value)
  { }

  void Eval(Defines* defines, std::ostringstream* out, TemplateError* error) override
  {
    Assert(out);
    Assert(defines);

    defines->Define(name_, value_);
  }

 private:
  std::string name_;
  std::string value_;
};

////////////////////////////////////////////////////////////////////////////////

enum class LexType {
  TEXT, IFDEF, EVAL, IDENT, END, SET, STRING, INCLUDE, END_OF_FILE
};

std::string LexTypeToString(LexType t) {
  switch(t) {
#define CASE(V) case LexType::V: return #V
    CASE(TEXT);
    CASE(IFDEF);
    CASE(EVAL);
    CASE(IDENT);
    CASE(END);
    CASE(SET);
    CASE(INCLUDE);
    CASE(STRING);
    CASE(END_OF_FILE);
#undef CASE
  }

  return "<UNHANDLED_CASE>";
}

std::string FirstChars(const std::string& str, unsigned int count=10) {
  if(str.length()>count) {
    return str.substr(0, count) + "...";
  }

  return str;
}

class Lex {
 public:
  Lex(LexType t, unsigned int l, unsigned int c, const std::string& v = "")
      : type(t), value(v), line(l), column(c)
  {}

  std::string ToString() const {
    return Str() << LexTypeToString(type) << "(" << FirstChars(value) << ")";
  }

  LexType type;
  std::string value;
  unsigned int line;
  unsigned int column;
};

std::vector<Lex> Lexer(const std::string& str, TemplateError* error, const std::string& file)
{
  Assert(error);

  TextFileParser parser(str);
  std::vector<Lex> r;

  bool inside = false;

  std::ostringstream buffer;
  unsigned int buffer_line = parser.GetLine();
  unsigned int buffer_column = parser.GetColumn();

  while(parser.HasMore()) {
    if(inside) {
      parser.SkipSpaces(true);
      if( IsIdentStart(parser.PeekChar()) ) {
        const unsigned int line = parser.GetLine();
        const unsigned int column = parser.GetColumn();
        const std::string ident = parser.ReadIdent();
        if(ident == "ifdef") {
          r.push_back(Lex{ LexType::IFDEF, line, column });
        }
        else if(ident == "end") {
          r.push_back(Lex{ LexType::END, line, column });
        }
        else if(ident == "eval") {
          r.push_back(Lex{ LexType::EVAL, line, column });
        }
        else if(ident == "set") {
          r.push_back(Lex{ LexType::SET, line, column });
        }
        else if(ident == "include") {
          r.push_back(Lex{ LexType::INCLUDE, line, column });
        }
        else {
          r.push_back(Lex{ LexType::IDENT, line, column, ident});
        }
      }
      else if( parser.PeekChar() == '@' ) {
        const unsigned int line = parser.GetLine();
        const unsigned int column = parser.GetColumn();
        parser.AdvanceChar();
        r.push_back(Lex{ LexType::EVAL, line, column});
      }
      else if( parser.PeekChar() == '\"' ) {
        const unsigned int line = parser.GetLine();
        const unsigned int column = parser.GetColumn();
        const std::string& str = parser.ReadString();
        r.push_back(Lex{ LexType::STRING, line, column, str});
      }
      else if( parser.PeekChar(0) == '}' && parser.PeekChar(1) == '}') {
        parser.AdvanceChar();
        parser.AdvanceChar();
        inside = false;
        buffer.str("");
        buffer_line = parser.GetLine();
        buffer_column = parser.GetColumn();
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
          buffer_line = parser.GetLine();
          buffer_column = parser.GetColumn();
          r.push_back(Lex{LexType::TEXT, buffer_line, buffer_column, b});
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

  if(inside) {
    error->AddError(file, parser.GetLine(), parser.GetColumn(), "Expected end marker }}");
  }

  const std::string buffer_str = buffer.str();
  if(buffer_str.empty() == false) {
    r.push_back(Lex{LexType::TEXT, buffer_line, buffer_column, buffer_str});
  }

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
    static const Lex end_of_file { LexType::END_OF_FILE, 0, 0};
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

  unsigned int GetLine() const {
    return Peek().line;
  }

  unsigned int GetColumn() const {
    return Peek().column;
  }

 private:
  std::vector<Lex> lex_;
  unsigned int pos_;
  unsigned int size_;
};

////////////////////////////////////////////////////////////////////////////////

void ReadTemplateList(std::shared_ptr<TemplateNodeList>* nodes, LexReader* reader, TemplateError* errors, const std::string& file, bool expect_end, FileSystem* fs);

void LoadFromFilesystemToNodeList(FileSystem* fs, const std::string& path, TemplateError* error, std::shared_ptr<TemplateNodeList>* nodes)
{
  if(fs == nullptr) {
    error->AddError(path, 0,0, Str() << "Missing filesystem, Failed to read " << path);
    return;
  }
  Assert(nodes);
  std::string content;
  if( !fs->ReadFileToString(path, &content) ) {
    error->AddError(path, 0,0, Str() << "Failed to open " << path);
    return;
  }
  LexReader reader(Lexer(content, error, path));
  ReadTemplateList(nodes, &reader, error, path, false, fs);
}

// -----------------------------------------------------------------------------

std::shared_ptr<TemplateNodeString> ReadText(LexReader* reader, TemplateError* errors, const std::string& file, FileSystem* fs) {
  Assert(reader);
  const Lex& lex = reader->Read();
  Assert(lex.type == LexType::TEXT);


  std::shared_ptr<TemplateNodeString> ret{ new TemplateNodeString { lex.value } };
  return ret;
}

std::shared_ptr<TemplateNodeEval> ReadEval(LexReader* reader, TemplateError* errors, const std::string& file, FileSystem* fs) {
  Assert(reader);
  const Lex& lex = reader->Read();

  if(lex.type == LexType::IDENT) {
    std::shared_ptr<TemplateNodeEval> ret { new TemplateNodeEval { lex.value } };
    return ret;
  }

  errors->AddError(file, reader->GetLine(), reader->GetColumn(), Str() << "Reading EVAL, expected IDENT but found " << lex.ToString());
  std::shared_ptr<TemplateNodeEval> ret { new TemplateNodeEval { "parse_error" } };
  return ret;
}

std::shared_ptr<TemplateNodeSet> ReadSet(LexReader* reader, TemplateError* errors, const std::string& file, FileSystem* fs) {
  Assert(reader);
  const Lex& name = reader->Read();

  if(name.type != LexType::IDENT) {
    errors->AddError(file, reader->GetLine(), reader->GetColumn(), Str() << "Reading SET, expected IDENT but found " << name.ToString());
    std::shared_ptr<TemplateNodeSet> ret { new TemplateNodeSet { "parse_error", "parse_error" } };
    return ret;
  }

  const Lex& val = reader->Read();

  if(val.type != LexType::STRING) {
    errors->AddError(file, reader->GetLine(), reader->GetColumn(), Str() << "Reading SET, expected STRING but found " << val.ToString());
    std::shared_ptr<TemplateNodeSet> ret { new TemplateNodeSet { name.value, "parse_error" } };
    return ret;
  }

  std::shared_ptr<TemplateNodeSet> ret { new TemplateNodeSet { name.value, val.value } };
  return ret;
}

std::shared_ptr<TemplateNodeIfdef> ReadIfdef(LexReader* reader, TemplateError* errors, const std::string& file, FileSystem* fs) {
  Assert(reader);
  const Lex& lex = reader->Read();

  if(lex.type == LexType::IDENT) {
    std::shared_ptr<TemplateNodeList> children { new TemplateNodeList{} };
    ReadTemplateList(&children, reader, errors, file, true, fs);
    std::shared_ptr<TemplateNodeIfdef> ret { new TemplateNodeIfdef { lex.value, children} };
    return ret;
  }
  errors->AddError(file, reader->GetLine(), reader->GetColumn(), Str() << "Reading IFDEF, expected IDENT but found " << lex.ToString());
  std::shared_ptr<TemplateNodeString> dummy { new TemplateNodeString{"parse_error"} };
  std::shared_ptr<TemplateNodeIfdef> ret { new TemplateNodeIfdef { "parse_error", dummy } };
  return ret;
}

std::shared_ptr<TemplateNodeList> ReadInclude(LexReader* reader, TemplateError* errors, const std::string& file, FileSystem* fs) {
  Assert(reader);
  const Lex& lex = reader->Read();

  if(lex.type == LexType::STRING) {
    std::shared_ptr<TemplateNodeList> ret { new TemplateNodeScopedList { } };
    LoadFromFilesystemToNodeList(fs, lex.value, errors, &ret);
    return ret;
  }
  errors->AddError(file, reader->GetLine(), reader->GetColumn(), Str() << "Reading INCLUDE, expected STRING but found " << lex.ToString());
  std::shared_ptr<TemplateNodeList> ret { new TemplateNodeList { } };
  return ret;
}

void ReadTemplateList(std::shared_ptr<TemplateNodeList>* nodes, LexReader* reader, TemplateError* errors, const std::string& file, bool expect_end, FileSystem* fs)
{
  Assert(nodes);
  std::shared_ptr<TemplateNodeList>& list = *nodes;

  Assert(reader);
  while(!errors->HasErrors() && reader->HasMore() && (!expect_end || reader->Peek().type != LexType::END)) {
    switch(reader->Peek().type) {
      case LexType::TEXT:
        list->Add(ReadText(reader, errors, file, fs));
        break;
      case LexType::EVAL:
        reader->Advance();
        list->Add(ReadEval(reader, errors, file, fs));
        break;
      case LexType::IFDEF:
        reader->Advance();
        list->Add(ReadIfdef(reader, errors, file, fs));
        break;
      case LexType::SET:
        reader->Advance();
        list->Add(ReadSet(reader, errors, file, fs));
        break;
      case LexType::INCLUDE:
        reader->Advance();
        list->Add(ReadInclude(reader, errors, file, fs));
        break;
      default:
        errors->AddError(file, reader->GetLine(), reader->GetColumn(), Str() << "Reading LIST "<< expect_end <<", Found " << reader->Peek().ToString());
        return;
    }
  }

  if(errors->HasErrors()) {
    return;
  }

  if(expect_end) {
    Lex end = reader->Read(); // skip end
    if(end.type != LexType::END) {
      errors->AddError(file, reader->GetLine(), reader->GetColumn(), Str() << "Reading LIST, expected END but found " << end.ToString());
    }
  }
}

////////////////////////////////////////////////////////////////////////////////

Template::Template(const std::string& text)
    : nodes_(new TemplateNodeList{})
{
  const std::string file = "from_string";
  LexReader reader(Lexer(text, &errors_, file));
  ReadTemplateList(&nodes_, &reader, &errors_, file, false, nullptr);
}

Template::Template(FileSystem* fs, const std::string& path)
    : nodes_(new TemplateNodeList{})
{
  Assert(fs);
  LoadFromFilesystemToNodeList(fs, path, &errors_, &nodes_);
}

Template::~Template()
{ }

std::string Template::Evaluate(const Defines& defines) {
  std::ostringstream ss;

  if(errors_.HasErrors()) {
    return "";
  }

  if(nodes_) {
    Defines my_defines = defines;
    nodes_->Eval(&my_defines, &ss, &errors_);
  }

  return ss.str();
}

const TemplateError& Template::GetErrors() const {
  return errors_;
}
