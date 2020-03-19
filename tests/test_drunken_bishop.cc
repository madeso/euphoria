#include "core/drunken_bishop.h"

#include <bitset>

#include "catch.hpp"
#include "tests/utils.h"
#include "tests/falsestring.h"
#include "tests/vectorequals.h"
#include "tests/stringeq.h"


using namespace euphoria::core;
using namespace euphoria::tests;


std::string
BinIntToString(u8 t)
{
    std::ostringstream ss;
    std::bitset<2> x(t);
    ss << x;
    return ss.str();
};


template<typename T>
std::string
HexIntToString(const T& t)
{
    std::ostringstream ss;
    ss << std::hex << static_cast<int>(t);
    return ss.str();
};


template<typename T, typename F>
FalseString
CompareInts(const T& lhs, const T& rhs, F to_string)
{
    if(lhs == rhs)
    {
        return FalseString::True();
    }
    else
    {
        std::ostringstream ss;
        ss << to_string(lhs) << " != " << to_string(rhs);
        return FalseString::False(ss.str());
    }
};


template<typename T>
FalseString
HexIntEquals
(
    const std::vector<T>& lhs,
    const std::vector<T>& rhs
)
{
    auto to_string = [](const T& t) { return HexIntToString(t); };
    return VectorEquals<T>
    (
        lhs,
        rhs,
        to_string,
        [&](const T& lhs, const T& rhs){ return CompareInts(lhs, rhs, to_string); }
    );
}


template<typename T>
FalseString
BinIntEquals
(
    const std::vector<T>& lhs,
    const std::vector<T>& rhs
)
{
    auto to_string = [](const T& t) { return BinIntToString(t); };
    return VectorEquals<T>
    (
        lhs,
        rhs,
        to_string,
        [&](const T& lhs, const T& rhs){ return CompareInts(lhs, rhs, to_string); }
    );
}


TEST_CASE("drunken bishop 0", "[drunken-bishop]")
{
    CHECK(HexIntEquals(ToBytes(static_cast<u32>(0)), {0x00, 0x00, 0x00, 0x00}));
    CHECK(HexIntEquals(ToBytes(static_cast<u32>(1)), {0x00, 0x00, 0x00, 0x01}));
    CHECK(HexIntEquals(ToBytes(static_cast<u32>(42)), {0x00, 0x00, 0x00, 0x2A}));
    CHECK(HexIntEquals(ToBytes(static_cast<u32>(1337)), {0x00, 0x00, 0x05, 0x39}));
    CHECK(HexIntEquals(ToBytes(std::numeric_limits<u32>::max()), {0xFF, 0xFF, 0xFF, 0xFF}));

    CHECK(BinIntEquals(ToCodes(0x00, true), {0b00, 0b00, 0b00, 0b00}));
    CHECK(BinIntEquals(ToCodes(0x01, true), {0b00, 0b00, 0b00, 0b01}));
    CHECK(BinIntEquals(ToCodes(0xFF, true), {0b11, 0b11, 0b11, 0b11}));
    CHECK(BinIntEquals(ToCodes(0x29, true), {0b00, 0b10, 0b10, 0b01}));
    CHECK(BinIntEquals(ToCodes(0x4D, true), {0b01, 0b00, 0b11, 0b01}));
}

TEST_CASE("drunken bishop strings", "[drunken-bishop]")
{
    auto test = []
    (
        const std::vector<u8>& bytes,
        const std::vector<std::string> correct_result
    )
    {
        const int width = 17;
        const int height = 9;

        const auto codes = ToCodes(bytes, false);
        const auto table = DrunkenBishop(codes, width, height);
        const auto res = Collapse(table, GetSshCharacters());

        return StringEq(res, correct_result);
    };

    // tests comes from https://github.com/calmh/randomart/blob/master/randomart_test.go
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
}

