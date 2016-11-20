#include "spacetyper/dictionary.h"

Dictionary::Dictionary() : adjectives_("wordlist_adjective.txt"), nouns_("wordlist_noun.txt") {
}

std::string Dictionary::Generate() const {
  return adjectives_.RandomWord() + " " + nouns_.RandomWord();
}
