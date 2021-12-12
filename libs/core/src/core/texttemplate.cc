#include "core/texttemplate.h"

#include "core/textfileparser.h"
#include "core/assert.h"
#include "core/str.h"
#include "core/noncopyable.h"
#include "core/vfs.h"
#include "core/vfs_path.h"
#include "core/stringutils.h"

#include <utility>
#include <vector>
#include <sstream>

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
    Defines::undefine(const std::string& name)
    {
        values.erase(name);
    }


    void
    Defines::define(const std::string& name, const std::string& value)
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
        const std::string message = StringBuilder() << file_name << ":" << line << ":"
                                          << " " << error;
        errors.push_back(message);
    }


    std::string
    TemplateErrorList::get_combined_errors() const
    {
        std::ostringstream ss;

        for(const auto& mess: errors)
        {
            ss << mess << "\n";
        }

        return ss.str();
    }


    ///////////////////////////////////////////////////////////////////////////


    struct TemplateNode
    {
        TemplateNode() = default;
        virtual ~TemplateNode() = default;

        NONCOPYABLE_CONSTRUCTOR(TemplateNode);
        NONCOPYABLE_ASSIGNMENT(TemplateNode);
        NONCOPYABLE_MOVE_CONSTRUCTOR(TemplateNode);
        NONCOPYABLE_MOVE_ASSIGNMENT(TemplateNode);

        virtual void
        evaluate
        (
            Defines* defines,
            std::ostringstream* out,
            TemplateErrorList* error
        ) = 0;
    };


    // ------------------------------------------------------------------------


    struct TemplateNodeString : TemplateNode
    {
        explicit TemplateNodeString(std::string text)
            : text(std::move(text))
        {
        }

        void
        evaluate(Defines* /*defines*/,
             std::ostringstream* out,
             TemplateErrorList* /*error*/) override
        {
            ASSERT(out);
            *out << text;
        }
        
        std::string text;
    };


    // ------------------------------------------------------------------------


    struct TemplateNodeList : TemplateNode
    {
        TemplateNodeList() = default;

        void
        evaluate(Defines* defines,
             std::ostringstream* out,
             TemplateErrorList* error) override
        {
            for(const auto& node: nodes)
            {
                node->evaluate(defines, out, error);
            }
        }

        void
        add(const std::shared_ptr<TemplateNode>& node)
        {
            nodes.push_back(node);
        }

        std::vector<std::shared_ptr<TemplateNode>> nodes;
    };


    // ------------------------------------------------------------------------


    struct TemplateNodeScopedList : TemplateNodeList
    {
        TemplateNodeScopedList() = default;

        void
        evaluate(core::Defines* defines,
             std::ostringstream* out,
             TemplateErrorList* error) override
        {
            ASSERT(defines);
            core::Defines my_defines = *defines;
            TemplateNodeList::evaluate(&my_defines, out, error);
        }
    };


    // ------------------------------------------------------------------------


    struct TemplateNodeIfdef : TemplateNode
    {
        TemplateNodeIfdef(std::string name, std::shared_ptr<TemplateNode> node)
            : name(std::move(name))
            , node(std::move(node))
        {
        }

        void
        evaluate(Defines* defines,
             std::ostringstream* out,
             TemplateErrorList* error) override
        {
            ASSERT(defines);
            if(defines->is_defined(name))
            {
                node->evaluate(defines, out, error);
            }
        }

        std::string name;
        std::shared_ptr<TemplateNode> node;
    };


    // ------------------------------------------------------------------------


    struct TemplateNodeEval : TemplateNode
    {
        explicit TemplateNodeEval(std::string name)
            : name(std::move(name))
        {
        }

        void
        evaluate(Defines* defines,
             std::ostringstream* out,
             TemplateErrorList* error) override
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
                    StringBuilder() << name << " is not defined"
                );
            }

            *out << defines->get_value(name);
        }
        
        std::string name;
    };

    // ------------------------------------------------------------------------

    struct TemplateNodeSet : TemplateNode
    {
        TemplateNodeSet(std::string name, std::string value)
            : name(std::move(name))
            , value(std::move(value))
        {
        }

        void
        evaluate(Defines* defines,
             std::ostringstream* out,
             TemplateErrorList* /*error*/) override
        {
            ASSERT(out);
            ASSERT(defines);

            defines->define(name, value);
        }
        
        std::string name;
        std::string value;
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

    std::string
    lex_type_to_string(TokenType t)
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
            return StringBuilder() << lex_type_to_string(type) << "(" << first_chars_with_ellipsis(value) << ")";
        }

        TokenType type;
        std::string value;
        int line;
        int column;
    };


    std::vector<Token>
    lexer
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

        std::ostringstream buffer;
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
                    buffer.str("");
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
                    const std::string b = buffer.str();
                    if(!b.empty())
                    {
                        buffer.str("");
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
                    buffer << parser.read_char();
                }
            }
        }

        if(inside)
        {
            error->add_error(
                    file,
                    parser.get_line(),
                    parser.get_column(),
                    "Expected end marker }}");
        }

        const std::string buffer_str = buffer.str();
        if(!buffer_str.empty())
        {
            r.emplace_back(TokenType::text, buffer_line, buffer_column, buffer_str);
        }

        return r;
    }


    ////////////////////////////////////////////////////////////////////////////////


    struct TokenReader
    {
        explicit TokenReader(const std::vector<Token>& input)
            : tokens(input)
            , next_token_index(0)
            , size(input.size())
        {
        }

        [[nodiscard]] bool
        has_more() const
        {
            return next_token_index < size;
        }

        [[nodiscard]] const Token&
        peek() const
        {
            if(has_more())
            {
                return tokens[next_token_index];
            }
            static const Token end_of_file {TokenType::end_of_file, 0, 0};
            return end_of_file;
        }

        void
        advance()
        {
            next_token_index += 1;
        }

        const Token&
        read()
        {
            const Token& lex = peek();
            advance();
            return lex;
        }

        [[nodiscard]] int
        get_line() const
        {
            return peek().line;
        }

        [[nodiscard]] int
        get_column() const
        {
            return peek().column;
        }

        std::vector<Token> tokens;
        std::size_t next_token_index;
        std::size_t size;
    };


    ////////////////////////////////////////////////////////////////////////////////


    void
    parse_template_list
    (
        std::shared_ptr<TemplateNodeList>* nodes,
        TokenReader* reader,
        TemplateErrorList* errors,
        const vfs::FilePath& file,
        bool expect_end,
        vfs::FileSystem* fs
    );


    void
    load_from_filesystem_to_node_list
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
                StringBuilder() << "Missing filesystem, Failed to read " << path
            );
            return;
        }
        ASSERT(nodes);
        std::string content;
        if(!fs->read_file_to_string(path, &content))
        {
            error->add_error(path, 0, 0, StringBuilder() << "Failed to open " << path);
            return;
        }
        TokenReader reader(lexer(content, error, path));
        parse_template_list(nodes, &reader, error, path, false, fs);
    }


    // ------------------------------------------------------------------------


    std::shared_ptr<TemplateNodeString>
    parse_text
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


        std::shared_ptr<TemplateNodeString> ret {
                new TemplateNodeString {lex.value}};
        return ret;
    }


    std::shared_ptr<TemplateNodeEval>
    parse_eval
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
            StringBuilder() << "Reading EVAL, expected IDENT but found " << lex.to_string()
        );
        std::shared_ptr<TemplateNodeEval> ret{new TemplateNodeEval{"parse_error"}};
        return ret;
    }


    std::shared_ptr<TemplateNodeSet>
    parse_set
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
                StringBuilder() << "Reading SET, expected IDENT but found " << name.to_string()
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
                StringBuilder() << "Reading SET, expected STRING but found " << val.to_string()
            );
            std::shared_ptr<TemplateNodeSet> ret{new TemplateNodeSet{name.value, "parse_error"}};
            return ret;
        }

        std::shared_ptr<TemplateNodeSet> ret {
                new TemplateNodeSet {name.value, val.value}};
        return ret;
    }


    std::shared_ptr<TemplateNodeIfdef>
    parse_ifdef
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
            StringBuilder() << "Reading IFDEF, expected IDENT but found " << lex.to_string()
        );
        std::shared_ptr<TemplateNodeString> dummy { new TemplateNodeString {"parse_error"}};
        std::shared_ptr<TemplateNodeIfdef> ret { new TemplateNodeIfdef {"parse_error", dummy}};
        return ret;
    }


    std::shared_ptr<TemplateNodeList>
    parse_include
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
                    StringBuilder() << "Invalid path "
                        << lex.value
                );
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
                    StringBuilder() << "Unable to open " << file_argument.value()
                );
            }

            return ret;
        }
        errors->add_error
        (
            file,
            reader->get_line(),
            reader->get_column(),
            StringBuilder() << "Reading INCLUDE, expected STRING but found " << lex.to_string()
        );
        std::shared_ptr<TemplateNodeList> ret {new TemplateNodeList {}};
        return ret;
    }


    void
    parse_template_list
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
                    StringBuilder() << "Reading LIST " << expect_end << ", Found " << reader->peek().to_string()
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
                    StringBuilder() << "Reading LIST, expected END but found " << end.to_string()
                );
            }
        }
    }


    ///////////////////////////////////////////////////////////////////////////


    CompiledTextTemplate::CompiledTextTemplate(const std::string& text)
        : nodes(new TemplateNodeList {})
    {
        const auto file = vfs::FilePath{"~/from_string"};
        TokenReader reader(lexer(text, &errors, file));
        parse_template_list(&nodes, &reader, &errors, file, false, nullptr);
    }


    CompiledTextTemplate::CompiledTextTemplate(vfs::FileSystem* fs, const vfs::FilePath& path)
        : nodes(new TemplateNodeList {})
    {
        ASSERT(fs);
        load_from_filesystem_to_node_list(fs, path, &errors, &nodes);
    }


    CompiledTextTemplate::~CompiledTextTemplate() = default;


    std::string
    CompiledTextTemplate::evaluate(const core::Defines& defines)
    {
        std::ostringstream ss;

        if(errors.has_errors())
        {
            return "";
        }

        if(nodes)
        {
            core::Defines my_defines = defines;
            nodes->evaluate(&my_defines, &ss, &errors);
        }

        return ss.str();
    }
}
