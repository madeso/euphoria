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


    defines::defines() = default;


    bool
    defines::is_defined(const std::string& name) const
    {
        const auto found = values.find(name);
        return found != values.end();
    }


    std::string
    defines::get_value(const std::string& name) const
    {
        const auto found = values.find(name);
        if(found == values.end())
        {
            return "";
        }

        return found->second;
    }


    void
    defines::undefine(const std::string& name)
    {
        values.erase(name);
    }


    void
    defines::define(const std::string& name, const std::string& value)
    {
        values[name] = value;
    }


    ///////////////////////////////////////////////////////////////////////////


    template_error_list::template_error_list() = default;


    bool
    template_error_list::has_errors() const
    {
        return !errors.empty();
    }


    void
    template_error_list::add_error
    (
        const std::optional<vfs::file_path>& file,
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
        const std::string message = string_builder() << file_name << ":" << line << ":"
                                          << " " << error;
        errors.push_back(message);
    }


    std::string
    template_error_list::get_combined_errors() const
    {
        std::ostringstream ss;

        for(const auto& mess: errors)
        {
            ss << mess << "\n";
        }

        return ss.str();
    }


    ///////////////////////////////////////////////////////////////////////////


    struct template_node
    {
        template_node() = default;
        virtual ~template_node() = default;

        NONCOPYABLE_CONSTRUCTOR(template_node);
        NONCOPYABLE_ASSIGNMENT(template_node);
        NONCOPYABLE_MOVE_CONSTRUCTOR(template_node);
        NONCOPYABLE_MOVE_ASSIGNMENT(template_node);

        virtual void
        evaluate
        (
            defines* defines,
            std::ostringstream* out,
            template_error_list* error
        ) = 0;
    };


    // ------------------------------------------------------------------------


    struct template_node_string : template_node
    {
        explicit template_node_string(std::string text)
            : text(std::move(text))
        {
        }

        void
        evaluate(defines* /*defines*/,
             std::ostringstream* out,
             template_error_list* /*error*/) override
        {
            ASSERT(out);
            *out << text;
        }
        
        std::string text;
    };


    // ------------------------------------------------------------------------


    struct template_node_list : template_node
    {
        template_node_list() = default;

        void
        evaluate(defines* defines,
             std::ostringstream* out,
             template_error_list* error) override
        {
            for(const auto& node: nodes)
            {
                node->evaluate(defines, out, error);
            }
        }

        void
        add(const std::shared_ptr<template_node>& node)
        {
            nodes.push_back(node);
        }

        std::vector<std::shared_ptr<template_node>> nodes;
    };


    // ------------------------------------------------------------------------


    struct template_node_scoped_list : template_node_list
    {
        template_node_scoped_list() = default;

        void
        evaluate(core::defines* defines,
             std::ostringstream* out,
             template_error_list* error) override
        {
            ASSERT(defines);
            core::defines my_defines = *defines;
            template_node_list::evaluate(&my_defines, out, error);
        }
    };


    // ------------------------------------------------------------------------


    struct template_node_ifdef : template_node
    {
        template_node_ifdef(std::string name, std::shared_ptr<template_node> node)
            : name(std::move(name))
            , node(std::move(node))
        {
        }

        void
        evaluate(defines* defines,
             std::ostringstream* out,
             template_error_list* error) override
        {
            ASSERT(defines);
            if(defines->is_defined(name))
            {
                node->evaluate(defines, out, error);
            }
        }

        std::string name;
        std::shared_ptr<template_node> node;
    };


    // ------------------------------------------------------------------------


    struct template_node_eval : template_node
    {
        explicit template_node_eval(std::string name)
            : name(std::move(name))
        {
        }

        void
        evaluate(defines* defines,
             std::ostringstream* out,
             template_error_list* error) override
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
                    string_builder() << name << " is not defined"
                );
            }

            *out << defines->get_value(name);
        }
        
        std::string name;
    };

    // ------------------------------------------------------------------------

    struct template_node_set : template_node
    {
        template_node_set(std::string name, std::string value)
            : name(std::move(name))
            , value(std::move(value))
        {
        }

        void
        evaluate(defines* defines,
             std::ostringstream* out,
             template_error_list* /*error*/) override
        {
            ASSERT(out);
            ASSERT(defines);

            defines->define(name, value);
        }
        
        std::string name;
        std::string value;
    };


    ///////////////////////////////////////////////////////////////////////////


    enum class token_type
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
    lex_type_to_string(token_type t)
    {
        switch(t)
        {
#define CASE(V) case token_type::V: return #V
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


    struct token
    {
        token(token_type t, int l, int c, std::string v = "")
            : type(t)
            , value(std::move(v))
            , line(l)
            , column(c)
        {
        }

        [[nodiscard]] std::string
        to_string() const
        {
            return string_builder() << lex_type_to_string(type) << "(" << first_chars_with_ellipsis(value) << ")";
        }

        token_type type;
        std::string value;
        int line;
        int column;
    };


    std::vector<token>
    lexer
    (
        const std::string& content,
        template_error_list* error,
        const vfs::file_path& file
    )
    {
        ASSERT(error);

        auto parser = textfile_parser::from_string(content);
        std::vector<token> r;

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
                        r.emplace_back(token_type::if_def, line, column);
                    }
                    else if(ident == "end")
                    {
                        r.emplace_back(token_type::end, line, column);
                    }
                    else if(ident == "eval")
                    {
                        r.emplace_back(token_type::eval, line, column);
                    }
                    else if(ident == "set")
                    {
                        r.emplace_back(token_type::set, line, column);
                    }
                    else if(ident == "include")
                    {
                        r.emplace_back(token_type::include, line, column);
                    }
                    else
                    {
                        r.emplace_back(token_type::ident, line, column, ident);
                    }
                }
                else if(parser.peek_char() == '@')
                {
                    const int line = parser.get_line();
                    const int column = parser.get_column();
                    parser.advance_char();
                    r.emplace_back(token_type::eval, line, column);
                }
                else if(parser.peek_char() == '\"')
                {
                    const int line = parser.get_line();
                    const int column = parser.get_column();
                    const std::string& str = parser.read_string();
                    r.emplace_back(token_type::string, line, column, str);
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
                        r.emplace_back(token_type::text, buffer_line, buffer_column, b);
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
            r.emplace_back(token_type::text, buffer_line, buffer_column, buffer_str);
        }

        return r;
    }


    ////////////////////////////////////////////////////////////////////////////////


    struct token_reader
    {
        explicit token_reader(const std::vector<token>& input)
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

        [[nodiscard]] const token&
        peek() const
        {
            if(has_more())
            {
                return tokens[next_token_index];
            }
            static const token end_of_file {token_type::end_of_file, 0, 0};
            return end_of_file;
        }

        void
        advance()
        {
            next_token_index += 1;
        }

        const token&
        read()
        {
            const token& lex = peek();
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

        std::vector<token> tokens;
        std::size_t next_token_index;
        std::size_t size;
    };


    ////////////////////////////////////////////////////////////////////////////////


    void
    parse_template_list
    (
        std::shared_ptr<template_node_list>* nodes,
        token_reader* reader,
        template_error_list* errors,
        const vfs::file_path& file,
        bool expect_end,
        vfs::file_system* fs
    );


    void
    load_from_filesystem_to_node_list
    (
        vfs::file_system* fs,
        const vfs::file_path& path,
        template_error_list* error,
        std::shared_ptr<template_node_list>* nodes
    )
    {
        if(fs == nullptr)
        {
            error->add_error
            (
                path,
                0,
                0,
                string_builder() << "Missing filesystem, Failed to read " << path
            );
            return;
        }
        ASSERT(nodes);
        std::string content;
        if(!fs->read_file_to_string(path, &content))
        {
            error->add_error(path, 0, 0, string_builder() << "Failed to open " << path);
            return;
        }
        token_reader reader(lexer(content, error, path));
        parse_template_list(nodes, &reader, error, path, false, fs);
    }


    // ------------------------------------------------------------------------


    std::shared_ptr<template_node_string>
    parse_text
    (
        token_reader* reader,
        template_error_list* /*unused*/,
        const vfs::file_path& /*unused*/,
        vfs::file_system* /*unused*/
    )
    {
        ASSERT(reader);
        const token& lex = reader->read();
        ASSERT(lex.type == token_type::text);


        std::shared_ptr<template_node_string> ret {
                new template_node_string {lex.value}};
        return ret;
    }


    std::shared_ptr<template_node_eval>
    parse_eval
    (
        token_reader* reader,
        template_error_list* errors,
        const vfs::file_path& file,
        vfs::file_system* /*unused*/
    )
    {
        ASSERT(reader);
        const token& lex = reader->read();

        if(lex.type == token_type::ident)
        {
            std::shared_ptr<template_node_eval> ret{new template_node_eval{lex.value}};
            return ret;
        }

        errors->add_error
        (
            file,
            reader->get_line(),
            reader->get_column(),
            string_builder() << "Reading EVAL, expected IDENT but found " << lex.to_string()
        );
        std::shared_ptr<template_node_eval> ret{new template_node_eval{"parse_error"}};
        return ret;
    }


    std::shared_ptr<template_node_set>
    parse_set
    (
        token_reader* reader,
        template_error_list* errors,
        const vfs::file_path& file,
        vfs::file_system* /*unused*/
    )
    {
        ASSERT(reader);
        const token& name = reader->read();

        if(name.type != token_type::ident)
        {
            errors->add_error
            (
                file,
                reader->get_line(),
                reader->get_column(),
                string_builder() << "Reading SET, expected IDENT but found " << name.to_string()
            );
            std::shared_ptr<template_node_set> ret{new template_node_set{"parse_error", "parse_error"}};
            return ret;
        }

        const token& val = reader->read();

        if(val.type != token_type::string)
        {
            errors->add_error
            (
                file,
                reader->get_line(),
                reader->get_column(),
                string_builder() << "Reading SET, expected STRING but found " << val.to_string()
            );
            std::shared_ptr<template_node_set> ret{new template_node_set{name.value, "parse_error"}};
            return ret;
        }

        std::shared_ptr<template_node_set> ret {
                new template_node_set {name.value, val.value}};
        return ret;
    }


    std::shared_ptr<template_node_ifdef>
    parse_ifdef
    (
        token_reader* reader,
        template_error_list* errors,
        const vfs::file_path& file,
        vfs::file_system* fs
    )
    {
        ASSERT(reader);
        const token& lex = reader->read();

        if(lex.type == token_type::ident)
        {
            std::shared_ptr<template_node_list> children{new template_node_list{}};
            parse_template_list(&children, reader, errors, file, true, fs);
            std::shared_ptr<template_node_ifdef> ret{new template_node_ifdef {lex.value, children}};
            return ret;
        }
        errors->add_error
        (
            file,
            reader->get_line(),
            reader->get_column(),
            string_builder() << "Reading IFDEF, expected IDENT but found " << lex.to_string()
        );
        std::shared_ptr<template_node_string> dummy { new template_node_string {"parse_error"}};
        std::shared_ptr<template_node_ifdef> ret { new template_node_ifdef {"parse_error", dummy}};
        return ret;
    }


    std::shared_ptr<template_node_list>
    parse_include
    (
        token_reader* reader,
        template_error_list* errors,
        const vfs::file_path& file,
        vfs::file_system* fs
    )
    {
        ASSERT(reader);
        const token& lex = reader->read();

        if(lex.type == token_type::string)
        {
            std::shared_ptr<template_node_list> ret
            {
                new template_node_scoped_list {}
            };
            const auto file_argument = vfs::file_path::from_script(lex.value);
            if(file_argument.has_value() == false)
            {
                errors->add_error
                (
                    file,
                    reader->get_line(),
                    reader->get_column(),
                    string_builder() << "Invalid path "
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
                    string_builder() << "Unable to open " << file_argument.value()
                );
            }

            return ret;
        }
        errors->add_error
        (
            file,
            reader->get_line(),
            reader->get_column(),
            string_builder() << "Reading INCLUDE, expected STRING but found " << lex.to_string()
        );
        std::shared_ptr<template_node_list> ret {new template_node_list {}};
        return ret;
    }


    void
    parse_template_list
    (
        std::shared_ptr<template_node_list>* nodes,
        token_reader* reader,
        template_error_list* errors,
        const vfs::file_path& file,
        bool expect_end,
        vfs::file_system* fs
    )
    {
        ASSERT(nodes);
        std::shared_ptr<template_node_list>& list = *nodes;

        ASSERT(reader);
        while(!errors->has_errors() && reader->has_more() && (!expect_end || reader->peek().type != token_type::end))
        {
            switch(reader->peek().type)
            {
            case token_type::text:
                list->add(parse_text(reader, errors, file, fs));
                break;
            case token_type::eval:
                reader->advance();
                list->add(parse_eval(reader, errors, file, fs));
                break;
            case token_type::if_def:
                reader->advance();
                list->add(parse_ifdef(reader, errors, file, fs));
                break;
            case token_type::set:
                reader->advance();
                list->add(parse_set(reader, errors, file, fs));
                break;
            case token_type::include:
                reader->advance();
                list->add(parse_include(reader, errors, file, fs));
                break;
            default:
                errors->add_error
                (
                    file,
                    reader->get_line(),
                    reader->get_column(),
                    string_builder() << "Reading LIST " << expect_end << ", Found " << reader->peek().to_string()
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
            if(end.type != token_type::end)
            {
                errors->add_error
                (
                    file,
                    reader->get_line(),
                    reader->get_column(),
                    string_builder() << "Reading LIST, expected END but found " << end.to_string()
                );
            }
        }
    }


    ///////////////////////////////////////////////////////////////////////////


    compiled_text_template::compiled_text_template(const std::string& text)
        : nodes(new template_node_list {})
    {
        const auto file = vfs::file_path{"~/from_string"};
        token_reader reader(lexer(text, &errors, file));
        parse_template_list(&nodes, &reader, &errors, file, false, nullptr);
    }


    compiled_text_template::compiled_text_template(vfs::file_system* fs, const vfs::file_path& path)
        : nodes(new template_node_list {})
    {
        ASSERT(fs);
        load_from_filesystem_to_node_list(fs, path, &errors, &nodes);
    }


    compiled_text_template::~compiled_text_template() = default;


    std::string
    compiled_text_template::evaluate(const core::defines& defines)
    {
        std::ostringstream ss;

        if(errors.has_errors())
        {
            return "";
        }

        if(nodes)
        {
            core::defines my_defines = defines;
            nodes->evaluate(&my_defines, &ss, &errors);
        }

        return ss.str();
    }
}
