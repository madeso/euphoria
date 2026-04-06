#pragma once

namespace eu
{
    /** \addtogroup math
     *  @{
     */
    
    /// a size
    struct Size
    {
        int width;
        int height;
    };
    constexpr bool operator==(const Size& lhs, const Size& rhs)
    {
        return lhs.width == rhs.width
            && lhs.height == rhs.height;
    }

    /// converts a 2d unit vector to a string, prefer fmt
    std::string string_from(const Size& v);

    /** @}*/
}

ADD_DEFAULT_FORMATTER(eu::Size, std::string, eu::string_from);
