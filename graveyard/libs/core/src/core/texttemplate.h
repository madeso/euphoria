#pragma once

#include <map>
#include <memory>
#include <optional>


namespace eu::io
{
    struct FileSystem;
    struct FilePath;
}


namespace eu::core
{
    struct Defines
    {
        std::map<std::string, std::string> values;

        Defines();

        void remove(const std::string& name);
        void set(const std::string& name, const std::string& value);

        [[nodiscard]] bool is_defined(const std::string& name) const;
        [[nodiscard]] std::string get_value(const std::string& name) const;
    };

    struct TemplateErrorList
    {
        std::vector<std::string> errors;

        TemplateErrorList();

        void add_error
        (
            const std::optional<io::FilePath>& file,
            int line,
            int column,
            const std::string& error
        );

        [[nodiscard]] bool has_errors() const;
        [[nodiscard]] std::string get_combined_errors() const;
    };

    struct TemplateNodeList;

    struct CompiledTextTemplate
    {
        // todo(Gustav): move errors to load and evaluate return values
        TemplateErrorList errors;
        std::shared_ptr<TemplateNodeList> nodes;

        // todo(Gustav): move to a named constructor
        explicit CompiledTextTemplate(const std::string& text);
        CompiledTextTemplate(io::FileSystem* fs, const io::FilePath& path);
        ~CompiledTextTemplate();

        CompiledTextTemplate(const CompiledTextTemplate&) = delete;
        CompiledTextTemplate(CompiledTextTemplate&&) = delete;
        void operator=(const CompiledTextTemplate&) = delete;
        void operator=(CompiledTextTemplate&&) = delete;

        // todo(Gustav): why is this not const nodiscard?
        std::string build_string(const Defines& defines);
    };

}
