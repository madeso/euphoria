#include "core/argparse.h"

#include <iostream>

#include <string>
#include <sstream>
#include <functional>
#include <vector>
#include <map>
#include <set>
#include <memory>
#include <iomanip>
// #include <filesystem>

#include "core/assert.h"
#include "core/stringutils.h"
#include "core/functional.h"

namespace euphoria::core
{
    namespace argparse
    {
        /////////////////////////////////////////////////////////////////////////////////////////
        // Name

        bool
        Name::IsOptional(const std::string& arg)
        {
            if(arg.empty())
            {
                return false;
            }
            return arg[0] == '-';
        }

        std::string
        Name::OptionalName(const std::string& name)
        {
            ASSERTX(IsOptional(name), name);
            ASSERTX(!name.empty(), name);
            return name.substr(1);
        }

        Name::Name() : is_optional(false) {}

        Name::Name(const char* str)
            : is_optional(IsOptional(Trim(str))), names(Split(str, ','))
        {
            if(is_optional)
            {
                for(auto& n: names)
                {
                    ASSERTX(IsOptional(Trim(n)), n);
                    n = OptionalName(Trim(n));
                }
            }

#ifdef _DEBUG
            AssertValid();
#endif
        }

        Name
        Name::Parse(const std::string& n)
        {
            return {n.c_str()};
        }

        Name
        Name::Optional(std::initializer_list<std::string> names)
        {
            return {true, names};
        }

        Name
        Name::Positional(const std::string& name)
        {
            return {false, {name}};
        }

        Name::Name(bool o, const std::vector<std::string>& n)
            : is_optional(o), names(n)
        {
#ifdef _DEBUG
            AssertValid();
#endif
        }

#ifdef _DEBUG
        void
        Name::AssertValid()
        {
            if(is_optional)
            {
                ASSERTX(names.size() > 0, names.size());
            }
            else
            {
                ASSERTX(names.size() == 1, names.size());
            }

            // todo: assert that name only contains valid characters...
        }
#endif

        /////////////////////////////////////////////////////////////////////////////////////////
        // ConsoleOutput

        void
        ConsoleOutput::OnError(const std::string& err)
        {
            std::cerr << err << "\n";
        }

        void
        ConsoleOutput::OnInfo(const std::string& info)
        {
            std::cout << info << "\n";
        }

        /////////////////////////////////////////////////////////////////////////////////////////
        // Running

        bool
        Running::HasMore() const
        {
            return next_index < arguments.size();
        }

        std::string
        Running::Read()
        {
            if(next_index < arguments.size())
            {
                const auto ret = arguments[next_index];
                next_index += 1;
                return ret;
            }
            else
            {
                return "";
            }
        }

        std::string
        Running::Peek(size_t advance) const
        {
            ASSERTX(advance > 0, advance);
            const auto suggested_index = next_index + advance - 1;
            if(suggested_index < arguments.size())
            {
                return arguments[suggested_index];
            }
            else
            {
                return "";
            }
        }

        /////////////////////////////////////////////////////////////////////////////////////////
        // Extra

        Extra&
        Extra::MetaVar(const std::string& m)
        {
            arg->meta_var = m;
            return *this;
        }

        Extra&
        Extra::Help(const std::string& h)
        {
            arg->help = h;
            return *this;
        }

        /////////////////////////////////////////////////////////////////////////////////////////
        // SimpleParser

        template <>
        ParseResult
        SimpleParser<std::string>(
                std::string* target,
                const std::string&,
                const std::string& value,
                Output*)
        {
            *target = value;
            return ParseResult::Ok;
        }

        template <>
        ParseResult
        SimpleParser<bool>(
                bool*              target,
                const std::string& name,
                const std::string& value,
                Output*            output)
        {
            auto v = Trim(ToLower(value));
            if(v == "t" || v == "1" || v == "true" || v == "yes" || v == "y")
            {
                *target = true;
                return ParseResult::Ok;
            }
            if(v == "f" || v == "0" || v == "false" || v == "no" || v == "n")
            {
                *target = false;
                return ParseResult::Ok;
            }
            output->OnError(
                    Str() << value << " for boolean " << name
                          << " is not accepted.");
            return ParseResult::Failed;
        }


        /////////////////////////////////////////////////////////////////////////////////////////
        // HelpCallback

        struct HelpCallback : public Arg
        {
            ParseResult
            Parse(const std::string&, Running* running) override
            {
                running->parser->WriteLongHelp(running);
                return ParseResult::Quit;
            }

            std::string
            ToShortArgumentString() override
            {
                return "";
            }

            bool
            CanCallManyTimes() override
            {
                return false;
            }

            bool
            TakesArguments() override
            {
                return false;
            }
        };

        /////////////////////////////////////////////////////////////////////////////////////////
        // Parser

        Parser::Parser(const std::string& d) : documentation(d)
        {
            AddArgument("-h, --help", std::make_shared<HelpCallback>())
                    .Help("show this help message and exit");
        }

        void
        Parser::WriteShortHelp(Running* running) const
        {
            auto       pos_args             = GetAllPosArgs();
            auto*      o                    = running->output;
            const auto optional_string_list = VectorToStringVector(
                    optional_arguments_list,
                    [](std::shared_ptr<Arg> a) -> std::string {
                        return Str() << "[-" << a->name.names[0]
                                     << a->ToShortArgumentString() << "]";
                    });
            const auto positional_string_list = VectorToStringVector(
                    pos_args, [](std::shared_ptr<Arg> a) -> std::string {
                        return Str() << a->ToShortArgumentString();
                    });
            const auto optional_string
                    = StringMerger::Space().Generate(optional_string_list);
            const auto positional_string
                    = StringMerger::Space().Generate(positional_string_list);
            o->OnInfo(
                    Str() << "usage: " << running->name << " "
                          << optional_string << " " << positional_string);
        }

        void
        Parser::WriteLongHelp(Running* running) const
        {
            auto pos_args = GetAllPosArgs();
            WriteShortHelp(running);
            const auto empty_line = "";
            const auto indent     = "  ";
            auto*      o          = running->output;
            o->OnInfo(empty_line);
            if(!documentation.empty())
            {
                o->OnInfo(documentation);
                o->OnInfo(empty_line);
            }
            auto arg_name_string = [](std::shared_ptr<Arg> p) -> std::string {
                auto name_list = VectorToStringVector(
                        p->name.names,
                        [p](const std::string& n) -> std::string {
                            return Str()
                                   << "-" << n << p->ToShortArgumentString();
                        });
                auto name = StringMerger::Comma().Generate(name_list);
                return name;
            };

            const auto subps
                    = subparsers == nullptr
                              ? std::vector<
                                      std::pair<std::string, std::string>> {}
                              : Map<std::pair<std::string, std::string>>(
                                      subparsers->enum_to_string,
                                      [](auto p, auto s) {
                                          return std::make_pair(
                                                  s, p->parser->documentation);
                                      });

            // determine max_size
            size_t max_size = 0;
            for(auto p: pos_args)
            {
                if(p->show_in_long_description)
                {
                    max_size = std::max(max_size, p->name.names[0].length());
                }
            }
            for(auto p: optional_arguments_list)
            {
                if(p->show_in_long_description)
                {
                    max_size = std::max(max_size, arg_name_string(p).length());
                }
            }
            for(const auto& p: subps)
            {
                max_size = std::max(max_size, p.first.length());
            }
            max_size += 1;  // extra spacing
            max_size = std::min<size_t>(max_size, 30);

            auto pa = Filter(pos_args, [](auto p) {
                return p && p->show_in_long_description;
            });
            if(!pa.empty())
            {
                o->OnInfo("positional arguments");
                for(auto p: pa)
                {
                    o->OnInfo(
                            Str() << indent << std::left << std::setw(max_size)
                                  << p->name.names[0] << std::setw(0) << " "
                                  << p->help);
                }
                o->OnInfo(empty_line);
            }

            auto oa = Filter(optional_arguments_list, [](auto p) {
                return p && p->show_in_long_description;
            });
            if(!oa.empty())
            {
                o->OnInfo("optional arguments");
                for(auto p: oa)
                {
                    o->OnInfo(
                            Str() << indent << std::left << std::setw(max_size)
                                  << arg_name_string(p) << std::setw(0) << " "
                                  << p->help);
                }
                o->OnInfo(empty_line);
            }

            if(!subps.empty())
            {
                o->OnInfo("sub commands");
                for(auto p: subps)
                {
                    o->OnInfo(
                            Str()
                            << indent << std::left << std::setw(max_size)
                            << p.first << std::setw(0) << " " << p.second);
                }
                o->OnInfo(empty_line);
            }
        }

        struct SubParserArg : public Arg
        {
            std::shared_ptr<SubParsers> parsers;
            SubParserArg(std::shared_ptr<SubParsers> ps) : parsers(ps)
            {
                name                     = Name::Positional("sub");
                show_in_long_description = false;
            }

            ParseResult
            Parse(const std::string&, Running* running) override
            {
                ASSERT(parsers);
                ASSERT(running->HasMore());
                const auto name  = running->Read();
                auto       match = parsers->Match(name, 3);
                if(!match.single_match)
                {
                    auto matches = VectorToStringVector(
                            match.values, [this](std::shared_ptr<SubParser> p) {
                                return parsers->ToString(p);
                            });
                    running->output->OnError(
                            Str()
                            << name << " is invalid, could be "
                            << StringMerger::EnglishOr().Generate(matches));
                    return ParseResult::Failed;
                }

                std::vector<std::string> args;
                while(running->HasMore())
                {
                    args.emplace_back(running->Read());
                }

                auto sub = match.values[0];
                ASSERT(sub);
                auto r = sub->parser->Parse(running->name + " " + name, args);
                if(r == ParseResult::Ok)
                {
                    sub->callback();
                }
                return r;
            }

            std::string
            ToShortArgumentString() override
            {
                return StringMerger()
                        .StartAndEnd("{", "}")
                        .Separator(", ")
                        .Generate(parsers->ListNames());
            }

            bool
            CanCallManyTimes() override
            {
                return false;
            }

            bool
            TakesArguments() override
            {
                return true;
            }
        };

        std::vector<std::shared_ptr<Arg>>
        Parser::GetAllPosArgs() const
        {
            auto pos_args = the_positional_arguments;
            if(subparsers != nullptr)
            {
                pos_args.push_back(std::make_shared<SubParserArg>(subparsers));
            }
            return pos_args;
        }

        Extra
        Parser::AddArgument(const Name& name, std::shared_ptr<Arg> arg)
        {
            arg->name     = name;
            arg->meta_var = ToUpper(name.names[0]);
            // strip away all - from the start
            while(Name::IsOptional(arg->meta_var))
            {
                arg->meta_var = Name::OptionalName(arg->meta_var);
            }
            // if empty, just set to some default
            if(arg->meta_var.empty())
            {
                arg->meta_var = "VAR";
            }

            if(name.is_optional)
            {
                ASSERT(!name.names.empty());
                for(auto n: name.names)
                {
                    ASSERTX(optional_arguments.find(n)
                                    == optional_arguments.end(),
                            n);
                    optional_arguments[n] = arg;
                }
                optional_arguments_list.push_back(arg);
            }
            else
            {
                the_positional_arguments.push_back(arg);
            }

            return Extra {arg};
        }

        std::shared_ptr<SubParser>
        Parser::AddSubParser(const std::string& name, const std::string& desc)
        {
            if(subparsers == nullptr)
            {
                subparsers = std::make_shared<SubParsers>();
            }
            auto p    = std::make_shared<SubParser>();
            p->parser = std::make_shared<Parser>(desc);
            subparsers->Add(name, p);
            return p;
        }

        std::shared_ptr<Parser>
        Parser::AddSubParser(
                const std::string&  name,
                SubParser::Callback callback)
        {
            return AddSubParser(name, "", callback);
        }

        std::shared_ptr<Parser>
        Parser::AddSubParser(
                const std::string&  name,
                const std::string&  desc,
                SubParser::Callback callback)
        {
            auto r      = AddSubParser(name, desc);
            r->callback = callback;
            return r->parser;
        }

        Extra
        Parser::SetTrue(const Name& name, bool* b)
        {
            return AddSimpleFunction(name, [b]() { *b = true; });
        }

        Extra
        Parser::SetFalse(const Name& name, bool* b)
        {
            return AddSimpleFunction(name, [b]() { *b = false; });
        }

        FileOutput::FileOutput(const std::string& o) : file(o), single(!(EndsWith(o, "/") || EndsWith(o, "\\")))
        {}

        std::string
        FileOutput::NextFile(bool print)
        {
            if(single)
            {
                return file;
            }
            
            std::ostringstream ss;
            index += 1;
            if(print)
            {
                std::cout << "Generating " << index << "...\n";
            }
            ss << file << std::setfill('0') << std::setw(5) << index << ".png";
            return ss.str();
        }

        void
        FileOutput::CreateDirIfMissing() const
        {
            if(single) return;
            // std::filesystem::create_directories(file);
        }

        Args
        Args::Extract(int argc, char* argv[])
        {
            Args ret;
            ret.program_name = argv[0];
            for(int i = 1; i < argc; i += 1)
            {
                ret.args.emplace_back(argv[i]);
            }
            return ret;
        }

        ParseResult
        Parser::Parse(int argc, char* argv[]) const
        {
            const auto args = Args::Extract(argc, argv);
            return Parse(args);
        }

        ParseResult
        Parser::Parse(const Args& args) const
        {
            return Parse(args.program_name, args.args);
        }

        ParseResult
        Parser::Parse(
                const std::string&              program_name,
                const std::vector<std::string>& args) const
        {
            auto pos_args = GetAllPosArgs();
            auto console  = ConsoleOutput {};
            auto running
                    = Running {program_name, args, output ? output : &console};

            std::set<Arg*> read_arguments;
            auto           has_read
                    = [&read_arguments](std::shared_ptr<Arg> arg) -> bool {
                auto found = read_arguments.find(arg.get());
                return found != read_arguments.end();
            };
            auto read_arg = [&read_arguments](std::shared_ptr<Arg> arg) {
                auto inserted = read_arguments.insert(arg.get());
                ASSERT(inserted.second);
            };

            running.parser = this;

            size_t next_positional_index = 0;

            while(running.HasMore())
            {
                const auto is_optional = Name::IsOptional(running.Peek());
                const auto has_more_positionals
                        = next_positional_index < pos_args.size();

                std::string                       arg_name;
                std::shared_ptr<Arg>              arg = nullptr;
                std::vector<std::shared_ptr<Arg>> found_args;

                auto add_multi_argument
                        = [this, &arg, &arg_name, &found_args](
                                  const std::string& optional_name) -> bool {
                    const auto chars = Name::OptionalName(optional_name);
                    for(char c: chars)
                    {
                        auto f = optional_arguments.find(std::string(1, c));
                        if(f == optional_arguments.end()
                           || f->second->TakesArguments())
                        {
                            // if there aren't a matching cmdline argument name or it takes a argument it is an errror
                            break;
                        }
                        else
                        {
                            found_args.push_back(f->second);
                        }
                    }
                    if(chars.size() == found_args.size())
                    {
                        arg      = found_args[0];
                        arg_name = chars;
                        return true;
                    }
                    else
                    {
                        found_args.clear();
                        return false;
                    }
                };

                if(has_more_positionals)
                {
                    // if it is a valid optional, parse it, otherwise send to optional
                    if(is_optional)
                    {
                        const auto testing_arg = running.Peek();
                        auto       found       = optional_arguments.find(
                                Name::OptionalName(testing_arg));
                        if(found != optional_arguments.end())
                        {
                            if(!has_read(found->second))
                            {
                                arg_name = running.Read();
                                ASSERT(Name::IsOptional(arg_name));
                                arg = found->second;
                            }
                        }
                        else
                        {
                            if(add_multi_argument(testing_arg))
                            {
                                running.Read();
                            }
                        }
                    }

                    if(arg == nullptr)
                    {
                        arg = pos_args[next_positional_index];
                        ASSERT(!arg->name.names.empty());
                        arg_name = arg->name.names[0];
                        next_positional_index += 1;
                    }
                }
                else
                {
                    // no more positionals, must be a optional
                    const auto optional_name = running.Read();
                    if(!Name::IsOptional(optional_name))
                    {
                        running.output->OnError(
                                Str() << "Got " << optional_name
                                      << " but all positionals are consumed");
                        return ParseResult::Failed;
                    }

                    auto found = optional_arguments.find(
                            Name::OptionalName(optional_name));
                    if(found == optional_arguments.end())
                    {
                        if(false == add_multi_argument(optional_name))
                        {
                            running.output->OnError(
                                    Str() << "Not a valid argument: "
                                          << optional_name);
                            found_args.clear();
                            return ParseResult::Failed;
                        }
                    }
                    else
                    {
                        arg_name = optional_name;
                        arg      = found->second;
                    }
                }

                if(has_read(arg))
                {
                    running.output->OnError(
                            Str() << arg_name << " specified earlier");
                    return ParseResult::Failed;
                }

                auto handle_arg = [&running, &read_arg](
                                          std::shared_ptr<Arg> arg,
                                          const std::string&   arg_name) {
                    auto r = arg->Parse(arg_name, &running);
                    if(r != ParseResult::Ok)
                    {
                        return r;
                    }
                    if(arg->CanCallManyTimes() == false)
                    {
                        read_arg(arg);
                    }
                    return ParseResult::Ok;
                };
                if(found_args.empty())
                {
                    const auto r = handle_arg(arg, arg_name);
                    if(r != ParseResult::Ok)
                    {
                        return r;
                    }
                }
                else
                {
                    for(auto a: found_args)
                    {
                        const auto r = handle_arg(a, "");
                        if(r != ParseResult::Ok)
                        {
                            return r;
                        }
                    }
                }
            }

            std::vector<std::string> missing_positionals;
            for(size_t i = next_positional_index; i < pos_args.size(); i += 1)
            {
                auto names = pos_args[i]->name.names;
                ASSERT(!names.empty());
                auto name = names[0];
                missing_positionals.push_back(name);
            }

            if(!missing_positionals.empty())
            {
                running.output->OnError(
                        Str() << "error: the following arguments are required: "
                              << StringMerger::EnglishAnd().Generate(
                                         missing_positionals));
                WriteShortHelp(&running);
                return ParseResult::Failed;
            }

            return ParseResult::Ok;
        }
    }  // namespace argparse
}  // namespace euphoria::core
