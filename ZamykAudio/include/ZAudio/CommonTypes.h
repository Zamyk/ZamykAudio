#pragma once

#include <vector>
#include <memory>
#include <cassert>
#include <chrono>
#include <string>
#include <cmath>

#include <ZAudio/StringTools.h>

namespace ZAudio {


using sample_t = double;

class Result {
public:
  Result() = default;
  Result(bool error_p, std::string description_p) : 
    error_(error_p),
    description_(std::move(description_p)) {}

  static Result error(std::string description_p) {
    return Result(true, std::move(description_p));
  }

  static Result success() {
    return Result(false, "");
  }  

  Result operator & (const Result& oth) {
    return Result(error_ |= oth.error_, combineDescriptions(description_, oth.description_));    
  }

  void operator &= (const Result& oth) {
    *this = *this & oth;
  }

  explicit operator bool() const {
    return !error_; 
  }

  std::string getDescription() const {
    return description_;
  }

private:
  bool error_ = true;
  std::string description_;

  static std::string combineDescriptions(const std::string& d1, const std::string& d2) {
    if(d1.empty()) {
      return d2;
    }
    if(d2.empty()) {
      return d1;
    }
    return d1 + " | " + d2;
  }
};

template<typename T>
class ResultValue {
public:
  ResultValue() = default;
  
  ResultValue(T&& value_p, Result result_p) :
    value(std::move(value_p)),
    result(std::move(result_p)) {}
  
  ResultValue(const T& value_p, Result result_p) :
    value(value_p),
    result(std::move(result_p)) {}

  /*implicit*/ ResultValue(T&& value_p) :
    value(std::move(value_p)),
    result(Result::success()) {}

  /*implicit*/ ResultValue(const T& value_p) :
    value(value_p),
    result(Result::success()) {}  
  
  /*implicit*/ ResultValue(Result result_p) :
    value(),
    result(result_p) {}    

  explicit operator bool() const {
    return static_cast<bool>(result); 
  }

  std::string getDescription() const {
    return result.getDescription();
  }

  T& get() {
    assert(result.success());
    return value;
  }

  const T& get() const {
    assert(result.success());
    return value;
  }

private:
  T value;
  Result result;
};


class Volume {
public:
  Volume() = default;

  static Volume dB(double v) {
    return Volume(pow(10, v / 10));
  }

  constexpr static Volume linear(double v) {
    return Volume(v);
  }

  double dB() const {
    if(linearValue <= Volume::dB(-96).linear()) {
      return -96;
    }
    return 10 * log10(linearValue);
  }

  double linear() const {
    return linearValue;
  }  

  bool operator < (const Volume& oth) const {
    return linearValue < oth.linearValue;
  }

  bool operator > (const Volume& oth) const {
    return linearValue > oth.linearValue;
  }

  std::string toString() const {
    return std::to_string(dB()) + "dB";
  }  

  static std::optional<Volume> fromString(const std::string& str) {
    if(str.size() > 2 && StringTools::caseInsensitiveEqual(str.substr(str.size() - 2), "db")) {
      auto v = StringTools::stringToDouble(str.substr(0, str.size() - 2));
      if(v) {
        return dB(*v);        
      }      
    }    
    return std::nullopt;
  }

private:
  double linearValue = 1.;

  constexpr explicit Volume(double linear_p) : linearValue(linear_p) {}
};


class Time {
public:
  Time() = default;

  constexpr static Time minutes(double minutes) {
    constexpr double SecondsPerMinute = 60;
    return Time(minutes * SecondsPerMinute);
  }

  constexpr static Time seconds(double seconds_p) {
    return Time(seconds_p);
  }

  constexpr static Time miliseconds(double miliseconds_p) {
    constexpr double mili = 1000.;
    return Time(miliseconds_p / mili);
  }

  constexpr static Time microseconds(double microseconds) {
    constexpr double micro = 1000000.;
    return Time(microseconds / micro);
  }

  constexpr double minutes() const {
    constexpr double SecondsPerMinute = 60;
    return timeInSeconds / SecondsPerMinute;
  }

  constexpr double seconds() const {
    return timeInSeconds;
  }

  constexpr double miliseconds() const {
    constexpr double mili = 1000.;
    return timeInSeconds * mili;
  }

  constexpr double microseconds() const {
    constexpr double micro = 1000000.;
    return timeInSeconds * micro;
  }

  constexpr Time operator + (const Time& oth) const {
    return Time(timeInSeconds + oth.timeInSeconds);
  }

  constexpr Time operator - (const Time& oth) const {
    return Time(timeInSeconds - oth.timeInSeconds);
  }

  constexpr bool operator < (const Time& oth) const {
    return timeInSeconds < oth.timeInSeconds;
  }

  constexpr bool operator > (const Time& oth) const {
    return timeInSeconds > oth.timeInSeconds;
  }

  constexpr Time operator / (double d) const {
    return Time(timeInSeconds / d);
  }

  constexpr Time operator * (double d) const {
    return Time(timeInSeconds * d);
  }  

  friend Time operator * (double d, Time t) {
    return t * d;
  }

  std::string toString() const {
    return std::to_string(timeInSeconds) + "s";
  }

  static std::optional<Time> fromString(const std::string& str) {
    if(str.size() > 2 && StringTools::caseInsensitiveEqual(str.substr(str.size() - 2), "ms")) {      
      auto v = StringTools::stringToDouble(str.substr(0, str.size() - 2));
      if(v) {
        return miliseconds(*v);        
      }    
    }    
    if(str.size() > 1 && StringTools::caseInsensitiveEqual(str.substr(str.size() - 1), "s")) {      
      auto v = StringTools::stringToDouble(str.substr(0, str.size() - 1));
      if(v) {
        return seconds(*v);        
      }    
    }    
    return std::nullopt;
  }

private:
  double timeInSeconds = 0.;
  explicit constexpr Time(double timeInSeconds_p) : timeInSeconds(timeInSeconds_p) {}
};

class Frequency {
public:    
  Frequency() = default;

  constexpr static Frequency Hz(double v) {
    return Frequency(v);
  }

  constexpr static Frequency KHz(double v) {
    constexpr double kilo = 1000.;
    return Frequency(v * kilo);
  }

  constexpr double Hz() const {
    return perSecond;
  }

  constexpr double KHz() const {
    constexpr double kilo = 1000.;
    return perSecond / kilo;
  }

  constexpr Time getPeriod() const {
     return Time::seconds(1. / perSecond);
  }

  std::chrono::nanoseconds getChronoPeriod() const {
    const uint32_t Nano = 1000000000;
    return std::chrono::nanoseconds(static_cast<uint32_t>(Nano / perSecond));
  }

  constexpr Frequency operator* (double mul) const {
    return Frequency(perSecond * mul);
  }

  friend Frequency operator*(double mul, Frequency f) {
    return f * mul;
  }

  Frequency& operator*= (double mul) {
    *this = *this * mul;
    return *this;
  }

  constexpr Frequency operator/ (double div) const {
    return Frequency(perSecond / div);
  }  

  Frequency& operator/= (double div) {
    *this = *this / div;
    return *this;
  }

  constexpr double operator/ (Frequency oth) const {
    return perSecond / oth.perSecond;
  }

  constexpr bool operator < (Frequency oth) const {
    return perSecond < oth.perSecond;
  }

  std::string toString() const {
    return std::to_string(perSecond) + "hz";
  }

  bool operator == (const Frequency& oth) const {
    constexpr double Eps = 0.00001;
    return std::fabs(perSecond - oth.perSecond) < Eps;
  }

  bool operator != (const Frequency& oth) const {
    return !(oth == *this);
  }
  
  static std::optional<Frequency> fromString(const std::string& str) {
    if(str.size() > 2 && StringTools::caseInsensitiveEqual(str.substr(str.size() - 2), "hz")) {
      auto v = StringTools::stringToDouble(str.substr(0, str.size() - 2));
      if(v) {
        return Hz(*v);        
      }      
    }    
    return std::nullopt;
  }

private:
  double perSecond = 0;

  explicit constexpr Frequency(double perSecond_p) : perSecond(perSecond_p) {}
};


struct ParameterValue {
public:
  ParameterValue() = default;
  /*implicit*/ // ParameterValue(Frequency frequency) : floating(frequency.Hz()) {}
  /*implicit*/ // ParameterValue(Volume volume) : floating(volume.linear()) {}
  /*implicit*/ // ParameterValue(Time time) : floating(time.seconds()) {}
  /*implicit*/ // ParameterValue(int32_t i) : floating(i) {}
  /*implicit*/ // ParameterValue(bool b) : floating(b ? 2. : 0.) {}
  /*implicit*/ // ParameterValue(double v) : floating(v) {}  

  static ParameterValue frequency(Frequency frequency) {
    return ParameterValue(0, frequency.Hz());
  }

  static ParameterValue volume(Volume volume) {
    return ParameterValue(0, volume.linear());
  }

  static ParameterValue time(Time time) {
    return ParameterValue(0, time.seconds());
  }

  static ParameterValue integer(int32_t i) {
    return ParameterValue(i, 0);
  }

  static ParameterValue nonInteger(double d) {
    return ParameterValue(0, d);
  }

  static ParameterValue boolean(bool b) {
    return ParameterValue(b, 0.);
  }

  template<typename T>
  static ParameterValue fromEnum(T v) {
    static_assert(std::is_enum<T>::value);    
    return ParameterValue(static_cast<int64_t>(v), 0.);
  }  

  Frequency getFrequency() const {
    return Frequency::Hz(floating_);
  }

  Volume getVolume() const {
    return Volume::linear(floating_);    
  }

  Time getTime() const {
    return Time::seconds(floating_);
  }

  int getInteger() const {
    return integer_;
  }

  double getNonInteger() const {
    return floating_;
  }

  bool getBoolean() const {
    return integer_;
  }

  template<typename T>
  T getEnum() const {
    static_assert(std::is_enum<T>::value);
    return static_cast<T>(integer_);
  }

private:    
  ParameterValue(int64_t integer_p, double floating_p) : integer_(integer_p), floating_(floating_p) {}
  int64_t integer_ = 0;
  double floating_ = 0;
};

class NormalizedValue {
public:

  NormalizedValue() = default;

  explicit NormalizedValue(double value) : v(value) {
    assert(value >= 0 && value <= 1);
  }

  NormalizedValue(double l, double r, double value) : v( (value - l) / (r - l) ) {}

  double bind(double l, double r) const {
    return (r - l) * v + l;
  }

  double bindFromLeft(double l, double r, double depth) const {
    return bind(l, NormalizedValue(depth).bind(l, r));
  }

  double bindFromRight(double l, double r, double depth) const {
    return bind(NormalizedValue(depth).bind(l, r), r);
  }

private:
  double v;
};


} // namespace ZAudio