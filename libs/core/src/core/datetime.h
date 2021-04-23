#pragma once

#include <ctime>
#include <string>
#include <cstdint>

namespace euphoria::core
{
    struct time_t_wrapper;
    struct struct_tm_wrapper;

    enum class month {
        JANUARY, FEBRUARY, MARCH, APRIL, MAY, JUNE, JULY, AUGUST, SEPTEMBER, OCTOBER, NOVEMBER, DECEMBER
    };

    struct time_t_wrapper
    {
        friend struct struct_tm_wrapper;
        explicit time_t_wrapper(time_t time);

        static time_t_wrapper from_local_time(const struct_tm_wrapper& dt);
        static time_t_wrapper from_gmt(const struct_tm_wrapper& dt);
        static time_t_wrapper from_current_time();

        static double get_difference(const time_t_wrapper& start, const time_t_wrapper& end);

        [[nodiscard]] struct_tm_wrapper to_local_time() const;
        [[nodiscard]] struct_tm_wrapper to_gmt() const;

        time_t time;
    };

    enum class dst_info {
        in_effect, not_in_effect, info_unavailable
    };

    struct struct_tm_wrapper
    {
    protected:
        friend struct time_t_wrapper;
        explicit struct_tm_wrapper(struct tm time);
        [[nodiscard]] struct tm time() const;

    public:
        struct_tm_wrapper(int year, core::month month, int day);
        struct_tm_wrapper(int year, core::month month, int day, int hour, int minute, int second, bool dst=false);

        void set_seconds(int seconds);
        void set_minutes(int minutes);
        void set_hour(int hour);
        void set_day_of_moth(int day_of_moth);
        void set_month(core::month month);
        void set_year(int year);
        void set_dst(dst_info dst);

        [[nodiscard]] int get_seconds() const;
        [[nodiscard]] int get_minutes() const;
        [[nodiscard]] int get_hour() const;
        [[nodiscard]] int get_day_of_moth() const;
        [[nodiscard]] month get_month() const;
        [[nodiscard]] int get_year() const;
        [[nodiscard]] dst_info get_dst() const;

        // format: http://www.cplusplus.com/reference/ctime/strftime/
        [[nodiscard]] std::string to_string(const std::string& format) const;
        [[nodiscard]] std::string to_debug_string() const;

    private:
        struct tm time_;
    };

    // unix date time format, 64 bit
    // todo(Gustav): test 2038 problem
    uint64_t date_time_to_int64(const time_t_wrapper& dt);
    time_t_wrapper int64_to_date_time(uint64_t i);

    enum class time_zone
    {
        gmt, local
    };

    // public interface
    // util class to make stuff nice to use
    struct date_time
    {
    public:
        static date_time FromDate(int year, core::month month, int day, time_zone timezone = time_zone::local);
        static date_time FromDateTime(int year, core::month month, int day, int hour, int minute, int second, time_zone timezone = time_zone::local);
        static date_time CurrentTime(time_zone timezone = time_zone::local);

        [[nodiscard]] std::string ToString(const std::string& format) const;
        [[nodiscard]] std::string DebugString() const;

        void set_seconds(int seconds);
        void set_minutes(int minutes);
        void set_hour(int hour);
        void set_day_of_moth(int day_of_moth);
        void set_month(core::month month);
        void set_year(int year);
        void set_dst(dst_info dst);

        [[nodiscard]] int get_seconds() const;
        [[nodiscard]] int get_minutes() const;
        [[nodiscard]] int get_hour() const;
        [[nodiscard]] int get_day_of_month() const;
        [[nodiscard]] month get_month() const;
        [[nodiscard]] int get_year() const;
        [[nodiscard]] dst_info get_dst() const;

        [[nodiscard]] time_zone get_timezone() const;
        [[nodiscard]] time_t_wrapper get_time() const;

    private:
        date_time();
        date_time(time_zone timezone, const struct_tm_wrapper& time);
        date_time(time_zone timezone, const time_t_wrapper& time);

        [[nodiscard]] struct_tm_wrapper as_struct() const;
        void update_time(const struct_tm_wrapper& s);

        time_zone timezone_;
        time_t_wrapper time_;
    };

}
