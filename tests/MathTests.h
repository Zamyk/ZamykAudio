#pragma once

#include "catch/catch.hpp"
#include <ZAudio/Math.h>



TEST_CASE("Math") {
  using namespace ZAudio;
  REQUIRE(Math::isPrime(3));
  REQUIRE(Math::isPrime(5));
  REQUIRE(Math::isPrime(13));
  REQUIRE(Math::isPrime(971));
  REQUIRE(Math::isPrime(263));
  REQUIRE(!Math::isPrime(4));
  REQUIRE(!Math::isPrime(24));
  REQUIRE(!Math::isPrime(100));
  REQUIRE(!Math::isPrime(36));
  REQUIRE(!Math::isPrime(50));  

  REQUIRE(Math::getFirstN_PrimeNumbers(10) == std::vector<int64_t>{2, 3, 5, 7, 11, 13, 17, 19, 23, 29});
  REQUIRE_THAT(Math::linearInterpolation(10., 50., 0.2), Catch::Matchers::WithinAbs(18., 0.0001));
  REQUIRE_THAT(Math::linearInterpolation(-20., -10., 0.1), Catch::Matchers::WithinAbs(-19., 0.0001));
  REQUIRE_THAT(Math::dist(10., 50.), Catch::Matchers::WithinAbs(40., 0.0001));
  REQUIRE_THAT(Math::dist(-20., -10.), Catch::Matchers::WithinAbs(10., 0.0001));
  REQUIRE_THAT(Math::dist(-20., 10.), Catch::Matchers::WithinAbs(30., 0.0001));  
  REQUIRE_THAT(Math::sgn(-20.), Catch::Matchers::WithinAbs(-1., 0.0001));
  REQUIRE_THAT(Math::sgn(-0.5), Catch::Matchers::WithinAbs(-1., 0.0001));
  REQUIRE_THAT(Math::sgn(0.), Catch::Matchers::WithinAbs(0., 0.0001));
  REQUIRE_THAT(Math::sgn(10.), Catch::Matchers::WithinAbs(1., 0.0001));
  REQUIRE_THAT(Math::sgn(0.5), Catch::Matchers::WithinAbs(1., 0.0001));  
}