#include "core/drunken_bishop.h"

#include <bitset>

#include "catch.hpp"
#include "tests/utils.h"
#include "tests/falsestring.h"
#include "tests/vectorequals.h"


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
    CHECK(HexIntEquals(ToBytes(0), {0x00, 0x00, 0x00, 0x00}));
    CHECK(HexIntEquals(ToBytes(1), {0x00, 0x00, 0x00, 0x01}));
    CHECK(HexIntEquals(ToBytes(42), {0x00, 0x00, 0x00, 0x2A}));
    CHECK(HexIntEquals(ToBytes(1337), {0x00, 0x00, 0x05, 0x39}));
    CHECK(HexIntEquals(ToBytes(std::numeric_limits<u32>::max()), {0xFF, 0xFF, 0xFF, 0xFF}));

    CHECK(BinIntEquals(ToCodes(0x00), {0b00, 0b00, 0b00, 0b00}));
    CHECK(BinIntEquals(ToCodes(0x01), {0b00, 0b00, 0b00, 0b01}));
    CHECK(BinIntEquals(ToCodes(0xFF), {0b11, 0b11, 0b11, 0b11}));
    CHECK(BinIntEquals(ToCodes(0x29), {0b00, 0b10, 0b10, 0b01}));
    CHECK(BinIntEquals(ToCodes(0x4D), {0b01, 0b00, 0b11, 0b01}));
}

