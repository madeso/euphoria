#include "core/ctree.h"
#include "core/textbox.h"

#include <vector>
#include <string>
#include <iostream>

using namespace euphoria::core;

struct t
{
    std::string name;
    std::vector<t> children;

    t(const std::string& n) : name(n) {}
    t(const std::string& n, std::initializer_list<t> t) : name(n), children(t) {}
};

int
main(int, char**)
{
    const auto simple_tree = t{"a", { t{"1"} }};

    const auto root_tree = t
    {
        "Tolkien characters",
        {
            t
            {
                "Heroes",
                {
                    t
                    {
                        "Humans",
                        {
                            t{"Aragon"},
                            t{"Boromir"}
                        }
                    },
                    t
                    {
                        "Hobbits",
                        {
                            t{"Frodo"},
                            t{"Samwise"},
                            t{"Merry"},
                            t{"Pippin"},
                        }
                    },
                    t
                    {
                        "Other",
                        {
                            t{"Legolas"},
                            t{"Gandalf"},
                            t{"Gimli"}
                        }
                    }
                }
            },
            t
            {
                "Enemies",
                {
                    t{"Sauron"},
                    t{"orcs"},
                    t{"Sauruman"}
                }
            }
        }
    };

    const auto trees = std::vector<t>{simple_tree, root_tree};

    bool first = true;

    for(auto tree_to_print: trees)
    {
        if(first) { first = false; }
        else { std::cout << "\n-----------------------------------------\n\n"; }

        print_hierarchy
        (
            tree_to_print,
            [](const t& tt) {return tt.name; },
            [](const t& tt) {return tt.children; },
            [](const std::string& s) {std::cout << s << "\n"; }
        );

        std::cout << "\n-----------------------------------------\n\n";

        {
            auto result = TextBox::create_empty();
            result.put_box
            (
                2,0,
                TextBox::create_tree_graph
                (
                    tree_to_print,
                    132-2,
                    [](const t& e)
                    {
                        return e.name;
                    },
                    [](const t& e) { return std::make_pair(e.children.cbegin(), e.children.cend()); },
                    [](const t& e) { return !e.children.empty(); }, // whether simplified horizontal layout can be used
                    [](const t& ) { return true; }
                )
            );

            auto strings = result.to_string();
            for(auto s: strings)
            {
                std::cout << s << "\n";
            }
        }
    }

    std::cout << "\n\n";
}
