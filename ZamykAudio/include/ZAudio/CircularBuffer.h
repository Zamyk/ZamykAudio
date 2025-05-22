#pragma once

#include <algorithm>
#include <cmath>
#include <ZAudio/Math.h>

#include <cassert>

namespace ZAudio::Tools {


template<typename T>
class CircularBuffer {
public:
class Iterator {
public:
  Iterator() = default;
  Iterator(std::vector<T>& buffer_p, int32_t curr_p) : buffer(&buffer_p), curr(curr_p) {
    assert(buffer == nullptr || (curr >= 0 && curr < static_cast<int32_t>(buffer->size())));
  }

  bool valid() const {
    return buffer != nullptr && static_cast<int32_t>(buffer->size()) > curr;
  }

  Iterator operator + (int32_t v) const {
    return Iterator(*buffer, (curr - v < 0 ? curr - v + buffer->size() : curr - v));
  }

  Iterator operator - (int32_t v) const {    
    return Iterator(*buffer, (curr + v >= buffer->size() ? curr + v - buffer->size() : curr + v));
  }  

  Iterator& operator += (int32_t v) {    
    *this = *this + v;
    return *this;
  }

  Iterator& operator -= (int32_t v) {
    *this = *this - v;
    return *this;
  }

  Iterator& operator++() {
    return *this += 1;    
  }

  Iterator operator++(int) & {
    auto tmp = *this;
    ++(*this);
    assert(curr >= 0 && curr < static_cast<int32_t>(buffer->size()));
    return tmp;
  }

  Iterator& operator--() {
    return *this -= 1;    
  }

  Iterator operator--(int) & {
    auto tmp = *this;
    --(*this);
    assert(curr >= 0 && curr < static_cast<int32_t>(buffer->size()));
    return tmp;
  }  

  T* operator->() const {
    assert(curr >= 0 && curr < buffer->size());
    return (*buffer)[curr];
  }  

  T& operator*() const {
    assert(curr >= 0 && curr < buffer->size());
    return (*buffer)[curr];
  }

private:
  std::vector<T>* buffer = nullptr;
  int32_t curr = 0;
};


  CircularBuffer() = default;
  explicit CircularBuffer(size_t size) : buffer(size) {}

  void reset(size_t size) {    
    buffer.clear();
    buffer.resize(size);        
    curr = 0;
  }  

  void resize(size_t newSize) {
    if(buffer.empty()) {
      buffer.resize(newSize);
    }
    else {
      if(newSize < buffer.size()) {            
        rotate(newSize - curr - 1);
        buffer.resize(newSize);
        curr = buffer.size() - 1;
      }
      else {
        // rotate, so newest element is at the end
        rotate(buffer.size() - curr - 1);
        curr = buffer.size() - 1;
        buffer.resize(newSize);    
      }
    }
  }

  void push(T sample) {    
    curr++;
    if(curr == static_cast<int32_t>(buffer.size())) {
      curr = 0;
    }
    buffer[curr] = sample;    
  }

  T get(size_t offset) const {          
    int32_t ind = curr - static_cast<int32_t>(offset);
    if(ind < 0) {
      ind += buffer.size();
    }
    return buffer[ind];
  }

  T getFrictional(double offset) const {
    const double v1 = get(static_cast<size_t>(offset));
    const double v2 = get(static_cast<size_t>(offset) + 1);
    return Math::linearInterpolation(v1, v2, offset - std::floor(offset));
  }

  Iterator current() {
    return Iterator(buffer, curr);
  }

  size_t size() const {
    return buffer.size();
  }

private:
  std::vector<T> buffer;
  int32_t curr = 0;
  
  void rotate(int32_t n) {
    if(n < 0) {
      std::rotate(buffer.begin(), buffer.begin() - n, buffer.end());
    }
    else {
      std::rotate(buffer.rbegin(), buffer.rbegin() + n, buffer.rend());
    }
  }
};



};// namespace ZAudio::Tools