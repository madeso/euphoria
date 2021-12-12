#include "core/datetime.h"

#include "catch.hpp"

using namespace euphoria::core;


TEST_CASE("datetime", "[datetime]")
{

    SECTION("TestBasic")
    {
        StructTmWrapper dt(2001, Month::january, 1, 11, 12, 13);
        REQUIRE(dt.to_debug_string() == "2001-01-01 11:12:13");
    }

    SECTION("TestLocalTime")
    {
        StructTmWrapper dt(2001, Month::december, 1, 11, 12, 13);
        auto r = TimetWrapper::from_local_time(dt);
        REQUIRE(dt.to_debug_string() == r.to_local_time().to_debug_string());
    }


    SECTION("TestIntParsing")
    {
        StructTmWrapper dt(2001, Month::december, 1, 11, 12, 13);
        auto r = TimetWrapper::from_local_time(dt);
        REQUIRE(dt.to_debug_string() == int64_to_date_time(date_time_to_int64(r)).to_local_time().to_debug_string());
    }

    //////////////////////////////////////////////////////////////////////////

    SECTION("TestConstructorGmt")
    {
        const auto dt = DateTime::create_from_date(2010, Month::february, 2, time_zone::gmt);
        REQUIRE(2010 == dt.get_year());
        REQUIRE(Month::february == dt.get_month());
        REQUIRE(2 == dt.get_day_of_month());
        REQUIRE(12 == dt.get_hour());
    }

    SECTION("TestConstructorLocal")
    {
        const auto dt = DateTime::create_from_date(2010, Month::february, 2, time_zone::local);
        REQUIRE(2010 == dt.get_year());
        REQUIRE(Month::february == dt.get_month());
        REQUIRE(2 == dt.get_day_of_month());
        REQUIRE(12 == dt.get_hour());
    }

    SECTION("TestChangeMembers")
    {
        auto dt = DateTime::create_from_date(2010, Month::february, 2, time_zone::local);
        dt.set_year(2012);
        dt.set_month(Month::june);
        dt.set_day_of_moth(10);
        dt.set_hour(8);
        dt.set_minutes(33);
        dt.set_seconds(42);
        REQUIRE(2012 == dt.get_year());
        REQUIRE(Month::june == dt.get_month());
        REQUIRE(10 == dt.get_day_of_month());
        REQUIRE(8 == dt.get_hour());
        REQUIRE(33 == dt.get_minutes());
        REQUIRE(42 == dt.get_seconds());
    }

}
