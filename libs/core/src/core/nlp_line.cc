#include "core/nlp_line.h"

#include "core/stringutils.h"
#include "core/stringmerger.h"

namespace euphoria::core
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
