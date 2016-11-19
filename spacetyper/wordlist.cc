#include "spacetyper/wordlist.h"

#include <fstream>
#include <iostream>

Wordlist::Wordlist(const std::string& path) : generator_(std::random_device()()) {
  std::ifstream f(path.c_str());
  if( !f.good() ) {
    std::cerr << "Failed to load wordlist " << path << "\n";
  }
  std::string word;
  while( f >> word ){
    words_.push_back(word);
  }
}

const std::string& Wordlist::RandomWord() const {
  const size_t index = std::uniform_int_distribution<size_t>(0, words_.size())(generator_);
  return words_[index];
}
