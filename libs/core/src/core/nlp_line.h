#pragma once

#include <vector>
#include <string>
#include <optional>


namespace euphoria::core
{

using text_line = std::vector<std::string>;


std::optional<text_line>
parse_line(const std::string& line);


std::string
line_to_string(const text_line& s);

}

