#ifndef EUPHORIA_TEMPLATE_H
#define EUPHORIA_TEMPLATE_H

#include <map>
#include <string>
#include <sstream>
#include <memory>
#include <vector>

#include "core/vfs.h"

namespace euphoria::core
{
    class Defines
    {
    public:
        Defines();

        bool
        IsDefined(const std::string& name) const;
        std::string
        GetValue(const std::string& name) const;

        void
        Undefine(const std::string& name);
        void
        Define(const std::string& name, const std::string& value);

    private:
        std::map<std::string, std::string> values_;
    };

    class TemplateError
    {
    public:
        TemplateError();

        bool
        HasErrors() const;
        void
        AddError(
                const std::string& file,
                int                line,
                int                column,
                const std::string& error);

        std::string
        GetCombinedErrors() const;

    private:
        std::vector<std::string> errors_;
    };

    class TemplateNodeList;

    class Template
    {
    public:
        explicit Template(const std::string& text);
        Template(vfs::FileSystem* fs, const std::string& path);
        ~Template();

        std::string
        Evaluate(const Defines& defines);
        const TemplateError&
        GetErrors() const;

    private:
        TemplateError                     errors_;
        std::shared_ptr<TemplateNodeList> nodes_;
    };

}  // namespace euphoria::core

#endif  // EUPHORIA_TEMPLATE_H
