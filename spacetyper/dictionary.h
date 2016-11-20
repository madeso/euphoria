#ifndef SPACETYPER_DICTIONARY_H
#define SPACETYPER_DICTIONARY_H

#include "spacetyper/wordlist.h"

class Dictionary {
 public:
  Dictionary();

  std::string Generate() const;
 private:
  Wordlist adjectives_;
  Wordlist nouns_;
};

#endif  //  SPACETYPER_DICTIONARY_H