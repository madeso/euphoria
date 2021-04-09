#ifndef EUPHORIA_CORE_NLP_LINE_H
#define EUPHORIA_CORE_NLP_LINE_H

#include <vector>
#include <string>
#include <optional>


namespace euphoria::core
{

using Line = std::vector<std::string>;


std::optional<Line>
ParseLine(const std::string& line);


std::string
LineToString(const Line& s);

}


#endif  // EUPHORIA_CORE_NLP_LINE_H
