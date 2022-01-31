#include "core/datetime.h"

#include <vector>
#include <cstring>

#include "assert/assert.h"
#include "core/cint.h"


#ifdef _WIN32
#define timegm _mkgmtime
#endif


namespace euphoria::core
{

    int
    month_to_int(Month month)
    {
        return static_cast<int>(month);
    }


    Month
    int_to_month(int m)
    {
        ASSERT(m >= month_to_int(Month::january) && m <= month_to_int(Month::december));
        return static_cast<Month>(m);
    }


    //////////////////////////////////////////////////////////////////////////


    TimetWrapper::TimetWrapper(time_t atime)
        : time(atime)
    {
    }


    TimetWrapper
    TimetWrapper::from_local_time(const StructTmWrapper& dt)
    {
        struct tm tt = dt.time;
        return TimetWrapper(mktime(&tt));
    }


    TimetWrapper
    TimetWrapper::from_gmt(const StructTmWrapper& dt)
    {
        // http://stackoverflow.com/questions/283166/easy-way-to-convert-a-struct-tm-expressed-in-utc-to-time-t-type
        struct tm tt = dt.time;
        return TimetWrapper(timegm(&tt));
    }

    TimetWrapper
    TimetWrapper::from_current_time()
    {
        return TimetWrapper(::time(nullptr));
    }


    double
    TimetWrapper::get_difference(const TimetWrapper& start, const TimetWrapper& end)
    {
        return difftime(end.time, start.time);
    }


    StructTmWrapper
    TimetWrapper::to_local_time() const
    {
        return StructTmWrapper(*localtime(&time));
    }


    StructTmWrapper
    TimetWrapper::to_gmt() const
    {
        return StructTmWrapper(*gmtime(&time));
    }


    //////////////////////////////////////////////////////////////////////////


    StructTmWrapper::StructTmWrapper(struct tm t)
        : time(t)
    {
    }


    StructTmWrapper::StructTmWrapper(int year, Month month, int day)
        : time()
    {
        std::memset(&time, 0, sizeof(struct tm));
        set_year(year);
        set_month(month);
        set_day_of_moth(day);
        set_hour(12);
        set_minutes(0);
        set_seconds(0);
    }


    StructTmWrapper::StructTmWrapper(int year, Month month, int day, int hour, int minute, int second, bool dst)
        : time()
    {
        memset(&time, 0, sizeof(struct tm));
        set_year(year);
        set_month(month);
        set_day_of_moth(day);
        set_hour(hour);
        set_minutes(minute);
        set_seconds(second);
        set_dst(dst ? DstInfo::in_effect : DstInfo::not_in_effect);
    }


    void
    StructTmWrapper::set_seconds(int seconds)
    {
        time.tm_sec = seconds;
    }


    void
    StructTmWrapper::set_minutes(int minutes)
    {
        time.tm_min = minutes;
    }


    void
    StructTmWrapper::set_hour(int hour)
    {
        time.tm_hour = hour;
    }


    void
    StructTmWrapper::set_day_of_moth(int day_of_moth)
    {
        time.tm_mday = day_of_moth;
    }


    void
    StructTmWrapper::set_month(Month month)
    {
        time.tm_mon = month_to_int(month);
    }


    void
    StructTmWrapper::set_year(int year)
    {
        time.tm_year = year - 1900;
    }


    void
    StructTmWrapper::set_dst(DstInfo dst)
    {
        switch (dst)
        {
        case DstInfo::in_effect:
            time.tm_isdst = 1;
            return;
        case DstInfo::not_in_effect:
            time.tm_isdst = 0;
            return;
        case DstInfo::info_unavailable:
            time.tm_isdst = -1;
            return;
        }

        DIE("unhandled dst value switch");
    }


    int
    StructTmWrapper::get_seconds() const
    {
        return time.tm_sec;
    }


    int
    StructTmWrapper::get_minutes() const
    {
        return time.tm_min;
    }


    int StructTmWrapper::get_hour() const
    {
        return time.tm_hour;
    }


    int
    StructTmWrapper::get_day_of_moth() const
    {
        return time.tm_mday;
    }


    Month
    StructTmWrapper::get_month() const
    {
        return int_to_month(time.tm_mon);
    }


    int StructTmWrapper::get_year() const
    {
        return time.tm_year + 1900;
    }


    DstInfo
    StructTmWrapper::get_dst() const
    {
        if (time.tm_isdst == 0)
        {
            return DstInfo::not_in_effect;
        }
        else if (time.tm_isdst > 0)
        {
            return DstInfo::in_effect;
        }
        else
        {
            return DstInfo::info_unavailable;
        }
    }


    std::string
    StructTmWrapper::to_debug_string() const
    {
        std::vector<char> ret;
        const size_t base_size = 100;
        for (int i = 1; i < 100; ++i)
        {
            const size_t string_length = base_size * i;
            ret.resize(string_length);

            constexpr const char* const format = "%Y-%m-%d %H:%M:%S";
            const auto characters_written = strftime(&ret[0], string_length, format, &time);

            if (characters_written != 0)
            {
                return &ret[0];
            }
        }

        return "";
    }


    /*
    tm_sec   int  seconds after the minute 0 - 60
    tm_min   int  minutes after the hour   0 - 59
    tm_hour  int  hours since midnight     0 - 23
    tm_mday  int  day of the month         1 - 31
    tm_mon   int  months since January     0 - 11
    tm_year  int  years since              1900+
    tm_isdst int  Daylight Saving Time flag

    The values of the members tm_wday and tm_yday of timeptr are ignored, and the values
    of the other members are interpreted even if out of their valid ranges(see struct tm).

    tm_wday  int   days since Sunday         0 - 6
    tm_yday  int   days since January 1      0 - 365
    */

    U64 date_time_to_int64(const TimetWrapper& dt)
    {
        const auto diff = TimetWrapper::get_difference
        (
            TimetWrapper::from_gmt(StructTmWrapper(1970, Month::january , 1, 0, 0, 0)),
            dt
        );
        return static_cast<U64>(diff);
    }


    TimetWrapper int64_to_date_time(U64 total_seconds)
    {
        const int actual_seconds = c_u64_to_int(total_seconds % 60);
        const U64 total_minutes = (total_seconds - (total_seconds % 60)) / 60;
        const U64 total_hours = (total_minutes - (total_minutes % 60)) / 60;
        const int acutal_minutes = c_u64_to_int(total_minutes % 60);

        return TimetWrapper::from_gmt
        (
            StructTmWrapper
            (
                1970,
                Month::january,
                1,
                c_u64_to_int(total_hours),
                acutal_minutes,
                actual_seconds
            )
        );
    }


    //////////////////////////////////////////////////////////////////////////


    DateTime
    DateTime::create_from_date(int year, core::Month month, int day, TimeZone timezone)
    {
        return DateTime(timezone, StructTmWrapper(year, month, day));
    }


    DateTime
    DateTime::create_from_date_and_time(int year, core::Month month, int day, int hour, int minute, int second, TimeZone timezone)
    {
        return DateTime(timezone, StructTmWrapper(year, month, day, hour, minute, second));
    }


    DateTime
    DateTime::create_from_current_time(TimeZone timezone)
    {
        return DateTime(timezone, TimetWrapper::from_current_time());
    }


    std::string
    DateTime::to_debug_string() const
    {
        return as_struct().to_debug_string();
    }


    void
    DateTime::set_seconds(int seconds)
    {
        auto s = as_struct();
        s.set_seconds(seconds);
        update_time(s);
    }


    void
    DateTime::set_minutes(int minutes)
    {
        auto s = as_struct();
        s.set_minutes(minutes);
        update_time(s);
    }


    void
    DateTime::set_hour(int hour)
    {
        auto s = as_struct();
        s.set_hour(hour);
        update_time(s);
    }


    void
    DateTime::set_day_of_moth(int day_of_moth)
    {
        auto s = as_struct();
        s.set_day_of_moth(day_of_moth);
        update_time(s);
    }


    void
    DateTime::set_month(core::Month month)
    {
        auto s = as_struct();
        s.set_month(month);
        update_time(s);
    }


    void
    DateTime::set_year(int year)
    {
        auto s = as_struct();
        s.set_year(year);
        update_time(s);
    }


    void
    DateTime::set_dst(DstInfo dst)
    {
        auto s = as_struct();
        s.set_dst(dst);
        update_time(s);
    }


    int
    DateTime::get_seconds() const
    {
        return as_struct().get_seconds();
    }


    int
    DateTime::get_minutes() const
    {
        return as_struct().get_minutes();
    }


    int
    DateTime::get_hour() const
    {
        return as_struct().get_hour();
    }


    int
    DateTime::get_day_of_month() const
    {
        return as_struct().get_day_of_moth();
    }


    Month
    DateTime::get_month() const
    {
        return as_struct().get_month();
    }


    int
    DateTime::get_year() const
    {
        return as_struct().get_year();
    }


    DstInfo
    DateTime::get_dst() const
    {
        return as_struct().get_dst();
    }


    TimeZone
    DateTime::get_timezone() const
    {
        return timezone;
    }


    TimetWrapper
    DateTime::get_time() const
    {
        return time;
    }


    TimetWrapper
    to_time_t_wrapper(const StructTmWrapper& s, TimeZone timezone)
    {
        switch (timezone)
        {
        case TimeZone::gmt:
            return TimetWrapper::from_gmt(s);
        case TimeZone::local:
            return TimetWrapper::from_local_time(s);
        }

        DIE("Invalid timezone");
        return TimetWrapper::from_gmt(s);
    }


    DateTime::DateTime(TimeZone atimezone, const StructTmWrapper& wrapped_time)
        : timezone(atimezone)
        , time(to_time_t_wrapper(wrapped_time, timezone))
    {
    }


    DateTime::DateTime(TimeZone atimezone, const TimetWrapper& atime)
        : timezone(atimezone)
        , time(atime)
    {
    }


    StructTmWrapper
    DateTime::as_struct() const
    {
        switch (timezone)
        {
        case TimeZone::gmt:
            return time.to_gmt();
        case TimeZone::local:
            return time.to_local_time();
        }

        DIE("Invalid timezone");
        return time.to_gmt();
    }


    void DateTime::update_time(const StructTmWrapper& s)
    {
        time = to_time_t_wrapper(s, timezone);
    }


}
