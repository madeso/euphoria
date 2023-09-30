#pragma once


#include <optional>



#include "base/string_io.h"
#include "base/stringutils.h"
#include "core/default_parse.h"


namespace eu::core
{
    /**
      Generic version of a CSS like padding type.
    */
    template<typename T>
    struct Lrud
    {
        using Self = Lrud<T>;

        T left;
        T right;
        T up;
        T down;

        explicit Lrud(const T& t)
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
        Self
        from_lrud(const T& lr, const T& ud)
        {
            return {lr, lr, ud, ud};
        }

        /** Create a new fourway from left right up and down.
          */
        [[nodiscard]]
        static
        Self
        from_lrud(const T& l, const T& r, const T& u, const T& d)
        {
            return {l, r, u, d};
        }

    private:
        Lrud
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
    operator==(const Lrud<T>& lhs, const Lrud<T>& rhs)
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
    operator!=(const Lrud<T>& lhs, const Lrud<T>& rhs)
    {
        return !(lhs == rhs);
    }
}

namespace eu
{

    /** Parses a Lrud according to the CSS spec.
      Either all, ver/hor or up/right/down/left
    */
    template<typename T>
    struct StringParser<core::Lrud<T>>
    {
        enum { value = 1 };

        using Lrud = core::Lrud<T>;

        static constexpr char separator = '/';

        static
        std::string
        to_string(const Lrud& fw)
        {
            return fmt::format
            (
                "{1}{0}{2}{0}{3}{0}{4}",
                separator,
                core::argparse::from_default_value_to_string(fw.up),
                core::argparse::from_default_value_to_string(fw.right),
                core::argparse::from_default_value_to_string(fw.down),
                core::argparse::from_default_value_to_string(fw.left)
            );
        }

        static
        Result<Lrud>
        parse(const std::string& value)
        {
            using R = Result<Lrud>;
            auto parse = [](const std::string& v)
            {
                return core::argparse::default_parse_function<T>(v);
            };
            const auto values = split(value, separator);
            switch(values.size())
            {
                case 1:
                {
                    const auto val = parse(values[0]);

                    if(!val) { return R::create_error(val.get_error()); }

                    return R::create_value(Lrud{*val});
                }
                case 2:
                {
                    const auto vvert = values[0];
                    const auto vhor = values[1];

                    const auto vert = parse(vvert);
                    const auto hor = parse(vhor);

                    if(!hor) { return R::create_error(fmt::format("invalid hor({}): {}", vhor, hor.get_error())); }
                    if(!vert) { return R::create_error(fmt::format("invalid vert({}): {}", vvert, vert.get_error())); }

                    return R::create_value(Lrud::from_lrud(*hor, *vert));
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

                    if(!left) { return R::create_error(fmt::format("invalid left({}): {}", vleft, left.get_error())); }
                    if(!right) { return R::create_error(fmt::format("invalid right({}): {}", vright, right.get_error())); }
                    if(!up) { return R::create_error(fmt::format("invalid up({}): {}", vup, up.get_error())); }
                    if(!down) { return R::create_error(fmt::format("invalid down({}): {}", vdown, down.get_error())); }

                    return R::create_value(Lrud::from_lrud
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


template<typename T>
struct fmt::formatter<eu::core::Lrud<T>> : formatter<std::string>
{
    template <typename FormatContext>
    auto format(const eu::core::Lrud<T>& fw, FormatContext& ctx) const
    {
        return formatter<string_view>::format
        (
            fmt::format("({} {} {} {})", fw.up, fw.right, fw.down, fw.left),
            ctx
        );
    }
};
