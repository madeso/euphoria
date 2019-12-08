#include "core/ctree.h"
#include "core/textbox.h"

#include <vector>
#include <string>
#include <iostream>

using namespace euphoria::core;

struct T
{
    std::string name;
    std::vector<T> children;

    T(const std::string& n) : name(n) {}
    T(const std::string& n, std::initializer_list<T> t) : name(n), children(t) {}
};

int
main(int, char**)
{
    const auto simple_tree = T("a", { T{"1"} });

    const auto root_tree =
        T{"Tolkien characters", {
            T{"Heroes", {
                T{"Humans", {
                    T{"Aragon"},
                    T{"Boromir"}
                }},
                T{"Hobbits", {
                    T{"Frodo"},
                    T{"Samwise"},
                    T{"Merry"},
                    T{"Pippin"},
                }},
                T{"Other", {
                    T{"Legolas"},
                    T{"Gandalf"},
                    T{"Gimli"}
                }}
            }},
            T{"Enemies", {
                T{"Sauron"},
                T{"orcs"},
                T{"Sauruman"}
            }}
        }};

    const auto trees = std::vector<T>{simple_tree, root_tree};

    bool first = true;

    for(auto t: trees)
    {
        if(first) first = false;
        else std::cout << "\n-----------------------------------------\n\n";

        PrintHierarchy(t, [](const T& t) {return t.name; }, [](const T& t) {return t.children; }, [](const std::string& s) {std::cout << s << "\n"; });

        std::cout << "\n-----------------------------------------\n\n";

        {
            auto result = TextBox::Empty();
            result.PutBox(2,0, TextBox::CreateTreeGraph(t, 132-2,
                [](const T& e)
                {
                    return e.name;
                },
                [](const T& e) { return std::make_pair(e.children.cbegin(), e.children.cend()); },
                [](const T& e) { return e.children.size() >= 1; }, // whether simplified horizontal layout can be used
                [](const T&  ) { return true; }));
            auto strings = result.ToString();
            for(auto s: strings)
            {
                std::cout << s << "\n";
            }
        }
    }

    std::cout << "\n\n";
}