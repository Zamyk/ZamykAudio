#pragma once

#include <optional>
#include <thread>

namespace ZAudio::Tools {


// only one writer and one reader!
template<typename T>
class ReaderWriterQueue {
public:
  ReaderWriterQueue(size_t size) : buffer(size + 1) {
    assert(size > 0);
  }

  template<typename T2>
  bool tryPush(T2&& v) {    
    if(next(writeIndex) != readIndex) {
      buffer[writeIndex] = std::forward<T2>(v);
      writeIndex = next(writeIndex);
      return true;
    }
    else {
      return false;
    }
  }

  template<typename T2>
  void waitAndPush(T2&& v) {
    while(next(writeIndex) == readIndex) {
      std::this_thread::yield();
    }
    buffer[writeIndex] = std::forward<T2>(v);
    writeIndex = next(writeIndex);    
  }

  std::optional<T> tryPop() {
    if(readIndex != writeIndex) {
      // get value first so push can't change it
      T ans = std::move(buffer[readIndex]);
      readIndex = next(readIndex);
      return std::move(ans);
    }    
    else {
      return std::nullopt;
    }
  }

  T waitAndPop() {
    while(readIndex == writeIndex) {
      std::this_thread::yield();
    }
    T ans = std::move(buffer[readIndex]);
    readIndex = next(readIndex);
    return ans;
  }

private:
  std::vector<T> buffer;
  std::atomic_uint32_t writeIndex = 0;
  std::atomic_uint32_t readIndex = 0;  

  uint32_t next(uint32_t v) {
    return v + 1 >= buffer.size() ? 0 : v + 1;
  }
};


} // namespace