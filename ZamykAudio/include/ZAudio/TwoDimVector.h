#pragma once

#include <vector>
#include <span>

namespace ZAudio::Tools {


template<typename T>
class TwoDimVector {
public:
template<typename container, typename pointer, typename reference>
class TCollumnIterator {
public:
  using difference_type = int32_t;
  using value_type = T;    
  TCollumnIterator() = default;
  TCollumnIterator(size_t row_p, size_t collumn_p, container* twoDimVector_p) : twoDimVector(twoDimVector_p), row(row_p), collumn(collumn_p) {}    

  TCollumnIterator& operator += (int32_t v) {    
    *this = *this + v;
    return *this;    
  }

  TCollumnIterator& operator -= (int32_t v) {
    *this = *this - v;
    return *this;
  }

  TCollumnIterator& operator++() {
    return *this += 1;
  }

  TCollumnIterator operator++(int) & {
    auto ans = *this;
    *this += 1;
    return ans;
  }

  TCollumnIterator& operator--() {
    return *this -= 1;
  }

  TCollumnIterator operator--(int) & {    
    auto ans = *this;
    *this -= 1;
    return ans;
  }  

  pointer operator->() const {
    return &twoDimVector->get(row, collumn);
  }

  reference operator*() const {    
    return twoDimVector->get(row, collumn);
  }

  reference operator[] (difference_type ind) const {
    return twoDimVector->get(row + ind, collumn);
  }  

  bool operator == (const TCollumnIterator& oth) const {
    return std::make_tuple(twoDimVector, row, collumn) == std::make_tuple(oth.twoDimVector, oth.row, oth.collumn);
  }

  bool operator != (const TCollumnIterator& oth) const {
    return std::make_tuple(twoDimVector, row, collumn) != std::make_tuple(oth.twoDimVector, oth.row, oth.collumn);
  }

  bool operator < (const TCollumnIterator& oth) const {
    return std::make_tuple(twoDimVector, row, collumn) < std::make_tuple(oth.twoDimVector, oth.row, oth.collumn);
  }

  bool operator <= (const TCollumnIterator& oth) const {
    return std::make_tuple(twoDimVector, row, collumn) <= std::make_tuple(oth.twoDimVector, oth.row, oth.collumn);
  }

  bool operator > (const TCollumnIterator& oth) const {
    return std::make_tuple(twoDimVector, row, collumn) > std::make_tuple(oth.twoDimVector, oth.row, oth.collumn);
  }

  bool operator >= (const TCollumnIterator& oth) const {
    return std::make_tuple(twoDimVector, row, collumn) >= std::make_tuple(oth.twoDimVector, oth.row, oth.collumn);
  }

  difference_type operator - (const TCollumnIterator& it) const {
    return row - it.row;
  }  

  friend TCollumnIterator operator + (TCollumnIterator it, difference_type v) {
    return TCollumnIterator(it.row + v, it.collumn, it.twoDimVector);
  }

  friend TCollumnIterator operator - (TCollumnIterator it, difference_type v) {
    return RowIterator(it.row - v, it.collumn, it.twoDimVector);
  }    

  friend TCollumnIterator operator + (difference_type v, TCollumnIterator it) {
    return it + v;
  }

  friend TCollumnIterator operator - (difference_type v, TCollumnIterator it) {
    return it + v;
  }    

private:
  container* twoDimVector = nullptr;
  size_t row = 0;
  size_t collumn = 0;  
};  
using CollumnIterator = TCollumnIterator<TwoDimVector<T>, T*, T&>;
using CollumnConstIterator = TCollumnIterator<const TwoDimVector<T>, const T*, const T&>;

  TwoDimVector() = default;
  TwoDimVector(size_t rows_p, size_t collumns_p) : rows(rows_p), collumns(collumns_p), vect(rows_p * collumns_p) {}
  TwoDimVector(size_t rows_p, size_t collumns_p, const T& v) : rows(rows_p), collumns(collumns_p), vect(rows_p * collumns_p, v) {}

  void clear() {
    vect.clear();
    rows = 0;
    collumns = 0;
  }

  void resize(size_t rows_p, size_t collumns_p) {
    vect.resize(rows_p * collumns_p);
    rows = rows_p;
    collumns = collumns_p;
  }

  size_t getNumOfRows() const {
    return rows;
  }

  size_t getNumOfCollumns() const {
    return collumns;
  }

  T& get(size_t row, size_t collumn) {
    return vect[collumn + row * collumns];
  }

  const T& get(size_t row, size_t collumn) const {
    return vect[collumn + row * collumns];
  }

  std::span<T> getRow (size_t row) {
    return std::span<T>(vect).subspan(row * collumns, collumns);
  }

  std::span<const T> getRow (size_t row) const {
    return std::span<const T>(vect).subspan(row * collumns, collumns);
  }  

  CollumnIterator getCollumnBegin (size_t collumn) {
    assert(collumn < collumns);
    return CollumnIterator(0, collumn, this);
  }

  CollumnIterator getCollumnEnd (size_t collumn) {
    assert(collumn < collumns);
    return CollumnIterator(rows, collumn, this);
  }

  CollumnConstIterator getCollumnCbegin (size_t collumn) const {
    assert(collumn < collumns);
    return CollumnConstIterator(0, collumn, this);
  }

  CollumnConstIterator getCollumnCend (size_t collumn) const {
    assert(collumn < collumns);
    return CollumnConstIterator(rows, collumn, this);
  }  

private:
  size_t rows = 0;
  size_t collumns = 0;
  std::vector<T> vect;  
};


} // namespace ZAudio