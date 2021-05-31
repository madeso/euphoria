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
    struct file_system;
    struct file_path;
}

namespace euphoria::core
{
    struct defines
    {
        defines();

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

    struct template_error_list
    {
        template_error_list();

        [[nodiscard]] bool
        has_errors() const;

        void
        add_error
        (
            const std::optional<vfs::file_path>& file,
            int line,
            int column,
            const std::string& error
        );

        [[nodiscard]] std::string
        get_combined_errors() const;

        std::vector<std::string> errors;
    };

    struct template_node_list;

    struct compiled_text_template
    {
        // todo(Gustav): move to a named constructor
        explicit compiled_text_template(const std::string& text);
        compiled_text_template(vfs::file_system* fs, const vfs::file_path& path);

        ~compiled_text_template();

        NONCOPYABLE(compiled_text_template);

        std::string
        evaluate(const defines& defines);

        // todo(Gustav): move errors to load and evaluate return values
        template_error_list errors;
        std::shared_ptr<template_node_list> nodes;
    };

}
