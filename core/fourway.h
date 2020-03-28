#ifndef EUPHORIA_CORE_FOURWAY_H
#define EUPHORIA_CORE_FOURWAY_H

#include <vector>
#include <optional>
#include <string>
#include <sstream>

#include "core/custom_parser.h"
#include "core/stringutils.h"


namespace euphoria::core
{
    template<typename T>
    struct Fourway
    {
        T left;
        T right;
        T up;
        T down;

        explicit Fourway(const T& t)
            : left(t)
            , right(t)
            , up(t)
            , down(t)
        {
        }

        Fourway
        (
            const T& horizontal,
            const T& vertical
        )
            : left(horizontal)
            , right(horizontal)
            , up(vertical)
            , down(vertical)
        {
        }

        Fourway
        (
            const T& l,
            const T& r,
            const T& u,
            const T& d
        )
            : left(l)
            , right(r)
            , up(u)
            , down(d)
        {
        }
    };


    template<typename T>
    struct CustomArgparser
    <Fourway<T>>
    {
        enum { value = 1 };

        static constexpr char SPLIT = '/';

        static
        std::string
        ToString(const Fourway<T>& fw)
        {
            std::ostringstream ss;
            ss << fw.up << SPLIT << fw.right << SPLIT << fw.down << SPLIT << fw.left;
            return ss.str();
        }

        static
        Result<Fourway<T>>
        Parse(const std::string&, const std::string& value)
        {
            using R = Result<Fourway<T>>;
            auto parse = [](const std::string& v) -> std::optional<T>
            {
                std::istringstream ss{v};
                T t;
                ss >> t;
                if(ss.fail() || !ss.eof())
                {
                    return std::nullopt;
                }
                else
                {
                    return t;
                }
            };
            const auto values = Split(value, SPLIT);
            switch(values.size())
            {
                case 1:
                {
                    const auto val = parse(values[0]);
                    if(!val) { return R::False("invalid value"); }
                    return R::True(Fourway<T>{*val});
                }
                case 2:
                {
                    const auto hor = parse(values[1]);
                    const auto vert = parse(values[0]);
                    if(!hor) { return R::False("invalid hor"); }
                    if(!vert) { return R::False("invalid vert"); }
                    return R::True(Fourway<T>{*hor, *vert});
                }
                case 4:
                {
                    const auto up = parse(values[0]);
                    const auto right = parse(values[1]);
                    const auto down = parse(values[2]);
                    const auto left = parse(values[2]);
                    if(!left) { return R::False("invalid left"); }
                    if(!right) { return R::False("invalid right"); }
                    if(!up) { return R::False("invalid up"); }
                    if(!down) { return R::False("invalid down"); }
                    return R::True(Fourway<T>{*left, *right, *up, *down});
                }
                default:
                {
                    return R::False("wrong number of splits in fourway");
                }
            }
        }
    };

}

#endif  // EUPHORIA_CORE_FOURWAY_H
