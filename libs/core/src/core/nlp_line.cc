#include "core/nlp_line.h"

#include "base/stringutils.h"
#include "base/stringmerger.h"

namespace eu::core
{

std::optional<TextLine>
parse_line(const std::string& line)
{
    return split(line, ' ');
}


std::string
from_line_to_string(const TextLine& s)
{
    return string_mergers::space.merge(s);
}

}
