#pragma once

#include "catch/catch.hpp"
#include <ZAudio/TwoDimVector.h>



TEST_CASE("TwoDimVector #1") {
  using namespace ZAudio::Tools;
  TwoDimVector<int> v(3, 2);
  v.get(0, 0) = 1;
  v.get(0, 1) = 2;  
  v.get(1, 0) = 3;
  v.get(1, 1) = 4;    
  v.get(2, 0) = 5;
  v.get(2, 1) = 6;    
  REQUIRE(v.getNumOfCollumns() == 2);
  REQUIRE(v.getNumOfRows() == 3);

  int val = 2;
  for(auto it = v.getCollumnBegin(1); it != v.getCollumnEnd(1); ++it) {    
    REQUIRE(*it == val);
    val += 2;
  }

  val = 3;
  for(auto& a : v.getRow(1)) {
    REQUIRE(a == val);
    val++;
  }

  v.resize(2, 1);
  REQUIRE(v.getNumOfCollumns() == 1);
  REQUIRE(v.getNumOfRows() == 2);  
  v.resize(10, 12);
  REQUIRE(v.getNumOfCollumns() == 12);
  REQUIRE(v.getNumOfRows() == 10);  
} 