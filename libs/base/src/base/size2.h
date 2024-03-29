#pragma once

#include "base/vec2.h"
#include "base/numeric.h"
#include "base/stringutils.h"
#include "base/string_io.h"


namespace eu
{
    struct size2f
    {
        float width;
        float height;

        size2f();

        [[nodiscard]] static size2f create_from_width_height(float w, float h);
        [[nodiscard]] static size2f create_from_height_width(float h, float w);
        [[nodiscard]] static size2f create_from_square(float s);

        [[nodiscard]] vec2f calc_center_offset_for(const size2f& o) const;

        void operator+=(const size2f& rhs);
        void operator-=(const size2f& rhs);
        void operator*=(float rhs);
        void operator/=(float rhs);

    private:
        size2f(float w, float h);
    };


    struct size2i
    {
        int width;
        int height;

        size2i();

        [[nodiscard]] static size2i create_from_width_height(int w, int h);
        [[nodiscard]] static size2i create_from_height_width(int h, int w);
        [[nodiscard]] static size2i create_from_square(int s);

        [[nodiscard]] vec2i calc_center_offset_for(const size2i& o) const;

        void operator+=(const size2i& rhs);
        void operator-=(const size2i& rhs);
        void operator*=(int rhs);

    private:
        size2i(int w, int h);
    };


    template<>
    struct StringParser<size2f>
    {
        enum { value = 1 };

        static std::string to_string(const size2f& s);
        static Result<size2f> parse(const std::string& value);
    };


    template<>
    struct StringParser<size2i>
    {
        enum { value = 1 };

        static std::string to_string(const size2i& s);
        static Result<size2i> parse(const std::string& value);
    };

    size2f min(const size2f& lhs, const size2f& rhs);
    size2i min(const size2i& lhs, const size2i& rhs);
    size2f max(const size2f& lhs, const size2f& rhs);
    size2i max(const size2i& lhs, const size2i& rhs);

    size2f operator+(const size2f& lhs, const size2f& rhs);
    size2i operator+(const size2i& lhs, const size2i& rhs);
    size2f operator-(const size2f& lhs, const size2f& rhs);
    size2i operator-(const size2i& lhs, const size2i& rhs);

    size2f operator*(const size2f& lhs, float rhs);
    size2i operator*(const size2i& lhs, int rhs);
    size2f operator*(float lhs, const size2f& rhs);
    size2i operator*(int lhs, const size2i& rhs);

    size2f operator/(const size2f& lhs, float rhs);

    std::string to_string(const size2f& r);
    std::string to_string(const size2i& r);
}

ADD_DEFAULT_FORMATTER(eu::size2f, std::string, eu::to_string);
ADD_DEFAULT_FORMATTER(eu::size2i, std::string, eu::to_string);
