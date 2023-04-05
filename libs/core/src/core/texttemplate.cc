#include "core/texttemplate.h"

#include <utility>

#include "assert/assert.h"

#include "core/textfileparser.h"
#include "core/vfs.h"
#include "core/vfs_path.h"
#include "core/stringutils.h"
#include "core/stringbuilder.h"


namespace euphoria::core
{


////////////////////////////////////////////////////////////////////////////////


Defines::Defines() = default;


bool
Defines::is_defined(const std::string& name) const
{
    const auto found = values.find(name);
    return found != values.end();
}


std::string
Defines::get_value(const std::string& name) const
{
    const auto found = values.find(name);
    if(found == values.end())
    {
        return "";
    }

    return found->second;
}


void
Defines::remove(const std::string& name)
{
    values.erase(name);
}


void
Defines::set(const std::string& name, const std::string& value)
{
    values[name] = value;
}


///////////////////////////////////////////////////////////////////////////


TemplateErrorList::TemplateErrorList() = default;


bool
TemplateErrorList::has_errors() const
{
    return !errors.empty();
}


void
TemplateErrorList::add_error
(
    const std::optional<vfs::FilePath>& file,
    int line,
    int /*unused*/,
    const std::string& error
)
{
    const auto file_name
        = file.has_value()
        ? file.value().path
        : "<no_file>"
    ;
    const std::string message = fmt::format("{}({}): {}", file_name, line, error);
    errors.push_back(message);
}


std::string
TemplateErrorList::get_combined_errors() const
{
    auto ss = StringBuilder{};

    for(const auto& mess: errors)
    {
        ss.add_string(mess);
        ss.add_char('\n');
    }

    return ss.to_string();
}


///////////////////////////////////////////////////////////////////////////


struct TemplateNode
{
    TemplateNode() = default;
    virtual ~TemplateNode() = default;

    TemplateNode(const TemplateNode&) = delete;
    TemplateNode(TemplateNode&&) = delete;
    void operator=(const TemplateNode&) = delete;
    void operator=(TemplateNode&&) = delete;

    virtual void build_string
    (
        Defines* defines,
        StringBuilder* out,
        TemplateErrorList* error
    ) = 0;
};


// ------------------------------------------------------------------------


struct TemplateNodeString : TemplateNode
{
    std::string text;

    explicit TemplateNodeString(std::string t)
        : text(std::move(t))
    {
    }

    void build_string(Defines* /*defines*/, StringBuilder* out, TemplateErrorList* /*error*/) override
    {
        ASSERT(out);
        out->add_string(text);
    }
};


// ------------------------------------------------------------------------


struct TemplateNodeList : TemplateNode
{
    std::vector<std::shared_ptr<TemplateNode>> nodes;

    TemplateNodeList() = default;

    void add(const std::shared_ptr<TemplateNode>& node)
    {
        nodes.push_back(node);
    }

    void build_string(Defines* defines, StringBuilder* out, TemplateErrorList* error) override
    {
        for(const auto& node: nodes)
        {
            node->build_string(defines, out, error);
        }
    }
};


// ------------------------------------------------------------------------


struct TemplateNodeScopedList : TemplateNodeList
{
    TemplateNodeScopedList() = default;

    void
    build_string(core::Defines* defines, StringBuilder* out, TemplateErrorList* error) override
    {
        ASSERT(defines);
        core::Defines my_defines = *defines;
        TemplateNodeList::build_string(&my_defines, out, error);
    }
};


// ------------------------------------------------------------------------


struct TemplateNodeIfdef : TemplateNode
{
    std::string name;
    std::shared_ptr<TemplateNode> node;

    TemplateNodeIfdef(std::string aname, std::shared_ptr<TemplateNode> anode)
        : name(std::move(aname))
        , node(std::move(anode))
    {
    }

    void
    build_string(Defines* defines, StringBuilder* out, TemplateErrorList* error) override
    {
        ASSERT(defines);
        if(defines->is_defined(name))
        {
            node->build_string(defines, out, error);
        }
    }
};


// ------------------------------------------------------------------------


struct TemplateNodeEval : TemplateNode
{
    std::string name;

    explicit TemplateNodeEval(std::string n)
        : name(std::move(n))
    {
    }

    void build_string(Defines* defines, StringBuilder* out, TemplateErrorList* error) override
    {
        ASSERT(out);
        ASSERT(defines);

        if(error != nullptr && !defines->is_defined(name))
        {
            // todo(Gustav): add file, line and column
            error->add_error
            (
                std::nullopt,
                0,
                0,
                fmt::format("{} is not defined", name)
            );
        }

        out->add_string(defines->get_value(name));
    }
};

// ------------------------------------------------------------------------

struct TemplateNodeSet : TemplateNode
{
    std::string name;
    std::string value;

    TemplateNodeSet(std::string aname, std::string avalue)
        : name(std::move(aname))
        , value(std::move(avalue))
    {
    }

    void build_string(Defines* defines, StringBuilder* out, TemplateErrorList* /*error*/) override
    {
        ASSERT(out);
        ASSERT(defines);

        defines->set(name, value);
    }
};


///////////////////////////////////////////////////////////////////////////


enum class TokenType
{
    text,
    if_def,
    eval,
    ident,
    end,
    set,
    string,
    include,
    end_of_file
};

std::string to_string(TokenType t)
{
    switch(t)
    {
#define CASE(V) case TokenType::V: return #V
        CASE(text);
        CASE(if_def);
        CASE(eval);
        CASE(ident);
        CASE(end);
        CASE(set);
        CASE(include);
        CASE(string);
        CASE(end_of_file);
#undef CASE
    }

    return "<UNHANDLED_CASE>";
}


struct Token
{
    TokenType type;
    std::string value;
    int line;
    int column;

    Token(TokenType t, int l, int c, std::string v = "")
        : type(t)
        , value(std::move(v))
        , line(l)
        , column(c)
    {
    }

    [[nodiscard]] std::string
    to_string() const
    {
        return fmt::format("{}({})", core::to_string(type), get_first_chars_with_ellipsis(value));
    }
};


std::vector<Token> tokenize
(
    const std::string& content,
    TemplateErrorList* error,
    const vfs::FilePath& file
)
{
    ASSERT(error);

    auto parser = TextfileParser::from_string(content);
    std::vector<Token> r;

    bool inside = false;

    StringBuilder buffer;
    int buffer_line = parser.get_line();
    int buffer_column = parser.get_column();

    while(parser.has_more())
    {
        if(inside)
        {
            parser.skip_spaces(true);
            if(is_ident_start(parser.peek_char()))
            {
                const int line = parser.get_line();
                const int column = parser.get_column();
                const std::string ident = parser.read_ident();
                if(ident == "ifdef")
                {
                    r.emplace_back(TokenType::if_def, line, column);
                }
                else if(ident == "end")
                {
                    r.emplace_back(TokenType::end, line, column);
                }
                else if(ident == "eval")
                {
                    r.emplace_back(TokenType::eval, line, column);
                }
                else if(ident == "set")
                {
                    r.emplace_back(TokenType::set, line, column);
                }
                else if(ident == "include")
                {
                    r.emplace_back(TokenType::include, line, column);
                }
                else
                {
                    r.emplace_back(TokenType::ident, line, column, ident);
                }
            }
            else if(parser.peek_char() == '@')
            {
                const int line = parser.get_line();
                const int column = parser.get_column();
                parser.advance_char();
                r.emplace_back(TokenType::eval, line, column);
            }
            else if(parser.peek_char() == '\"')
            {
                const int line = parser.get_line();
                const int column = parser.get_column();
                const std::string& str = parser.read_string();
                r.emplace_back(TokenType::string, line, column, str);
            }
            else if(parser.peek_char(0) == '}' && parser.peek_char(1) == '}')
            {
                parser.advance_char();
                parser.advance_char();
                inside = false;
                buffer.clear();
                buffer_line = parser.get_line();
                buffer_column = parser.get_column();
            }
            else
            {
                // parser error
                return {};
            }
        }
        else
        {
            // outside of the {{ }}
            if(parser.peek_char(0) == '{' && parser.peek_char(1) == '{')
            {
                const std::string b = buffer.to_string();
                buffer.clear();
                if(!b.empty())
                {
                    buffer_line = parser.get_line();
                    buffer_column = parser.get_column();
                    r.emplace_back(TokenType::text, buffer_line, buffer_column, b);
                }
                parser.advance_char();
                parser.advance_char();
                parser.skip_spaces(true);
                inside = true;
            }
            else
            {
                buffer.add_char(parser.read_char());
            }
        }
    }

    if(inside)
    {
        error->add_error( file, parser.get_line(), parser.get_column(), "Expected end marker }}");
    }

    const std::string buffer_str = buffer.to_string();
    if(!buffer_str.empty())
    {
        r.emplace_back(TokenType::text, buffer_line, buffer_column, buffer_str);
    }

    return r;
}


////////////////////////////////////////////////////////////////////////////////


struct TokenReader
{
    std::vector<Token> tokens;
    std::size_t next_token_index = 0;
    std::size_t size;

    explicit TokenReader(const std::vector<Token>& input)
        : tokens{input}
        , size{input.size()}
    {
    }

    void advance()
    {
        next_token_index += 1;
    }

    const Token& read()
    {
        const Token& lex = peek();
        advance();
        return lex;
    }

    [[nodiscard]] bool has_more() const
    {
        return next_token_index < size;
    }

    [[nodiscard]] const Token& peek() const
    {
        if(has_more())
        {
            return tokens[next_token_index];
        }
        static const Token end_of_file {TokenType::end_of_file, 0, 0};
        return end_of_file;
    }

    [[nodiscard]] int get_line() const
    {
        return peek().line;
    }

    [[nodiscard]] int get_column() const
    {
        return peek().column;
    }
};


////////////////////////////////////////////////////////////////////////////////


void parse_template_list
(
    std::shared_ptr<TemplateNodeList>* nodes,
    TokenReader* reader,
    TemplateErrorList* errors,
    const vfs::FilePath& file,
    bool expect_end,
    vfs::FileSystem* fs
);


void load_from_filesystem_to_node_list
(
    vfs::FileSystem* fs,
    const vfs::FilePath& path,
    TemplateErrorList* error,
    std::shared_ptr<TemplateNodeList>* nodes
)
{
    if(fs == nullptr)
    {
        error->add_error
        (
            path,
            0,
            0,
            fmt::format("Missing filesystem, Failed to read {}", path)
        );
        return;
    }
    ASSERT(nodes);
    auto content = fs->read_file_to_string(path);
    if(!content)
    {
        error->add_error(path, 0, 0, fmt::format("Failed to open {}", path));
        return;
    }
    auto reader = TokenReader{tokenize(*content, error, path)};
    parse_template_list(nodes, &reader, error, path, false, fs);
}


// ------------------------------------------------------------------------


std::shared_ptr<TemplateNodeString> parse_text
(
    TokenReader* reader,
    TemplateErrorList* /*unused*/,
    const vfs::FilePath& /*unused*/,
    vfs::FileSystem* /*unused*/
)
{
    ASSERT(reader);
    const Token& lex = reader->read();
    ASSERT(lex.type == TokenType::text);


    std::shared_ptr<TemplateNodeString> ret {new TemplateNodeString {lex.value}};
    return ret;
}


std::shared_ptr<TemplateNodeEval> parse_eval
(
    TokenReader* reader,
    TemplateErrorList* errors,
    const vfs::FilePath& file,
    vfs::FileSystem* /*unused*/
)
{
    ASSERT(reader);
    const Token& lex = reader->read();

    if(lex.type == TokenType::ident)
    {
        std::shared_ptr<TemplateNodeEval> ret{new TemplateNodeEval{lex.value}};
        return ret;
    }

    errors->add_error
    (
        file,
        reader->get_line(),
        reader->get_column(),
        fmt::format("Reading EVAL, expected IDENT but found {}", lex.to_string())
    );
    std::shared_ptr<TemplateNodeEval> ret{new TemplateNodeEval{"parse_error"}};
    return ret;
}


std::shared_ptr<TemplateNodeSet> parse_set
(
    TokenReader* reader,
    TemplateErrorList* errors,
    const vfs::FilePath& file,
    vfs::FileSystem* /*unused*/
)
{
    ASSERT(reader);
    const Token& name = reader->read();

    if(name.type != TokenType::ident)
    {
        errors->add_error
        (
            file,
            reader->get_line(),
            reader->get_column(),
            fmt::format("Reading SET, expected IDENT but found {}", name.to_string())
        );
        std::shared_ptr<TemplateNodeSet> ret{new TemplateNodeSet{"parse_error", "parse_error"}};
        return ret;
    }

    const Token& val = reader->read();

    if(val.type != TokenType::string)
    {
        errors->add_error
        (
            file,
            reader->get_line(),
            reader->get_column(),
            fmt::format("Reading SET, expected STRING but found {}", val.to_string())
        );
        std::shared_ptr<TemplateNodeSet> ret{new TemplateNodeSet{name.value, "parse_error"}};
        return ret;
    }

    std::shared_ptr<TemplateNodeSet> ret {new TemplateNodeSet {name.value, val.value}};
    return ret;
}


std::shared_ptr<TemplateNodeIfdef> parse_ifdef
(
    TokenReader* reader,
    TemplateErrorList* errors,
    const vfs::FilePath& file,
    vfs::FileSystem* fs
)
{
    ASSERT(reader);
    const Token& lex = reader->read();

    if(lex.type == TokenType::ident)
    {
        std::shared_ptr<TemplateNodeList> children{new TemplateNodeList{}};
        parse_template_list(&children, reader, errors, file, true, fs);
        std::shared_ptr<TemplateNodeIfdef> ret{new TemplateNodeIfdef {lex.value, children}};
        return ret;
    }
    errors->add_error
    (
        file,
        reader->get_line(),
        reader->get_column(),
        fmt::format("Reading IFDEF, expected IDENT but found {}", lex.to_string())
    );
    std::shared_ptr<TemplateNodeString> dummy { new TemplateNodeString {"parse_error"}};
    std::shared_ptr<TemplateNodeIfdef> ret { new TemplateNodeIfdef {"parse_error", dummy}};
    return ret;
}


std::shared_ptr<TemplateNodeList> parse_include
(
    TokenReader* reader,
    TemplateErrorList* errors,
    const vfs::FilePath& file,
    vfs::FileSystem* fs
)
{
    ASSERT(reader);
    const Token& lex = reader->read();

    if(lex.type == TokenType::string)
    {
        std::shared_ptr<TemplateNodeList> ret
        {
            new TemplateNodeScopedList {}
        };
        const auto file_argument = vfs::FilePath::from_script(lex.value);
        if(file_argument.has_value() == false)
        {
            errors->add_error
            (
                file,
                reader->get_line(),
                reader->get_column(),
                fmt::format("Invalid path {}", lex.value)
            );
            return ret;
        }
        const auto resolved_file = vfs::resolve_relative
        (
            file_argument.value(),
            file.get_directory()
        );
        if(resolved_file.has_value())
        {
            load_from_filesystem_to_node_list
            (
                fs,
                resolved_file.value(),
                errors,
                &ret
            );
        }
        else
        {
            errors->add_error
            (
                file,
                reader->get_line(),
                reader->get_column(),
                fmt::format("Unable to open {}", file_argument.value())
            );
        }

        return ret;
    }
    errors->add_error
    (
        file,
        reader->get_line(),
        reader->get_column(),
        fmt::format("Reading INCLUDE, expected STRING but found {}", lex.to_string())
    );
    std::shared_ptr<TemplateNodeList> ret {new TemplateNodeList {}};
    return ret;
}


void parse_template_list
(
    std::shared_ptr<TemplateNodeList>* nodes,
    TokenReader* reader,
    TemplateErrorList* errors,
    const vfs::FilePath& file,
    bool expect_end,
    vfs::FileSystem* fs
)
{
    ASSERT(nodes);
    std::shared_ptr<TemplateNodeList>& list = *nodes;

    ASSERT(reader);
    while(!errors->has_errors() && reader->has_more() && (!expect_end || reader->peek().type != TokenType::end))
    {
        switch(reader->peek().type)
        {
        case TokenType::text:
            list->add(parse_text(reader, errors, file, fs));
            break;
        case TokenType::eval:
            reader->advance();
            list->add(parse_eval(reader, errors, file, fs));
            break;
        case TokenType::if_def:
            reader->advance();
            list->add(parse_ifdef(reader, errors, file, fs));
            break;
        case TokenType::set:
            reader->advance();
            list->add(parse_set(reader, errors, file, fs));
            break;
        case TokenType::include:
            reader->advance();
            list->add(parse_include(reader, errors, file, fs));
            break;
        default:
            errors->add_error
            (
                file,
                reader->get_line(),
                reader->get_column(),
                fmt::format("Reading LIST {}, Found unexpected {}", expect_end, reader->peek().to_string())
            );
            return;
        }
    }

    if(errors->has_errors())
    {
        return;
    }

    if(expect_end)
    {
        auto end = reader->read(); // skip end
        if(end.type != TokenType::end)
        {
            errors->add_error
            (
                file,
                reader->get_line(),
                reader->get_column(),
                fmt::format("Reading LIST, expected END but found {}", end.to_string())
            );
        }
    }
}


///////////////////////////////////////////////////////////////////////////


CompiledTextTemplate::CompiledTextTemplate(const std::string& text)
    : nodes(new TemplateNodeList {})
{
    const auto file = vfs::FilePath{"~/from_string"};
    auto reader = TokenReader{tokenize(text, &errors, file)};
    parse_template_list(&nodes, &reader, &errors, file, false, nullptr);
}


CompiledTextTemplate::CompiledTextTemplate(vfs::FileSystem* fs, const vfs::FilePath& path)
    : nodes(new TemplateNodeList {})
{
    ASSERT(fs);
    load_from_filesystem_to_node_list(fs, path, &errors, &nodes);
}


CompiledTextTemplate::~CompiledTextTemplate() = default;


std::string CompiledTextTemplate::build_string(const core::Defines& defines)
{
    StringBuilder ss;

    if(errors.has_errors())
    {
        return "";
    }

    if(nodes)
    {
        core::Defines my_defines = defines;
        nodes->build_string(&my_defines, &ss, &errors);
    }

    return ss.to_string();
}


}
