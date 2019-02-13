#include "core/stringtable.h"

struct Person
{
  std::string first_name;
  std::string last_name;
  int         age;
};

const std::vector<Person> persons
{
    // First name   Last name       Age
    {"Tinu",        "Elejogun",     14},
    {"Blaszczyk",   "Kostrzewski",  25},
    {"Lily",        "McGarrett",    18},
    {"Olatunkbo",   "Chijiaku",     22},
    {"Adrienne",    "Anthoula",     22},
    {"Axelia",      "Athanasios",   22},
    {"Jon-Kabat",   "Zinn",         22},
    {"Thabang",     "Mosoa",        15},
    {"Kgaogelo",    "Mosoa",        11},
};

int
main()
{
  const auto table =
      TableGenerator<Person>(persons)
          .Add(
              "Name",
              [](const Person& p) -> std::string {
                return p.first_name + " " + p.last_name;
              })
          .Add(
              "Age",
              [](const Person& p) -> std::string { return Str() << p.age; })
          .ToTable();
  ;
  PrintTable(std::cout, table);
  return 0;
}
