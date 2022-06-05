#pragma once

#include <ctime>

#include <cstdint>

#include "core/ints.h"


namespace euphoria::core
{
    struct TimetWrapper;
    struct StructTmWrapper;

    enum class Month {
        january, february, march, april, may, june, july, august, september, october, november, december
    };

    struct TimetWrapper
    {
        friend struct StructTmWrapper;
        explicit TimetWrapper(time_t time);

        static TimetWrapper from_local_time(const StructTmWrapper& dt);
        static TimetWrapper from_gmt(const StructTmWrapper& dt);
        static TimetWrapper from_current_time();

        static double get_difference(const TimetWrapper& start, const TimetWrapper& end);

        [[nodiscard]] StructTmWrapper to_local_time() const;
        [[nodiscard]] StructTmWrapper to_gmt() const;

        time_t time;
    };

    enum class DstInfo {
        in_effect, not_in_effect, info_unavailable
    };

    struct StructTmWrapper
    {
    protected:
        friend struct TimetWrapper;
        explicit StructTmWrapper(struct tm time);

    public:
        StructTmWrapper(int year, core::Month month, int day);
        StructTmWrapper(int year, core::Month month, int day, int hour, int minute, int second, bool dst=false);

        void set_seconds(int seconds);
        void set_minutes(int minutes);
        void set_hour(int hour);
        void set_day_of_moth(int day_of_moth);
        void set_month(core::Month month);
        void set_year(int year);
        void set_dst(DstInfo dst);

        [[nodiscard]] int get_seconds() const;
        [[nodiscard]] int get_minutes() const;
        [[nodiscard]] int get_hour() const;
        [[nodiscard]] int get_day_of_moth() const;
        [[nodiscard]] Month get_month() const;
        [[nodiscard]] int get_year() const;
        [[nodiscard]] DstInfo get_dst() const;

        [[nodiscard]] std::string to_debug_string() const;

        struct tm time;
    };

    // unix date time format, 64 bit
    // todo(Gustav): test 2038 problem
    uint64_t date_time_to_int64(const TimetWrapper& dt);
    TimetWrapper int64_to_date_time(uint64_t i);

    enum class TimeZone
    {
        gmt, local
    };

    // public interface
    // util class to make stuff nice to use
    struct DateTime
    {
    public:
        DateTime() = delete;

        static DateTime create_from_date(int year, core::Month month, int day, TimeZone timezone = TimeZone::local);
        static DateTime create_from_date_and_time(int year, core::Month month, int day, int hour, int minute, int second, TimeZone timezone = TimeZone::local);
        static DateTime create_from_current_time(TimeZone timezone = TimeZone::local);

        [[nodiscard]] std::string to_debug_string() const;

        void set_seconds(int seconds);
        void set_minutes(int minutes);
        void set_hour(int hour);
        void set_day_of_moth(int day_of_moth);
        void set_month(core::Month month);
        void set_year(int year);
        void set_dst(DstInfo dst);

        [[nodiscard]] int get_seconds() const;
        [[nodiscard]] int get_minutes() const;
        [[nodiscard]] int get_hour() const;
        [[nodiscard]] int get_day_of_month() const;
        [[nodiscard]] Month get_month() const;
        [[nodiscard]] int get_year() const;
        [[nodiscard]] DstInfo get_dst() const;

        [[nodiscard]] TimeZone get_timezone() const;
        [[nodiscard]] TimetWrapper get_time() const;

    private:
        DateTime(TimeZone timezone, const StructTmWrapper& time);
        DateTime(TimeZone timezone, const TimetWrapper& time);

        [[nodiscard]] StructTmWrapper as_struct() const;
        void update_time(const StructTmWrapper& s);

        TimeZone timezone;
        TimetWrapper time;
    };

}
