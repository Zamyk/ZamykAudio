#pragma once

#include "catch/catch.hpp"
#include <ZAudio/StringTools.h>



TEST_CASE("String Tools") {
  using namespace ZAudio;
  REQUIRE(StringTools::caseInsensitiveEqual("aBcDefgHiJkL", "ABCDEFGhijkl"));
  REQUIRE(StringTools::caseInsensitiveEqual("a", "A"));
  REQUIRE(StringTools::caseInsensitiveEqual("abcdefgh", "abcdefgh"));
  REQUIRE(StringTools::caseInsensitiveEqual("abcdefghZZ", "abcdefghzz"));
  REQUIRE(!StringTools::caseInsensitiveEqual("aBcDefgHhiJkL", "ABCDEFGhijklm"));
  REQUIRE(!StringTools::caseInsensitiveEqual("a", "b"));
  REQUIRE(!StringTools::caseInsensitiveEqual("A", "AB"));

  auto tmp = StringTools::stringToDouble("0.123");
  REQUIRE(tmp);
  REQUIRE_THAT(*tmp, Catch::Matchers::WithinAbs(0.123, 0.0001));
  tmp = StringTools::stringToDouble("-1200");
  REQUIRE(tmp);
  REQUIRE_THAT(*tmp, Catch::Matchers::WithinAbs(-1200., 0.0001));
  tmp = StringTools::stringToDouble("abcdefgh");
  REQUIRE(!tmp);

  auto tmp2 = StringTools::stringToInt("123");
  REQUIRE(tmp2);
  REQUIRE_THAT(*tmp2, Catch::Matchers::WithinAbs(123, 0.0001));
  tmp2 = StringTools::stringToInt("-12");
  REQUIRE(tmp2);
  REQUIRE_THAT(*tmp2, Catch::Matchers::WithinAbs(-12, 0.0001));
  tmp2 = StringTools::stringToInt("abcdefgh");
  REQUIRE(!tmp2);  
}