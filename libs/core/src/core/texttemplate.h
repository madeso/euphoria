#pragma once

#include <map>


#include <memory>

#include <optional>

#include "core/noncopyable.h"

namespace euphoria::core::vfs
{
    struct FileSystem;
    struct FilePath;
}

namespace euphoria::core
{
    struct Defines
    {
        Defines();

        [[nodiscard]] bool
        is_defined(const std::string& name) const;

        [[nodiscard]] std::string
        get_value(const std::string& name) const;

        void
        undefine(const std::string& name);

        void
        define(const std::string& name, const std::string& value);

        std::map<std::string, std::string> values;
    };

    struct TemplateErrorList
    {
        TemplateErrorList();

        [[nodiscard]] bool
        has_errors() const;

        void
        add_error
        (
            const std::optional<vfs::FilePath>& file,
            int line,
            int column,
            const std::string& error
        );

        [[nodiscard]] std::string
        get_combined_errors() const;

        std::vector<std::string> errors;
    };

    struct TemplateNodeList;

    struct CompiledTextTemplate
    {
        // todo(Gustav): move to a named constructor
        explicit CompiledTextTemplate(const std::string& text);
        CompiledTextTemplate(vfs::FileSystem* fs, const vfs::FilePath& path);

        ~CompiledTextTemplate();

        NONCOPYABLE(CompiledTextTemplate);

        std::string
        evaluate(const Defines& defines);

        // todo(Gustav): move errors to load and evaluate return values
        TemplateErrorList errors;
        std::shared_ptr<TemplateNodeList> nodes;
    };

}
