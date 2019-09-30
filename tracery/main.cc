#include "core/tracery.h"
#include <iostream>
#include <fstream>

namespace tracery = euphoria::core::tracery;

tracery::Result LoadFromFile(tracery::Grammar* grammar, const std::string& file)
{
    std::ifstream       t(file);
    if(t.good() == false)
    {
      return tracery::Result(tracery::Result::UNABLE_TO_OPEN_FILE) << file;
    }
    std::string data(
        (std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
    return grammar->LoadFromString(data);
}

int
main(int argc, char* argv[])
{
  if(argc >= 2)
  {
    tracery::Grammar grammar;

    const std::string file = argv[1];

    grammar.RegisterEnglish();

    const auto r = LoadFromFile(&grammar, file);
    if(r == false)
    {
      std::cerr << r << "\n";
      return 3;
    }

    const std::string rule  = (argc >= 3) ? argv[2] : "#origin#";
    const int         count = (argc >= 4) ? atoi(argv[3]) : 1;
    for(int i = 0; i < count; ++i)
    {
      const auto r = grammar.Flatten(rule);
      if(r == false)
      {
        std::cerr << r;
        return 2;
      }
      std::cout << r.GetText() << "\n";
    }
  }
  else
  {
    std::cerr << "Expected input!\n";
    std::cerr << argv[0] << " json-file [rule] [count]\n";
  }
  return 0;
}
