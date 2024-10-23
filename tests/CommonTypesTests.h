#pragma once

#include "catch/catch.hpp"
#include <ZAudio/CommonTypes.h>

TEST_CASE("Volume tests") {
  using namespace ZAudio;  
  Volume vol = Volume::dB(0);
  REQUIRE_THAT(vol.linear(), Catch::Matchers::WithinAbs(1., 0.0001));
  vol = Volume::dB(3);
  REQUIRE_THAT(vol.linear(), Catch::Matchers::WithinAbs(1.995262315, 0.0001));
  vol = Volume::dB(-3);
  REQUIRE_THAT(vol.linear(), Catch::Matchers::WithinAbs(0.5011872336, 0.0001));
  vol = Volume::dB(-1.5);
  REQUIRE_THAT(vol.linear(), Catch::Matchers::WithinAbs(0.7079457844, 0.0001));
  REQUIRE(Volume::dB(5.) > Volume::dB(4.5));

  vol = Volume::linear(1.);
  REQUIRE_THAT(vol.dB(), Catch::Matchers::WithinAbs(0., 0.0001));

  auto tmp = Volume::fromString("5db");
  REQUIRE(tmp);
  REQUIRE_THAT(tmp->dB(), Catch::Matchers::WithinAbs(5., 0.0001));
}

TEST_CASE("Time tests") {
  using namespace ZAudio;  
  Time time = Time::seconds(2.);
  REQUIRE_THAT(time.miliseconds(), Catch::Matchers::WithinAbs(2000., 0.0001));
  REQUIRE_THAT(time.microseconds(), Catch::Matchers::WithinAbs(2000000., 0.0001));

  auto tmp = Time::fromString("5s");
  REQUIRE(tmp);
  REQUIRE_THAT(tmp->seconds(), Catch::Matchers::WithinAbs(5., 0.0001));

  REQUIRE(Time::seconds(5.) < Time::miliseconds(5010));
  REQUIRE(Time::seconds(1.) > Time::miliseconds(300));

  REQUIRE_THAT((Time::seconds(3.) * 2.5).seconds(), Catch::Matchers::WithinAbs(3. * 2.5, 0.0001));
  REQUIRE_THAT((2.5 * Time::seconds(3.)).seconds(), Catch::Matchers::WithinAbs(3. * 2.5, 0.0001));

  REQUIRE_THAT((Time::seconds(15.) / 0.5).seconds(), Catch::Matchers::WithinAbs(30., 0.0001));
  REQUIRE_THAT((2.5 * Time::seconds(3.)).seconds(), Catch::Matchers::WithinAbs(7.5, 0.0001));

  REQUIRE_THAT((Time::seconds(3.5)).seconds(), Catch::Matchers::WithinAbs(3.5, 0.0001));
  REQUIRE_THAT((Time::seconds(3.5)).miliseconds(), Catch::Matchers::WithinAbs(3500, 0.0001));
  REQUIRE_THAT((Time::seconds(3.5)).microseconds(), Catch::Matchers::WithinAbs(3500000, 0.0001));

  REQUIRE_THAT((Time::miliseconds(11)).seconds(), Catch::Matchers::WithinAbs(0.011, 0.0001));
  REQUIRE_THAT((Time::miliseconds(11)).miliseconds(), Catch::Matchers::WithinAbs(11, 0.0001));
  REQUIRE_THAT((Time::miliseconds(11)).microseconds(), Catch::Matchers::WithinAbs(11000, 0.0001));

  REQUIRE_THAT((Time::seconds(0.3) * 2.5).seconds(), Catch::Matchers::WithinAbs(0.75, 0.0001));
  REQUIRE_THAT((Time::seconds(0.3) / 2.).seconds(), Catch::Matchers::WithinAbs(0.15, 0.0001));  
}

TEST_CASE("Frequency tests") {
  using namespace ZAudio;  
  Frequency f = Frequency::Hz(155.5);
  REQUIRE_THAT(f.KHz(), Catch::Matchers::WithinAbs(0.1555, 0.0001));
  f = Frequency::KHz(1.2);
  REQUIRE_THAT(f.Hz(), Catch::Matchers::WithinAbs(1200., 0.0001));

  auto tmp = Frequency::fromString("11.4Hz");
  REQUIRE(tmp);
  REQUIRE_THAT(tmp->Hz(), Catch::Matchers::WithinAbs(11.4, 0.0001));

  REQUIRE_THAT(Frequency::Hz(5).getPeriod().seconds(), Catch::Matchers::WithinAbs(1. / 5., 0.0001));
  REQUIRE_THAT((Frequency::Hz(2.) * 1.5).Hz(), Catch::Matchers::WithinAbs(3., 0.0001));
  REQUIRE_THAT((Frequency::Hz(5.) / 2.).Hz(), Catch::Matchers::WithinAbs(2.5, 0.0001));
  REQUIRE_THAT(Frequency::Hz(2.) / Frequency::Hz(4.), Catch::Matchers::WithinAbs(0.5, 0.0001));
  REQUIRE(Frequency::Hz(4.) == Frequency::Hz(4.));
  REQUIRE(Frequency::Hz(4.) != Frequency::Hz(3.6));
}

TEST_CASE("Normalized value tests") {
  using namespace ZAudio;  
  NormalizedValue n(0.5);
  REQUIRE_THAT(n.bind(10., 15.), Catch::Matchers::WithinAbs(12.5, 0.0001));
  REQUIRE_THAT(n.bind(-10., 10.), Catch::Matchers::WithinAbs(0., 0.0001));
  REQUIRE_THAT(n.bind(15., 15.), Catch::Matchers::WithinAbs(15., 0.0001));

  n = NormalizedValue(0., 2., 1.6);  
  REQUIRE_THAT(n.bindFromLeft(10., 20., 0.5), Catch::Matchers::WithinAbs(14., 0.0001));
  REQUIRE_THAT(n.bindFromRight(10., 20., 0.5), Catch::Matchers::WithinAbs(19., 0.0001));
}

TEST_CASE("Example result tests") {
  using namespace ZAudio;  
  Result result = Result::success();  
  REQUIRE(result);

  result = Result::error("Some very bad things happend!");
  REQUIRE(!result);  
  REQUIRE(result.getDescription() == "Some very bad things happend!");  
  
  result &= Result::success();
  REQUIRE(!result);  

  Result err1 = Result::error("very");
  Result err2 = Result::error("bad");
  Result err3 = Result::success();
  REQUIRE(!(err1 & err2 & err3));
  REQUIRE((err1 & err2 & err3).getDescription() == "very | bad");  
}

using namespace ZAudio;

ResultValue<int> functionThatMayFail(std::string str, int v) {
  if(str == ":)") {
    return 30 * v;
  }
  else {
    return Result::error("Why not smiling?");
  }
}

TEST_CASE("Example resultvalue tests") {
  auto res = functionThatMayFail(":)", 2);
  REQUIRE(res);
  REQUIRE(res.get() == 60);  
  res = functionThatMayFail(":(", 1);
  REQUIRE(!res);
}