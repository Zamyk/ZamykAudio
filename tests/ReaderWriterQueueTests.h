#pragma once

#include <thread>
#include <atomic>

#include "catch/catch.hpp"
#include <ZAudio/ReaderWriterQueue.h>



TEST_CASE("ReaderWriterQueue single thread") {
  using namespace ZAudio::Tools;
  ReaderWriterQueue<int> queue(10);
  REQUIRE(queue.tryPush(123));
  int v = 124;
  REQUIRE(queue.tryPush(v));
  const int v2 = 125;
  REQUIRE(queue.tryPush(v2));
  auto tmp = queue.tryPop();
  REQUIRE(tmp);
  REQUIRE(*tmp == 123);
  tmp = queue.tryPop();
  REQUIRE(tmp);
  REQUIRE(*tmp == 124);
  tmp = queue.tryPop();
  REQUIRE(tmp);
  REQUIRE(*tmp == 125);    
}

struct MoveOnly {
  MoveOnly() = default;
  explicit MoveOnly(int v_p) : v(v_p) {}

  MoveOnly(const MoveOnly& oth) = delete;
  MoveOnly(MoveOnly&& oth) = default;
  MoveOnly& operator= (const MoveOnly& oth) = delete;
  MoveOnly& operator= (MoveOnly&& oth) = default;

  int v = 0;
};

TEST_CASE("ReaderWriterQueue single thread move only") {
  using namespace ZAudio::Tools;
  ReaderWriterQueue<MoveOnly> queue(10);
  REQUIRE(queue.tryPush(MoveOnly(50)));
  MoveOnly m(60);
  REQUIRE(queue.tryPush(std::move(m)));
  auto tmp = queue.tryPop();
  REQUIRE(tmp);
  REQUIRE(tmp->v == 50);
  tmp = queue.tryPop();
  REQUIRE(tmp);
  REQUIRE(tmp->v == 60);
}

using namespace ZAudio::Tools;

void writerThread1(std::atomic_bool& start, const std::vector<int>& vals, ReaderWriterQueue<int>& queue) {    
  while(!start) {
    std::this_thread::yield();
  }
  for(int i = 0; i < vals.size(); i++) {
    queue.waitAndPush(vals[i]);
  }
}

void readerThread1(std::atomic_bool& start, std::vector<int>& vals, int n, ReaderWriterQueue<int>& queue) {  
  while(!start) {
    std::this_thread::yield();
  }
  while(vals.size() < n) {
    if(auto tmp = queue.tryPop()) {
      vals.push_back(*tmp);
    }
  }  
}

TEST_CASE("ReaderWriterQueue two threads #1") {
  using namespace ZAudio::Tools;
  std::atomic_bool start(false);
  int n = 100000;
  std::vector<int> in(n);
  std::iota(in.begin(), in.end(), 0);
  std::vector<int> out;
  ReaderWriterQueue<int> queue(10);

  std::thread thread1(writerThread1, std::ref(start), std::ref(in), std::ref(queue));
  std::thread thread2(readerThread1, std::ref(start), std::ref(out), n, std::ref(queue));
  start = true;
  thread1.join();
  thread2.join();
  REQUIRE(out == in);
}




void writerThread2(std::atomic_bool& start, const std::vector<int>& vals, ReaderWriterQueue<int>& queue) {    
  while(!start) {
    std::this_thread::yield();
  }
  int i = 0;
  while(i < vals.size()) {
    bool succ = queue.tryPush(vals[i]);
    if(succ) {
      i++;
    }
  }
}

void readerThread2(std::atomic_bool& start, std::vector<int>& vals, int n, ReaderWriterQueue<int>& queue) {  
  while(!start) {
    std::this_thread::yield();
  }
  while(vals.size() < n) {
    if(auto tmp = queue.tryPop()) {
      vals.push_back(*tmp);
    }
  }  
}

TEST_CASE("ReaderWriterQueue two threads #2") {
  using namespace ZAudio::Tools;
  std::atomic_bool start(false);
  int n = 100000;
  std::vector<int> in(n);
  std::iota(in.begin(), in.end(), 0);
  std::vector<int> out;
  ReaderWriterQueue<int> queue(12);

  std::thread thread1(writerThread2, std::ref(start), std::ref(in), std::ref(queue));
  std::thread thread2(readerThread2, std::ref(start), std::ref(out), n, std::ref(queue));
  start = true;
  thread1.join();
  thread2.join();
  REQUIRE(out == in);
}
