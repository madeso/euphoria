#include "core/nlp_line.h"

#include "core/stringutils.h"
#include "core/stringmerger.h"

namespace euphoria::core
{

std::optional<Line>
ParseLine(const std::string& line)
{
    return Split(line, ' ');
}


std::string
LineToString(const Line& s)
{
    return StringMerger::Space().Generate(s);
}

}
