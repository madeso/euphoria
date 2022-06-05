#pragma once



#include <optional>


namespace euphoria::core
{

using TextLine = std::vector<std::string>;


std::optional<TextLine>
parse_line(const std::string& line);


std::string
line_to_string(const TextLine& s);

}

