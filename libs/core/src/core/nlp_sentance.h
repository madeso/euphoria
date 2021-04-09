#ifndef EUPHORIA_CORE_NLP_SENTANCE_H
#define EUPHORIA_CORE_NLP_SENTANCE_H

#include <vector>
#include <string>
#include <functional>
#include <istream>

namespace euphoria::core
{

using Sentance = std::vector<std::string>;
using OnSentance = std::function<void(const Sentance&)>;


bool
ParseSentances(std::istream& data, OnSentance on_sentance);


std::string
SentanceToString(const Sentance& s);

}

#endif  // EUPHORIA_CORE_NLP_SENTANCE_H
