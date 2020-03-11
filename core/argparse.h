#ifndef CORE_ARGPARSE_H
#define CORE_ARGPARSE_H

#include <vector>
#include <string>
#include <memory>
#include <map>
#include <functional>

#include "core/str.h"
#include "core/enumtostring.h"
#include "core/stringmerger.h"

// todo: mutiple names for subcommands
// todo: groups
// todo: examples that are validated during debug builds
// todo: boolean attributes, [--value | --no-value] style
// todo: break at 80/120 character width

namespace euphoria::core::argparse
{
    struct Parser;

    /////////////////////////////////////////////////////////////////////////////////////////
    // Enum

    enum class Greedy
    {
        Yes,
        No
    };

    enum class ParseResult
    {
        // parsing was successfull, use arguments
        Ok,

        // some argumetns failed, please exit
        Failed,

        // parsing was successfull, but exit was requested, please exit
        Quit,
    };

    /////////////////////////////////////////////////////////////////////////////////////////
    // Util

    struct Name
    {
        [[nodiscard]] static bool
        IsOptional(const std::string& name);

        [[nodiscard]] static std::string
        OptionalName(const std::string& name);

        Name();
        Name(const char* n);

        bool                     is_optional;
        std::vector<std::string> names;

        [[nodiscard]] static Name
        Parse(const std::string& n);

        [[nodiscard]] static Name
        Optional(std::initializer_list<std::string> names);

        [[nodiscard]] static Name
        Positional(const std::string& names);

    private:
        Name(bool o, const std::vector<std::string>& n);

#ifdef _DEBUG
        void
        AssertValid();
#endif
    };

    struct Output
    {
        virtual ~Output() {}

        virtual void
        OnError(const std::string& err)
                = 0;
        virtual void
        OnInfo(const std::string& info)
                = 0;
    };

    struct ConsoleOutput : public Output
    {
        void
        OnError(const std::string& err) override;
        void
        OnInfo(const std::string& info) override;
    };

    struct Running
    {
        std::string              name;
        std::vector<std::string> arguments;
        Output*                  output = nullptr;
        const Parser*            parser = nullptr;

        size_t next_index = 0;

        bool
        HasMore() const;

        std::string
        Read();

        std::string
        Peek(size_t advance = 1) const;
    };

    struct Arg
    {
        Name        name;
        std::string meta_var;
        std::string help;

        bool show_in_long_description = true;

        virtual ~Arg() {}

        virtual ParseResult
        Parse(const std::string& name, Running* running)
                = 0;

        virtual std::string
        ToShortArgumentString()
                = 0;

        virtual bool
        CanCallManyTimes()
                = 0;

        virtual bool
        TakesArguments()
                = 0;
    };

    struct Extra
    {
        std::shared_ptr<Arg> arg;

        Extra&
        MetaVar(const std::string& m);

        Extra&
        Help(const std::string& h);
    };

    template <typename T>
    ParseResult
    SimpleParser(
            T*                 target,
            const std::string& name,
            const std::string& value,
            Output*            output)
    {
        std::stringstream ss(value);
        T                 t;
        ss >> t;
        if(ss.fail() || !ss.eof())
        {
            output->OnError(
                    Str() << value << " for " << name << " is not accepted.");
            return ParseResult::Failed;
        }
        else
        {
            *target = t;
            return ParseResult::Ok;
        }
    }

    template <>
    ParseResult
    SimpleParser<std::string>(
            std::string* target,
            const std::string&,
            const std::string& value,
            Output*);

    template <>
    ParseResult
    SimpleParser<bool>(
            bool* target,
            const std::string&,
            const std::string& value,
            Output*);

    template <typename T>
    struct SimpleArg : public Arg
    {
        T* target;

        ParseResult
        Parse(const std::string& cmd_name, Running* running) override
        {
            if(running->HasMore())
            {
                const auto value = running->Read();
                // todo: add validator (InRange, etc...)
                return SimpleParser<T>(
                        target, cmd_name, value, running->output);
            }
            else
            {
                running->output->OnError(Str() << cmd_name << " missing value");
                return ParseResult::Failed;
            }
        }

        bool
        CanCallManyTimes() override
        {
            return false;
        }

        std::string
        ToShortArgumentString() override
        {
            return Str() << " " << meta_var;
        }

        bool
        TakesArguments() override
        {
            return true;
        }
    };

    template <typename T>
    struct EnumArg : public Arg
    {
        T* target;

        ParseResult
        Parse(const std::string& cmd_name, Running* running) override
        {
            if(running->HasMore())
            {
                const auto value = running->Read();
                const auto match = StringToEnum<T>(value);
                if(match.single_match)
                {
                    *target = match.values[0];
                    return ParseResult::Ok;
                }
                else
                {
                    const auto could_be = EnumToString(match.values);
                    running->output->OnError(
                            Str()
                            << value << " is invalid, could be "
                            << StringMerger::EnglishOr().Generate(could_be));
                    return ParseResult::Failed;
                }
            }
            else
            {
                running->output->OnError(Str() << cmd_name << " missing value");
                return ParseResult::Failed;
            }
        }

        std::string
        ToShortArgumentString() override
        {
            return StringMerger()
                    .Separator(" | ")
                    .StartAndEnd("{", "}")
                    .Generate(EnumToString<T>());
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

    template <typename T>
    struct FunctionArgument : public Arg
    {
        T t;

        FunctionArgument(T tt) : t(tt) {}
        ParseResult
        Parse(const std::string&, Running*) override
        {
            t();
            return ParseResult::Ok;
        }

        std::string
        ToShortArgumentString() override
        {
            return "";
        }

        bool
        CanCallManyTimes() override
        {
            return true;
        }

        bool
        TakesArguments() override
        {
            return false;
        }
    };

    template <typename T>
    struct CountArg : public Arg
    {
        T* target;

        ParseResult
        Parse(const std::string&, Running*) override
        {
            (*target)++;
            return ParseResult::Ok;
        }

        std::string
        ToShortArgumentString() override
        {
            return "";
        }

        bool
        CanCallManyTimes() override
        {
            return true;
        }

        bool
        TakesArguments() override
        {
            return false;
        }
    };

    template <typename T>
    struct VectorArg : public Arg
    {
        std::vector<T>* target;
        Greedy          greedy = Greedy::No;

        ParseResult
        ParseOne(const std::string& cmd_name, Running* running)
        {
            if(!running->HasMore())
            {
                running->output->OnError(Str() << cmd_name << " missing value");
                return ParseResult::Failed;
            }
            const auto value = running->Read();
            T          t;
            const auto r
                    = SimpleParser<T>(&t, cmd_name, value, running->output);
            if(r == ParseResult::Ok)
            {
                // todo: add validator (InRange, etc...)
                target->emplace_back(t);
            }
            return r;
        }

        ParseResult
        Parse(const std::string& cmd_name, Running* running) override
        {
            if(greedy == Greedy::Yes)
            {
                while(running->HasMore())
                {
                    auto r = ParseOne(cmd_name, running);
                    if(r != ParseResult::Ok)
                    {
                        return r;
                    }
                }

                return ParseResult::Ok;
            }
            else
            {
                return ParseOne(cmd_name, running);
            }
        }

        std::string
        ToShortArgumentString() override
        {
            if(greedy == Greedy::Yes)
            {
                return Str() << " " << meta_var << "+";
            }
            else
            {
                return Str() << "+ " << meta_var;
            }
        }

        bool
        CanCallManyTimes() override
        {
            // if it isn't greedy we should be able to call it many times
            // if it is greedy we don't need to call it many times
            return greedy == Greedy::No;
        }

        bool
        TakesArguments() override
        {
            return true;
        }
    };

    struct SubParser
    {
        using Callback = std::function<void(void)>;
        std::shared_ptr<Parser> parser;
        Callback                callback;
    };

    using SubParsers = EnumToStringImpl<std::shared_ptr<SubParser>>;

    /////////////////////////////////////////////////////////////////////////////////////////

    struct FileOutput
    {
        explicit FileOutput(const std::string& o);

        std::string
        NextFile(bool print = true);

        void
        CreateDirIfMissing() const;

        std::string file;
        bool        single;
        int         index = 0;
    };

    /////////////////////////////////////////////////////////////////////////////////////////
    // Main

    struct Args
    {
        std::string              program_name;
        std::vector<std::string> args;

        static Args
        Extract(int argc, char* argv[]);
    };

    struct Parser
    {
        explicit Parser(const std::string& d);

        void
        WriteShortHelp(Running* running) const;

        void
        WriteLongHelp(Running* running) const;

        // todo: add subparser
        // todo: get inspiration from
        // https://stackoverflow.com/questions/491595/best-way-to-parse-command-line-arguments-in-c
        // https://github.com/clap-rs/clap
        // https://docs.python.org/3/library/argparse.html

        std::string documentation;

        // if null, use standard console output
        Output* output = nullptr;

        std::map<std::string, std::shared_ptr<Arg>> optional_arguments;
        std::vector<std::shared_ptr<Arg>>           optional_arguments_list;
        std::vector<std::shared_ptr<Arg>>           the_positional_arguments;
        std::shared_ptr<SubParsers>                 subparsers;

        std::vector<std::shared_ptr<Arg>>
        GetAllPosArgs() const;

        Extra
        AddArgument(const Name& name, std::shared_ptr<Arg> arg);

        std::shared_ptr<SubParser>
        AddSubParser(const std::string& name, const std::string& desc);

        std::shared_ptr<Parser>
        AddSubParser(const std::string& name, SubParser::Callback callback);

        std::shared_ptr<Parser>
        AddSubParser(
                const std::string&  name,
                const std::string&  desc,
                SubParser::Callback callback);

        template <typename T>
        Extra
        AddSimple(const Name& name, T* var)
        {
            auto a    = std::make_shared<SimpleArg<T>>();
            a->target = var;
            return AddArgument(name, a);
        }

        template <typename T>
        Extra
        AddCount(const Name& name, T* var)
        {
            auto a    = std::make_shared<CountArg<T>>();
            a->target = var;
            return AddArgument(name, a);
        }

        // todo: use enable_if to merge call with AddSimple above
        template <typename T>
        Extra
        AddEnum(const Name& name, T* var)
        {
            auto a    = std::make_shared<EnumArg<T>>();
            a->target = var;
            return AddArgument(name, a);
        }

        template <typename T>
        Extra
        AddSimpleFunction(const Name& name, T callback)
        {
            auto a = std::make_shared<FunctionArgument<T>>(callback);
            return AddArgument(name, a);
        }

        Extra
        SetTrue(const Name& name, bool* b);

        Extra
        SetFalse(const Name& name, bool* b);

        template <typename T>
        Extra
        AddVector(const Name& name, std::vector<T>* vec)
        {
            auto a    = std::make_shared<VectorArg<T>>();
            a->target = vec;
            a->greedy = name.is_optional ? Greedy::No : Greedy::Yes;
            return AddArgument(name, a);
        }

        ParseResult
        Parse(int argc, char* argv[]) const;

        ParseResult
        Parse(const Args& args) const;

        ParseResult
        Parse(const std::string&              program_name,
              const std::vector<std::string>& args) const;
    };

}  // namespace euphoria::core::argparse

#endif  // CORE_ARGPARSE_H
