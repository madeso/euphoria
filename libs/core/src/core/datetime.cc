#include "core/datetime.h"

#include "core/assert.h"

#include <vector>
#include <cstring>

#ifdef _WIN32
#define timegm _mkgmtime
#endif

namespace euphoria::core
{

    int
    month_to_int(month month)
    {
        return static_cast<int>(month);
    }


    month
    int_to_month(int m)
    {
        ASSERT(m >= month_to_int(month::january) && m <= month_to_int(month::december));
        return static_cast<month>(m);
    }


    //////////////////////////////////////////////////////////////////////////


    time_t_wrapper::time_t_wrapper(time_t time)
        : time(time)
    {
    }


    time_t_wrapper
    time_t_wrapper::from_local_time(const struct_tm_wrapper& dt)
    {
        struct tm tt = dt.time;
        return time_t_wrapper(mktime(&tt));
    }


    time_t_wrapper
    time_t_wrapper::from_gmt(const struct_tm_wrapper& dt)
    {
        // http://stackoverflow.com/questions/283166/easy-way-to-convert-a-struct-tm-expressed-in-utc-to-time-t-type
        struct tm tt = dt.time;
        return time_t_wrapper(timegm(&tt));
    }

    time_t_wrapper
    time_t_wrapper::from_current_time()
    {
        return time_t_wrapper(::time(nullptr));
    }


    double
    time_t_wrapper::get_difference(const time_t_wrapper& start, const time_t_wrapper& end)
    {
        return difftime(end.time, start.time);
    }


    struct_tm_wrapper
    time_t_wrapper::to_local_time() const
    {
        return struct_tm_wrapper(*localtime(&time));
    }


    struct_tm_wrapper
    time_t_wrapper::to_gmt() const
    {
        return struct_tm_wrapper(*gmtime(&time));
    }


    //////////////////////////////////////////////////////////////////////////


    struct_tm_wrapper::struct_tm_wrapper(struct tm t)
        : time(t)
    {
    }


    struct_tm_wrapper::struct_tm_wrapper(int year, month month, int day)
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


    struct_tm_wrapper::struct_tm_wrapper(int year, month month, int day, int hour, int minute, int second, bool dst)
        : time()
    {
        memset(&time, 0, sizeof(struct tm));
        set_year(year);
        set_month(month);
        set_day_of_moth(day);
        set_hour(hour);
        set_minutes(minute);
        set_seconds(second);
        set_dst(dst ? dst_info::in_effect : dst_info::not_in_effect);
    }


    void
    struct_tm_wrapper::set_seconds(int seconds)
    {
        time.tm_sec = seconds;
    }


    void
    struct_tm_wrapper::set_minutes(int minutes)
    {
        time.tm_min = minutes;
    }


    void
    struct_tm_wrapper::set_hour(int hour)
    {
        time.tm_hour = hour;
    }


    void
    struct_tm_wrapper::set_day_of_moth(int day_of_moth)
    {
        time.tm_mday = day_of_moth;
    }


    void
    struct_tm_wrapper::set_month(month month)
    {
        time.tm_mon = month_to_int(month);
    }


    void
    struct_tm_wrapper::set_year(int year)
    {
        time.tm_year = year - 1900;
    }


    void
    struct_tm_wrapper::set_dst(dst_info dst)
    {
        switch (dst)
        {
        case dst_info::in_effect:
            time.tm_isdst = 1;
            return;
        case dst_info::not_in_effect:
            time.tm_isdst = 0;
            return;
        case dst_info::info_unavailable:
            time.tm_isdst = -1;
            return;
        }

        DIE("unhandled dst value switch");
    }


    int
    struct_tm_wrapper::get_seconds() const
    {
        return time.tm_sec;
    }


    int
    struct_tm_wrapper::get_minutes() const
    {
        return time.tm_min;
    }


    int struct_tm_wrapper::get_hour() const
    {
        return time.tm_hour;
    }


    int
    struct_tm_wrapper::get_day_of_moth() const
    {
        return time.tm_mday;
    }


    month
    struct_tm_wrapper::get_month() const
    {
        return int_to_month(time.tm_mon);
    }


    int struct_tm_wrapper::get_year() const
    {
        return time.tm_year + 1900;
    }


    dst_info
    struct_tm_wrapper::get_dst() const
    {
        if (time.tm_isdst == 0)
        {
            return dst_info::not_in_effect;
        }
        else if (time.tm_isdst > 0)
        {
            return dst_info::in_effect;
        }
        else
        {
            return dst_info::info_unavailable;
        }
    }


    std::string
    struct_tm_wrapper::to_string(const std::string& format) const
    {
        std::vector<char> ret;
        const size_t base_size = 100;
        for (int i = 1; i < 100; ++i)
        {
            const size_t string_length = base_size * i;
            ret.resize(string_length);

            const auto characters_written = strftime(&ret[0], string_length, format.c_str(), &time);

            if (characters_written != 0)
            {
                return &ret[0];
            }
        }

        return "";
    }

    std::string
    struct_tm_wrapper::to_debug_string() const
    {
        return to_string("%Y-%m-%d %H:%M:%S");
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

    uint64_t date_time_to_int64(const time_t_wrapper& dt)
    {
        const auto diff = time_t_wrapper::get_difference
        (
            time_t_wrapper::from_gmt(struct_tm_wrapper(1970, month::january , 1, 0, 0, 0)),
            dt
        );
        return static_cast<uint64_t>(diff);
    }


    time_t_wrapper int64_to_date_time(uint64_t i)
    {
        const uint64_t minutes = (i - (i % 60)) / 60;
        const int actual_seconds = static_cast<int>(i % 60);
        const uint64_t hours = (minutes - (minutes % 60)) / 60;
        const int acutal_minutes = static_cast<int>(minutes % 60);

        return time_t_wrapper::from_gmt
        (
            struct_tm_wrapper(1970, month::january, 1, hours, acutal_minutes, actual_seconds)
        );
    }


    //////////////////////////////////////////////////////////////////////////


    date_time
    date_time::create_from_date(int year, core::month month, int day, time_zone timezone)
    {
        return date_time(timezone, struct_tm_wrapper(year, month, day));
    }


    date_time
    date_time::create_from_date_and_time(int year, core::month month, int day, int hour, int minute, int second, time_zone timezone)
    {
        return date_time(timezone, struct_tm_wrapper(year, month, day, hour, minute, second));
    }


    date_time
    date_time::create_from_current_time(time_zone timezone)
    {
        return date_time(timezone, time_t_wrapper::from_current_time());
    }


    std::string
    date_time::to_string(const std::string& format) const
    {
        return as_struct().to_string(format);
    }


    std::string
    date_time::to_debug_string() const
    {
        return as_struct().to_debug_string();
    }


    void
    date_time::set_seconds(int seconds)
    {
        auto s = as_struct();
        s.set_seconds(seconds);
        update_time(s);
    }


    void
    date_time::set_minutes(int minutes)
    {
        auto s = as_struct();
        s.set_minutes(minutes);
        update_time(s);
    }


    void
    date_time::set_hour(int hour)
    {
        auto s = as_struct();
        s.set_hour(hour);
        update_time(s);
    }


    void
    date_time::set_day_of_moth(int day_of_moth)
    {
        auto s = as_struct();
        s.set_day_of_moth(day_of_moth);
        update_time(s);
    }


    void
    date_time::set_month(core::month month)
    {
        auto s = as_struct();
        s.set_month(month);
        update_time(s);
    }


    void
    date_time::set_year(int year)
    {
        auto s = as_struct();
        s.set_year(year);
        update_time(s);
    }


    void
    date_time::set_dst(dst_info dst)
    {
        auto s = as_struct();
        s.set_dst(dst);
        update_time(s);
    }


    int
    date_time::get_seconds() const
    {
        return as_struct().get_seconds();
    }


    int
    date_time::get_minutes() const
    {
        return as_struct().get_minutes();
    }


    int
    date_time::get_hour() const
    {
        return as_struct().get_hour();
    }


    int
    date_time::get_day_of_month() const
    {
        return as_struct().get_day_of_moth();
    }


    month
    date_time::get_month() const
    {
        return as_struct().get_month();
    }


    int
    date_time::get_year() const
    {
        return as_struct().get_year();
    }


    dst_info
    date_time::get_dst() const
    {
        return as_struct().get_dst();
    }


    time_zone
    date_time::get_timezone() const
    {
        return timezone;
    }


    time_t_wrapper
    date_time::get_time() const
    {
        return time;
    }


    time_t_wrapper
    to_time_t_wrapper(const struct_tm_wrapper& s, time_zone timezone)
    {
        switch (timezone)
        {
        case time_zone::gmt:
            return time_t_wrapper::from_gmt(s);
        case time_zone::local:
            return time_t_wrapper::from_local_time(s);
        }

        DIE("Invalid timezone");
        return time_t_wrapper::from_gmt(s);
    }


    date_time::date_time(time_zone timezone, const struct_tm_wrapper& time)
        : timezone(timezone)
        , time(to_time_t_wrapper(time, timezone))
    {
    }


    date_time::date_time(time_zone timezone, const time_t_wrapper& time)
        : timezone(timezone)
        , time(time)
    {
    }


    struct_tm_wrapper
    date_time::as_struct() const
    {
        switch (timezone)
        {
        case time_zone::gmt:
            return time.to_gmt();
        case time_zone::local:
            return time.to_local_time();
        }

        DIE("Invalid timezone");
        return time.to_gmt();
    }


    void date_time::update_time(const struct_tm_wrapper& s)
    {
        time = to_time_t_wrapper(s, timezone);
    }


}
