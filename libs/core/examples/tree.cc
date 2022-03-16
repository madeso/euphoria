#include "core/ctree.h"
#include "core/textbox.h"

#include "core/argparse.h"
#include "core/dotoutput.h"

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


Tree
create_simple_tree()
{
    return Tree{"a", { Tree{"1"} }};
}


Tree
create_tolkien_tree()
{
    return Tree
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
                    Tree{"Orcs"},
                    Tree{"Sauruman"}
                }
            }
        }
    };
}


void
print_tree_simple(const Tree& tree_to_print)
{
    print_hierarchy
    (
        tree_to_print,
        [](const Tree& tt) {return tt.name; },
        [](const Tree& tt) {return tt.children; },
        [](const std::string& s) {std::cout << s << "\n"; }
    );
}


void
print_tree_dot(const Tree& tree_to_print)
{
    dot::create_graph
    (
        tree_to_print,
        [](const Tree& tt) {return tt.name; },
        [](const Tree& tt) {return tt.children; }
    ).print();
}


void
print_tree_textbox(const Tree& tree_to_print)
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


void
print_line()
{
    std::cout << "\n-----------------------------------------\n\n";
}


enum class Trees
{
    all, simple, tolkien
};


enum class Output
{
    all, simple, textbox, dot
};


struct TreeApp
{
    bool first_divide = true;

    void
    print_divide()
    {
        if(first_divide) { first_divide = false; }
        else { print_line(); }
    }

    void print_tree(const Tree& tree, Output what_output)
    {
        print_divide();
        switch(what_output)
        {
        case Output::all:
            print_tree_simple(tree);
            print_divide();
            print_tree_textbox(tree);
            break;
        case Output::simple:
            print_tree_simple(tree);
            break;
        case Output::textbox:
            print_tree_textbox(tree);
            break;
        case Output::dot:
            print_tree_dot(tree);
            break;
        }
    }

    void
    run_tree(Trees what_trees, Output what_output)
    {
        switch(what_trees)
        {
        case Trees::all:
            print_tree(create_simple_tree(), what_output);
            print_divide();
            print_tree(create_tolkien_tree(), what_output);
            break;
        case Trees::simple:
            print_tree(create_simple_tree(), what_output);
            break;
        case Trees::tolkien:
            print_tree(create_tolkien_tree(), what_output);
            break;
        }
    }
};


int
main(int argc, char** argv)
{
    auto trees = Trees::all;
    auto output = Output::all;

    auto parser = argparse::Parser {"tree example"};
    parser.add("--tree", &trees).set_help("What trees to print");
    parser.add("--output", &output).set_help("How to print the trees");

    if(const auto r = parser.parse(argc, argv))
    {
        return *r;
    }

    auto app = TreeApp{};
    app.run_tree(trees, output);
    std::cout << "\n\n";

    return 0;
}
