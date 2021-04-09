// Copyright (2015) Gustav

#ifndef CORE_DATETIME_H
#define CORE_DATETIME_H

#include <ctime>
#include <string>
#include <cstdint>

namespace euphoria::core
{

struct TimetWrapper;
struct StructTmWrapper;

enum class Month {
    JANUARY, FEBRUARY, MARCH, APRIL, MAY, JUNE, JULY, AUGUST, SEPTEMBER, OCTOBER, NOVEMBER, DECEMBER
};

struct TimetWrapper
{
protected:
    friend struct StructTmWrapper;
    explicit TimetWrapper(time_t time);
public:
    static TimetWrapper FromLocalTime(const StructTmWrapper& dt);
    static TimetWrapper FromGmt(const StructTmWrapper& dt);
    static TimetWrapper CurrentTime();

    static double Difference(const TimetWrapper& start, const TimetWrapper& end);

    [[nodiscard]] StructTmWrapper ToLocalTime() const;
    [[nodiscard]] StructTmWrapper ToGmt() const;
private:
    time_t time_;
};

enum class DstInfo {
    IN_EFFECT, NOT_IN_EFFECT, INFO_UNAVAILABLE
};

struct StructTmWrapper
{
protected:
    friend struct TimetWrapper;
    explicit StructTmWrapper(struct tm time);
    [[nodiscard]] struct tm time() const;

public:
    StructTmWrapper(int year, Month month, int day);
    StructTmWrapper(int year, Month month, int day, int hour, int minute, int second, bool dst=false);

    void set_seconds(int seconds);
    void set_minutes(int minutes);
    void set_hour(int hour);
    void set_day_of_moth(int day_of_moth);
    void set_month(Month month);
    void set_year(int year);
    void set_dst(DstInfo dst);

    [[nodiscard]] int seconds() const;
    [[nodiscard]] int minutes() const;
    [[nodiscard]] int hour() const;
    [[nodiscard]] int day_of_moth() const;
    [[nodiscard]] Month month() const;
    [[nodiscard]] int year() const;
    [[nodiscard]] DstInfo dst() const;

    // format: http://www.cplusplus.com/reference/ctime/strftime/
    [[nodiscard]] std::string ToString(const std::string& format) const;
    [[nodiscard]] std::string DebugString() const;

private:
    struct tm time_;
};

// unix date time format, 64 bit
// todo(Gustav): test 2038 problem
uint64_t DateTimeToInt64(const TimetWrapper& dt);
TimetWrapper Int64ToDateTime(uint64_t i);

enum class TimeZone
{
    GMT, LOCAL
};

// public interface
// util class to make stuff nice to use
struct DateTime
{
public:
    static DateTime FromDate(int year, Month month, int day, TimeZone timezone = TimeZone::LOCAL);
    static DateTime FromDateTime(int year, Month month, int day, int hour, int minute, int second, TimeZone timezone = TimeZone::LOCAL);
    static DateTime CurrentTime(TimeZone timezone = TimeZone::LOCAL);

    [[nodiscard]] std::string ToString(const std::string& format) const;
    [[nodiscard]] std::string DebugString() const;

    void set_seconds(int seconds);
    void set_minutes(int minutes);
    void set_hour(int hour);
    void set_day_of_moth(int day_of_moth);
    void set_month(Month month);
    void set_year(int year);
    void set_dst(DstInfo dst);

    [[nodiscard]] int seconds() const;
    [[nodiscard]] int minutes() const;
    [[nodiscard]] int hour() const;
    [[nodiscard]] int day_of_moth() const;
    [[nodiscard]] Month month() const;
    [[nodiscard]] int year() const;
    [[nodiscard]] DstInfo dst() const;

    [[nodiscard]] TimeZone timezone() const;
    [[nodiscard]] TimetWrapper time() const;

private:
    DateTime();
    DateTime(TimeZone timezone, const StructTmWrapper& time);
    DateTime(TimeZone timezone, const TimetWrapper& time);

    [[nodiscard]] StructTmWrapper AsStruct() const;
    void UpdateTime(const StructTmWrapper& s);

    TimeZone timezone_;
    TimetWrapper time_;
};

}

#endif    // CORE_DATETIME_H
