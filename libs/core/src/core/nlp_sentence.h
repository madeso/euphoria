#pragma once

#include <vector>
#include <string>
#include <functional>
#include <istream>

namespace euphoria::core
{

using TextSentence = std::vector<std::string>;
using OnSentenceFunction = std::function<void(const TextSentence&)>;


bool
parse_sentences(std::istream& data, OnSentenceFunction on_sentence);


std::string
sentence_to_string(const TextSentence& s);

}
