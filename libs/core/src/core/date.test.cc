#include "core/datetime.h"

#include "catch.hpp"

using namespace euphoria::core;


TEST_CASE("datetime", "[datetime]")
{

    SECTION("TestBasic")
    {
        struct_tm_wrapper dt(2001, month::JANUARY, 1, 11, 12, 13);
        REQUIRE(dt.to_debug_string() == "2001-01-01 11:12:13");
    }

    SECTION("TestLocalTime")
    {
        struct_tm_wrapper dt(2001, month::DECEMBER, 1, 11, 12, 13);
        auto r = time_t_wrapper::from_local_time(dt);
        REQUIRE(dt.to_debug_string() == r.to_local_time().to_debug_string());
    }


    SECTION("TestIntParsing")
    {
        struct_tm_wrapper dt(2001, month::DECEMBER, 1, 11, 12, 13);
        auto r = time_t_wrapper::from_local_time(dt);
        REQUIRE(dt.to_debug_string() == int64_to_date_time(date_time_to_int64(r)).to_local_time().to_debug_string());
    }

    //////////////////////////////////////////////////////////////////////////

    SECTION("TestConstructorGmt")
    {
        const auto dt = date_time::FromDate(2010, month::FEBRUARY, 2, time_zone::gmt);
        REQUIRE(2010 == dt.get_year());
        REQUIRE(month::FEBRUARY == dt.get_month());
        REQUIRE(2 == dt.get_day_of_month());
        REQUIRE(12 == dt.get_hour());
    }

    SECTION("TestConstructorLocal")
    {
        const auto dt = date_time::FromDate(2010, month::FEBRUARY, 2, time_zone::local);
        REQUIRE(2010 == dt.get_year());
        REQUIRE(month::FEBRUARY == dt.get_month());
        REQUIRE(2 == dt.get_day_of_month());
        REQUIRE(12 == dt.get_hour());
    }

    SECTION("TestChangeMembers")
    {
        auto dt = date_time::FromDate(2010, month::FEBRUARY, 2, time_zone::local);
        dt.set_year(2012);
        dt.set_month(month::JUNE);
        dt.set_day_of_moth(10);
        dt.set_hour(8);
        dt.set_minutes(33);
        dt.set_seconds(42);
        REQUIRE(2012 == dt.get_year());
        REQUIRE(month::JUNE == dt.get_month());
        REQUIRE(10 == dt.get_day_of_month());
        REQUIRE(8 == dt.get_hour());
        REQUIRE(33 == dt.get_minutes());
        REQUIRE(42 == dt.get_seconds());
    }

}
