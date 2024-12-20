#pragma once



#include <optional>


namespace eu::core
{

using TextLine = std::vector<std::string>;


std::optional<TextLine>
parse_line(const std::string& line);


std::string
from_line_to_string(const TextLine& s);

}

