#include "core/ctree.h"

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
    const auto simple_tree = T("a", { T{"1"}, T{"2"}, T{"3"} });

    const auto root_tree =
        T{"root", {
            T{"branch-A", {
                T{"sibling-X", {
                    T{"grandchild-A"},
                    T{"grandchild-B"}
                }},
                T{"sibling-Y", {
                    T{"grandchild-C"},
                    T{"grandchild-D"}
                }},
                T{"sibling-Z", {
                    T{"grandchild-E"},
                    T{"grandchild-F"}
                }}
            }},
            T{"branch-B", {
                T{"sibling-J"},
                T{"sibling-K"}
            }}
        }};

    const auto trees = std::vector<T>{simple_tree, root_tree};

    bool first = true;

    for(auto t: trees)
    {
        if(first) first = false;
        else std::cout << "\n-----------------------------------------\n\n";
        PrintHierarchy(t, [](const T& t) {return t.name; }, [](const T& t) {return t.children; }, [](const std::string& s) {std::cout << s << "\n"; });
    }

    return 0;
}
