#include <iostream>
#include <vector>
#include <tgmath.h>
#include <fstream>
template <typename U, typename Pred, typename iter>
iter Partition(Pred pred, const iter left, const iter right) {
  iter first = left;
  iter second = right;
  if (first == second) {
    return first;
  }
  --second;
  while (first <= second) {
    while (pred(first)) {
      ++first;
    }
    while (!pred(second) && (!pred.equal(second))) {
      --second;
    }
    if (first > second) {
      break;
    }
    U x = *first;
    *first = *second;
    *second = x;
    ++first;
    --second;
  }
  return first;
}
template <typename U>
class Predicate {
 public:
  Predicate(U x) {
    value_ = x;
  }
  bool operator() (U other) {
    return other < value_;
  }
  template<typename iter>
  bool equal(iter other) {
    return *other == value_;
  }
  template<typename iter>
  bool operator() (iter other) {
    return *other < value_;
  }
 private:
  U value_;
};
template <typename U, typename iter>
void QuickSort(const iter first, const iter second) {
  if (second == first || second == first + 1) {
    return;
  }
  if (second - first == 2) {
    if (*(second - 1) < *(first)) {
      U x = *first;
      *first = *(second - 1);
      *(second - 1) = x;
    }
    return;
  }
  Predicate<U> pred(int(*(first + std::rand() % (second - first))));
  const iter mid = Partition<U>(pred, first, second);
  if (mid > first + 1) {
    QuickSort<U>(first, mid);
  }
  if (second > mid + 1) {
    QuickSort<U>(mid, second);
  }
}
