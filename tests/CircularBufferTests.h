#pragma once

#include "catch/catch.hpp"
#include <ZAudio/CircularBuffer.h>

TEST_CASE("Circular buffer #1 push get") {
  using ZAudio::Tools::CircularBuffer;
  CircularBuffer<int> cb(10);
  REQUIRE(cb.size() == 10);
  cb.push(1);
  REQUIRE(cb.get(0) == 1);
  cb.push(2);
  REQUIRE(cb.get(0) == 2);
  cb.push(3);
  REQUIRE(cb.get(0) == 3);
  cb.push(4);
  REQUIRE(cb.get(0) == 4);
  cb.push(5);
  REQUIRE(cb.get(0) == 5);
  cb.push(6);
  REQUIRE(cb.get(0) == 6);  
  REQUIRE(cb.get(1) == 5);
  REQUIRE(cb.get(2) == 4);
  REQUIRE(cb.get(3) == 3);
  REQUIRE(cb.get(4) == 2);
  REQUIRE(cb.get(5) == 1);
  cb.push(7);
  cb.push(8);
  cb.push(9);
  cb.push(10);
  REQUIRE(cb.get(0) == 10);  
  REQUIRE(cb.get(1) == 9);
  REQUIRE(cb.get(2) == 8);
  cb.push(11);
  REQUIRE(cb.get(0) == 11);
  REQUIRE(cb.get(10) == 11);
  REQUIRE(cb.get(11) == 10);
}

TEST_CASE("Circular buffer #2 iterator") {
  using ZAudio::Tools::CircularBuffer;
  CircularBuffer<int> buffer(5);
  REQUIRE(buffer.size() == 5);
  buffer.push(1);
  buffer.push(2);
  buffer.push(3);
  buffer.push(4);
  buffer.push(5);  
  auto curr = buffer.current();
  REQUIRE(*curr == 5);
  curr++;
  REQUIRE(*curr == 4);
  ++curr;
  REQUIRE(*curr == 3);
  curr++;
  REQUIRE(*curr == 2);
  ++curr;
  REQUIRE(*curr == 1);
  curr++;
  REQUIRE(*curr == 5);
  curr--;
  REQUIRE(*curr == 1);
  --curr;
  REQUIRE(*curr == 2);
  curr--;
  REQUIRE(*curr == 3);
  --curr;
  REQUIRE(*curr == 4);
}

TEST_CASE("Circular buffer #2 resize") {
  using ZAudio::Tools::CircularBuffer;
  CircularBuffer<int> buffer(5);
  REQUIRE(buffer.size() == 5);
  buffer.push(1);
  buffer.push(2);
  buffer.push(3);
  buffer.push(4);
  buffer.push(5);
  
  buffer.resize(7);
  REQUIRE(buffer.size() == 7);
  REQUIRE(buffer.get(0) == 5);
  REQUIRE(buffer.get(1) == 4);
  REQUIRE(buffer.get(2) == 3);
  REQUIRE(buffer.get(3) == 2);
  REQUIRE(buffer.get(4) == 1);
  buffer.push(6);
  REQUIRE(buffer.get(0) == 6);
  REQUIRE(buffer.get(1) == 5);
  REQUIRE(buffer.get(2) == 4);
  REQUIRE(buffer.get(3) == 3);
  REQUIRE(buffer.get(4) == 2);
  REQUIRE(buffer.get(5) == 1);

  buffer.resize(2);
  REQUIRE(buffer.size() == 2);
  REQUIRE(buffer.get(0) == 6);
  REQUIRE(buffer.get(1) == 5);
  REQUIRE(buffer.get(2) == 6);
  REQUIRE(buffer.get(3) == 5);
}

TEST_CASE("doc example") {
  using namespace ZAudio::Tools;
  CircularBuffer<int> buffer(5);  
  buffer.push(1);
  buffer.push(2);
  buffer.push(3);
  buffer.push(4);
  buffer.push(5);

  REQUIRE(buffer.get(0) == 5);
  REQUIRE(buffer.get(1) == 4);
  REQUIRE(buffer.get(2) == 3);
  REQUIRE(buffer.get(3) == 2);
  REQUIRE(buffer.get(4) == 1);

  buffer.push(6);
  REQUIRE(buffer.get(0) == 6);
  REQUIRE(buffer.get(1) == 5);
  REQUIRE(buffer.get(2) == 4);
  REQUIRE(buffer.get(3) == 3);
  REQUIRE(buffer.get(4) == 2);

  buffer.resize(2);
  REQUIRE(buffer.get(0) == 6);
  REQUIRE(buffer.get(1) == 5);

  buffer.reset(4);
  REQUIRE(buffer.get(0) == 0);
  REQUIRE(buffer.get(1) == 0);
  REQUIRE(buffer.get(2) == 0);
  REQUIRE(buffer.get(3) == 0);

  buffer.push(5);
  buffer.push(15);  
  REQUIRE(buffer.getFrictional(0.5) == 10);
}