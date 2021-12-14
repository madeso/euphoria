#include "core/ctree.h"
#include "core/textbox.h"

#include <vector>
#include <string>
#include <iostream>

using namespace euphoria::core;

struct Tree
{
    std::string name;
    std::vector<Tree> children;

    Tree(const std::string& n) : name(n) {}
    Tree(const std::string& n, std::initializer_list<Tree> t) : name(n), children(t) {}
};

int
main(int, char**)
{
    const auto simple_tree = Tree{"a", { Tree{"1"} }};

    const auto root_tree = Tree
    {
        "Tolkien characters",
        {
            Tree
            {
                "Heroes",
                {
                    Tree
                    {
                        "Humans",
                        {
                            Tree{"Aragon"},
                            Tree{"Boromir"}
                        }
                    },
                    Tree
                    {
                        "Hobbits",
                        {
                            Tree{"Frodo"},
                            Tree{"Samwise"},
                            Tree{"Merry"},
                            Tree{"Pippin"},
                        }
                    },
                    Tree
                    {
                        "Other",
                        {
                            Tree{"Legolas"},
                            Tree{"Gandalf"},
                            Tree{"Gimli"}
                        }
                    }
                }
            },
            Tree
            {
                "Enemies",
                {
                    Tree{"Sauron"},
                    Tree{"orcs"},
                    Tree{"Sauruman"}
                }
            }
        }
    };

    const auto trees = std::vector<Tree>{simple_tree, root_tree};

    bool first = true;

    for(auto tree_to_print: trees)
    {
        if(first) { first = false; }
        else { std::cout << "\n-----------------------------------------\n\n"; }

        print_hierarchy
        (
            tree_to_print,
            [](const Tree& tt) {return tt.name; },
            [](const Tree& tt) {return tt.children; },
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
                    [](const Tree& e)
                    {
                        return e.name;
                    },
                    [](const Tree& e) { return std::make_pair(e.children.cbegin(), e.children.cend()); },
                    [](const Tree& e) { return !e.children.empty(); }, // whether simplified horizontal layout can be used
                    [](const Tree& ) { return true; }
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
