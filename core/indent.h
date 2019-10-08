#ifndef CORE_INDENT_H
#define CORE_INDENT_H

#include <iostream>

namespace euphoria::core
{
    struct Indent
    {
        Indent()
        {
            ++val();
        }
        ~Indent()
        {
            --val();
        }
        static int&
        val()
        {
            static int v = 0;
            return v;
        }
        void
        print()
        {
            for(int i = 0; i < val(); ++i)
            {
                std::cout << "  ";
            }
        }
    };

#define DEBUG_SCOPE() Indent local_indent
#define DEBUG_PRINT(x)                                                         \
    do                                                                         \
    {                                                                          \
        local_indent.print();                                                  \
        std::cout << x << "\n";                                                \
    } while(false)


}  // namespace euphoria::core

#endif  // CORE_INDENT_H
