// Copyright (2015) Gustav

#ifndef CORE_DATETIME_H
#define CORE_DATETIME_H

#include <ctime>
#include <string>
#include <cstdint>

namespace euphoria::core
{

class TimetWrapper;
class StructTmWrapper;

enum class Month {
  JANUARY, FEBRUARY, MARCH, APRIL, MAY, JUNE, JULY, AUGUST, SEPTEMBER, OCTOBER, NOVEMBER, DECEMBER
};

class TimetWrapper {
protected:
  friend class StructTmWrapper;
  explicit TimetWrapper(time_t time);
public:
  static TimetWrapper FromLocalTime(const StructTmWrapper& dt);
  static TimetWrapper FromGmt(const StructTmWrapper& dt);
  static TimetWrapper CurrentTime();

  static double Difference(const TimetWrapper& start, const TimetWrapper& end);

  StructTmWrapper ToLocalTime() const;
  StructTmWrapper ToGmt() const;
private:
  time_t time_;
};

enum class DstInfo {
  IN_EFFECT, NOT_IN_EFFECT, INFO_UNAVAILABLE
};

class StructTmWrapper {
protected:
  friend class TimetWrapper;
  explicit StructTmWrapper(struct tm time);
  struct tm time() const;

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

  int seconds() const;
  int minutes() const;
  int hour() const;
  int day_of_moth() const;
  Month month() const;
  int year() const;
  DstInfo dst() const;

  // format: http://www.cplusplus.com/reference/ctime/strftime/
  std::string ToString(const std::string& format) const;
  std::string DebugString() const;

private:
  struct tm time_;
};

// unix date time format, 64 bit
// todo: test 2038 problem
uint64_t DateTimeToInt64(const TimetWrapper& dt);
TimetWrapper Int64ToDateTime(uint64_t i);

enum class TimeZone {
  GMT, LOCAL
};

// public interface
// util class to make stuff nice to use
class DateTime {
public:
  static DateTime FromDate(int year, Month month, int day, TimeZone timezone = TimeZone::LOCAL);
  static DateTime FromDateTime(int year, Month month, int day, int hour, int minute, int second, TimeZone timezone = TimeZone::LOCAL);
  static DateTime CurrentTime(TimeZone timezone = TimeZone::LOCAL);

public:
  std::string ToString(const std::string& format) const;
  std::string DebugString() const;

public:
  void set_seconds(int seconds);
  void set_minutes(int minutes);
  void set_hour(int hour);
  void set_day_of_moth(int day_of_moth);
  void set_month(Month month);
  void set_year(int year);
  void set_dst(DstInfo dst);

  int seconds() const;
  int minutes() const;
  int hour() const;
  int day_of_moth() const;
  Month month() const;
  int year() const;
  DstInfo dst() const;

public:
  TimeZone timezone() const;
  TimetWrapper time() const;

private:
  DateTime();
  DateTime(TimeZone timezone, const StructTmWrapper& time);
  DateTime(TimeZone timezone, const TimetWrapper& time);

private:
  StructTmWrapper AsStruct() const;
  void UpdateTime(const StructTmWrapper& s);

private:
  TimeZone timezone_;
  TimetWrapper time_;
};

}

#endif  // CORE_DATETIME_H
