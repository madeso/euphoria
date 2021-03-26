#include "core/datetime.h"

#include "catch.hpp"

using namespace euphoria::core;


TEST_CASE("datetime", "[datetime]")
{

    SECTION("TestBasic")
    {
        StructTmWrapper dt(2001, Month::JANUARY, 1, 11, 12, 13);
        REQUIRE(dt.DebugString() == "2001-01-01 11:12:13");
    }

    SECTION("TestLocalTime")
    {
        StructTmWrapper dt(2001, Month::DECEMBER, 1, 11, 12, 13);
        auto r = TimetWrapper::FromLocalTime(dt);
        REQUIRE(dt.DebugString() == r.ToLocalTime().DebugString());
    }


    SECTION("TestIntParsing")
    {
        StructTmWrapper dt(2001, Month::DECEMBER, 1, 11, 12, 13);
        auto r = TimetWrapper::FromLocalTime(dt);
        REQUIRE(dt.DebugString() == Int64ToDateTime(DateTimeToInt64(r)).ToLocalTime().DebugString());
    }

    //////////////////////////////////////////////////////////////////////////

    SECTION("TestConstructorGmt")
    {
        const auto dt = DateTime::FromDate(2010, Month::FEBRUARY, 2, TimeZone::GMT);
        REQUIRE(2010 == dt.year());
        REQUIRE(Month::FEBRUARY == dt.month());
        REQUIRE(2 == dt.day_of_moth());
        REQUIRE(12 == dt.hour());
    }

    SECTION("TestConstructorLocal")
    {
        const auto dt = DateTime::FromDate(2010, Month::FEBRUARY, 2, TimeZone::LOCAL);
        REQUIRE(2010 == dt.year());
        REQUIRE(Month::FEBRUARY == dt.month());
        REQUIRE(2 == dt.day_of_moth());
        REQUIRE(12 == dt.hour());
    }

    SECTION("TestChangeMembers")
    {
        auto dt = DateTime::FromDate(2010, Month::FEBRUARY, 2, TimeZone::LOCAL);
        dt.set_year(2012);
        dt.set_month(Month::JUNE);
        dt.set_day_of_moth(10);
        dt.set_hour(8);
        dt.set_minutes(33);
        dt.set_seconds(42);
        REQUIRE(2012 == dt.year());
        REQUIRE(Month::JUNE == dt.month());
        REQUIRE(10 == dt.day_of_moth());
        REQUIRE(8 == dt.hour());
        REQUIRE(33 == dt.minutes());
        REQUIRE(42 == dt.seconds());
    }

}
