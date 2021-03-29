#pragma once

#include <map>
#include <string>
#include <sstream>
#include <memory>
#include <vector>
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
        IsDefined(const std::string& name) const;

        [[nodiscard]] std::string
        GetValue(const std::string& name) const;

        void
        Undefine(const std::string& name);

        void
        Define(const std::string& name, const std::string& value);

        std::map<std::string, std::string> values;
    };

    struct TemplateErrorList
    {
        TemplateErrorList();

        [[nodiscard]] bool
        HasErrors() const;

        void
        AddError
        (
            const std::optional<vfs::FilePath>& file,
            int line,
            int column,
            const std::string& error
        );

        [[nodiscard]] std::string
        GetCombinedErrors() const;

        std::vector<std::string> errors;
    };

    struct TemplateNodeList;

    struct Template
    {
        explicit Template(const std::string& text);
        Template(vfs::FileSystem* fs, const vfs::FilePath& path);
        ~Template();

        NONCOPYABLE(Template);

        std::string
        Evaluate(const Defines& defines);

        TemplateErrorList errors;

        std::shared_ptr<TemplateNodeList> nodes;
    };

}
