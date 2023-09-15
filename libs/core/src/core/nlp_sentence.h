#pragma once



#include <functional>
#include <istream>

namespace eu::core
{

using TextSentence = std::vector<std::string>;
using OnSentenceFunction = std::function<void(const TextSentence&)>;


bool
parse_sentences(std::istream& data, OnSentenceFunction on_sentence);


std::string
from_sentence_to_string(const TextSentence& s);

}
