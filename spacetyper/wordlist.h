#ifndef SPACETYPER_WORDLIST_H
#define SPACETYPER_WORDLIST_H

#include <string>
#include <vector>
#include <random>

class Wordlist {
 public:
  Wordlist(const std::string& path);

  const std::string& RandomWord() const;

 private:
  mutable std::mt19937 generator_;
  std::vector<std::string> words_;
};

#endif  // SPACETYPER_WORDLIST_H
