#ifndef EUPHORIA_CORE_FOURWAY_H
#define EUPHORIA_CORE_FOURWAY_H

#include <vector>
#include <optional>
#include <string>
#include <sstream>

#include "core/custom_parser.h"
#include "core/stringutils.h"
#include "core/default_parse.h"


namespace euphoria::core
{
    /**
      Generic version of a CSS like padding type.
    */
    template<typename T>
    struct fourway
    {
        using self = fourway<T>;

        T left;
        T right;
        T up;
        T down;

        explicit fourway(const T& t)
            : left(t)
            , right(t)
            , up(t)
            , down(t)
        {
        }

        /** Create a new fourway from left-righ and up-down.
          */
        [[nodiscard]]
        static
        self
        from_lrud(const T& lr, const T& ud)
        {
            return {lr, lr, ud, ud};
        }

        /** Create a new fourway from left right up and down.
          */
        [[nodiscard]]
        static
        self
        from_lrud(const T& l, const T& r, const T& u, const T& d)
        {
            return {l, r, u, d};
        }

    private:
        fourway
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
    operator==(const fourway<T>& lhs, const fourway<T>& rhs)
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
    operator!=(const fourway<T>& lhs, const fourway<T>& rhs)
    {
        return !(lhs == rhs);
    }


    template<typename T>
    std::ostream&
    operator<<(std::ostream& s, const fourway<T>& fw)
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
    struct custom_argparser
    <fourway<T>>
    {
        enum { value = 1 };

        static constexpr char SPLIT = '/';

        static
        std::string
        to_string(const fourway<T>& fw)
        {
            std::ostringstream ss;
            ss
                << argparse::default_value_to_string(fw.up) << SPLIT
                << argparse::default_value_to_string(fw.right) << SPLIT
                << argparse::default_value_to_string(fw.down) << SPLIT
                << argparse::default_value_to_string(fw.left)
                ;
            return ss.str();
        }

        static
        result<fourway<T>>
        parse(const std::string& value)
        {
            using R = result<fourway<T>>;
            auto parse = [](const std::string& v)
            {
                return argparse::default_parse_function<T>(v);
            };
            const auto values = split(value, SPLIT);
            switch(values.size())
            {
                case 1:
                {
                    const auto val = parse(values[0]);

                    if(!val) { return R::create_error(val.get_error()); }

                    return R::create_value(fourway<T>{*val});
                }
                case 2:
                {
                    const auto vvert = values[0];
                    const auto vhor = values[1];

                    const auto vert = parse(vvert);
                    const auto hor = parse(vhor);

                    if(!hor) { return R::create_error(string_builder() << "invalid hor(" << vhor << "): " << hor.get_error()); }
                    if(!vert) { return R::create_error(string_builder() << "invalid vert(" << vvert << "): " << vert.get_error()); }

                    return R::create_value(fourway<T>::from_lrud(*hor, *vert));
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

                    if(!left) { return R::create_error(string_builder() << "invalid left(" << vleft << "): " << left.get_error()); }
                    if(!right) { return R::create_error(string_builder() << "invalid right(" << vright << "): " << right.get_error()); }
                    if(!up) { return R::create_error(string_builder() << "invalid up(" << vup << "): " << up.get_error()); }
                    if(!down) { return R::create_error(string_builder() << "invalid down(" << vdown << "): " << down.get_error()); }

                    return R::create_value(fourway<T>::from_lrud
                    (
                        *left,
                        *right,
                        *up,
                        *down
                    ));
                }
                default:
                {
                    return R::create_error("wrong number of splits in fourway");
                }
            }
        }
    };

}

#endif // EUPHORIA_CORE_FOURWAY_H
