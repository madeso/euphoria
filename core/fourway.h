#ifndef EUPHORIA_CORE_FOURWAY_H
#define EUPHORIA_CORE_FOURWAY_H

#include <vector>
#include <optional>
#include <string>
#include <sstream>

#include "core/custom_parser.h"
#include "core/stringutils.h"
#include "core/argparse.h"


namespace euphoria::core
{
    /**
      Generic version of a CSS like padding type.
    */
    template<typename T>
    struct Fourway
    {
        using Self = Fourway<T>;

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

        /** Create a new fourway from left-righ and up-down.
          */
        static
        Self
        FromLrud(const T& lr, const T& ud)
        {
            return {lr, lr, ud, ud};
        }

        /** Create a new fourway from left right up and down.
          */
        static
        Self
        FromLrud(const T& l, const T& r, const T& u, const T& d)
        {
            return {l, r, u, d};
        }

    private:
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
    bool
    operator==(const Fourway<T>& lhs, const Fourway<T>& rhs)
    {
        return
            lhs.left == rhs.left &&
            lhs.right == rhs.right &&
            lhs.up == rhs.up &&
            lhs.down == rhs.down
            ;
    }


    template<typename T>
    bool
    operator!=(const Fourway<T>& lhs, const Fourway<T>& rhs)
    {
        return !(lhs == rhs);
    }


    template<typename T>
    std::ostream&
    operator<<(std::ostream& s, const Fourway<T>& fw)
    {
        s << "("
          << fw.up << " "
          << fw.right << " "
          << fw.down << " "
          << fw.left
          << ")"
        ;
        return s;
    }


    /** Parses a fourway according to the CSS spec.
      Either all, ver/hor or up/right/down/left
    */
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
        Parse(const std::string& value)
        {
            using R = Result<Fourway<T>>;
            auto parse = [](const std::string& v)
            {
                return argparse::DefaultParseFunction<T>(v);
            };
            const auto values = Split(value, SPLIT);
            switch(values.size())
            {
                case 1:
                {
                    const auto val = parse(values[0]);

                    if(!val) { return R::False(val.Error()); }

                    return R::True(Fourway<T>{*val});
                }
                case 2:
                {
                    const auto vvert = values[0];
                    const auto vhor = values[1];

                    const auto vert = parse(vvert);
                    const auto hor = parse(vhor);

                    if(!hor) { return R::False(Str() << "invalid hor(" << vhor << "): " << hor.Error()); }
                    if(!vert) { return R::False(Str() << "invalid vert(" << vvert << "): " << vert.Error()); }

                    return R::True(Fourway<T>::FromLrud(*hor, *vert));
                }
                case 4:
                {
                    const auto vup = values[0];
                    const auto vright = values[1];
                    const auto vdown = values[2];
                    const auto vleft = values[3];

                    const auto up = parse(vup);
                    const auto right = parse(vright);
                    const auto down = parse(vdown);
                    const auto left = parse(vleft);

                    if(!left) { return R::False(Str() << "invalid left(" << vleft << "): " << left.Error()); }
                    if(!right) { return R::False(Str() << "invalid right(" << vright << "): " << right.Error()); }
                    if(!up) { return R::False(Str() << "invalid up(" << vup << "): " << up.Error()); }
                    if(!down) { return R::False(Str() << "invalid down(" << vdown << "): " << down.Error()); }

                    return R::True(Fourway<T>::FromLrud
                    (
                        *left,
                        *right,
                        *up,
                        *down
                    ));
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
