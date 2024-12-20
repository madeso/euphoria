#include "core/drunken_bishop.h"

#include <bitset>

#include "catch2/catch_all.hpp"
#include "tests/utils.h"
#include "tests/falsestring.h"
#include "tests/vectorequals.h"
#include "tests/stringeq.h"

using namespace eu;
using namespace eu::core;
using namespace eu::tests;


template<typename T, typename F>
FalseString
compare_ints(const T& lhs, const T& rhs, F to_string)
{
    if(lhs == rhs)
    {
        return FalseString::create_true();
    }
    else
    {
        return FalseString::create_false
        (
            fmt::format("{} != {}", to_string(lhs), to_string(rhs))
        );
    }
}


template<typename T>
FalseString
int_equals_hex
(
    const std::vector<T>& lhs,
    const std::vector<T>& rhs
)
{
    auto to_string = [](const T& t)
    {
        return fmt::format("{:x}", t);
    };
    return is_vector_equal<T>
    (
        lhs,
        rhs,
        to_string,
        [&](const T& alhs, const T& arhs)
        { return compare_ints(alhs, arhs, to_string); }
    );
}


template<typename T>
FalseString
int_equals_bin
(
    const std::vector<T>& lhs,
    const std::vector<T>& rhs
)
{
    const auto to_string = [](const T& t)
    {
        const auto u8 = static_cast<U8>(t);
        return fmt::format("{:0>2x}", u8);
    };
    return is_vector_equal<T>
    (
        lhs,
        rhs,
        to_string,
        [&](const T& alhs, const T& arhs)
        { return compare_ints(alhs, arhs, to_string); }
    );
}


TEST_CASE("drunken bishop 0", "[drunken-bishop]")
{
    CHECK(int_equals_hex(to_bytes(static_cast<U32>(0)), {0x00, 0x00, 0x00, 0x00}));
    CHECK(int_equals_hex(to_bytes(static_cast<U32>(1)), {0x00, 0x00, 0x00, 0x01}));
    CHECK(int_equals_hex(to_bytes(static_cast<U32>(42)), {0x00, 0x00, 0x00, 0x2A}));
    CHECK(int_equals_hex(to_bytes(static_cast<U32>(1337)), {0x00, 0x00, 0x05, 0x39}));
    CHECK(int_equals_hex(to_bytes(std::numeric_limits<U32>::max()), {0xFF, 0xFF, 0xFF, 0xFF}));

    CHECK(int_equals_bin(to_codes(0x00, true), {0b00, 0b00, 0b00, 0b00}));
    CHECK(int_equals_bin(to_codes(0x01, true), {0b00, 0b00, 0b00, 0b01}));
    CHECK(int_equals_bin(to_codes(0xFF, true), {0b11, 0b11, 0b11, 0b11}));
    CHECK(int_equals_bin(to_codes(0x29, true), {0b00, 0b10, 0b10, 0b01}));
    CHECK(int_equals_bin(to_codes(0x4D, true), {0b01, 0b00, 0b11, 0b01}));
}


// examples comes from https://github.com/calmh/randomart/blob/master/randomart_test.go
// I only removed the S(tart) and E(nd) characters that I think should belong to a debug rendering of drunken bishop
TEST_CASE("drunken bishop strings", "[drunken-bishop]")
{
    auto test = []
    (
        const std::vector<U8>& bytes,
        const std::vector<std::string> correct_result
    )
    {
        const int width = 17;
        const int height = 9;

        const auto codes = to_codes(bytes, false);
        const auto table = get_drunken_bishop_result(codes, width, height);
        const auto result = render_table(table, get_ssh_characters());

        return is_string_equal(result, correct_result);
    };

    CHECK(test({
        0x9b, 0x4c, 0x7b, 0xce,
        0x7a, 0xbd, 0x0a, 0x13,
        0x61, 0xfb, 0x17, 0xc2,
        0x06, 0x12, 0x0c, 0xed,
    },{
        "    .+.          ",
        "      o.         ",
        "     .. +        ",
        "      .o =       ",
        "        + + .    ",
        "       o B . .   ",
        "        B o..    ",
        "         *...    ",
        "        .o+...   ",
    }));

    CHECK(test({
        0x30, 0xaa, 0x88, 0x72,
        0x7d, 0xc8, 0x30, 0xd0,
        0x2b, 0x99, 0xc7, 0x8f,
        0xd1, 0x86, 0x59, 0xfc,
    },{
        "                 ",
        " . .             ",
        ". . o o          ",
        " = * o o         ",
        "+ X + .          ",
        ".+ @ .           ",
        "+ + = .          ",
        "..   .           ",
        "                 ",
    }));

    CHECK(test({
        0x30, 0xaa, 0x88, 0x72,
        0x7d, 0xc8, 0x30, 0xd0,
        0x2b, 0x99, 0xc7, 0x8f,
        0xd1, 0x86, 0x59, 0xfc,
    },{
        "                 ",
        " . .             ",
        ". . o o          ",
        " = * o o         ",
        "+ X + .          ",
        ".+ @ .           ",
        "+ + = .          ",
        "..   .           ",
        "                 ",
    }));

    CHECK(test({
        0xa1, 0x37, 0xe4, 0xd4,
        0xdf, 0xd2, 0xa0, 0x96,
        0x1b, 0xc6, 0xf5, 0x9f,
        0xf5, 0x34, 0x05, 0x80,
        0xa7, 0xbd, 0x8f, 0x58,
        0x3d, 0x55, 0x92, 0xff,
        0x76, 0x1e, 0x4f, 0x6e,
        0x30, 0xbb, 0x9f, 0x75,
    },{
        "           ..... ",
        "         .. . o..",
        "        + .+o  oo",
        "       = o.=.=  +",
        "      . + B oo+o+",
        "       . + oo.=+X",
        "          .o o @O",
        "          . . o O",
        "              .=.",
    }));
}

