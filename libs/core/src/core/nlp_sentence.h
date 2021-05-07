#pragma once

#include <vector>
#include <string>
#include <functional>
#include <istream>

namespace euphoria::core
{

using text_sentence = std::vector<std::string>;
using on_sentence = std::function<void(const text_sentence&)>;


bool
parse_sentences(std::istream& data, on_sentence on_sentence);


std::string
sentence_to_string(const text_sentence& s);

}
