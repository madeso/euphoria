#include "eu/runner/script.h"

#pragma once

#include "eu/io/file.h"

#include "lax/lax.h"
#include "lax/printhandler.h"

namespace eu::runner
{

    
struct PrintLaxError : lax::PrintHandler
{
    void on_line(std::string_view line) override
    {
        LOG_ERR("> {0}", line);
    }
};


Script:: Script()
: lax(std::make_unique<PrintLaxError>(), [](const std::string& str) { LOG_INFO("> {0}", str); })
{
}

bool Script::run_file(const std::string& file)
{
    const auto content = io::string_from_file("main.lax");
    const auto result = lax.run_string(content);

    if(result == false)
    {
        LOG_ERR("Failed to run lax file {}", file);
    }

    return result;
}


}
