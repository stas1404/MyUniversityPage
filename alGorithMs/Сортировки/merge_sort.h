#include <iostream>
#include <vector>
template <typename Container>
Container Merge(typename Container::iterator first_begin,typename  Container::iterator first_end,
                typename Container::iterator second_begin,typename  Container::iterator second_end) {
  Container result(first_end - first_begin + second_end - second_begin);
  size_t i = 0;
  while (first_begin != first_end && second_begin != second_end) {
    if (*first_begin < *second_begin) {
      result[i] = *first_begin;
      ++first_begin;
    } else {
      result[i] = *second_begin;
      ++second_begin;
    }
    ++i;
  }
  while (first_begin != first_end) {
    result[i] = *first_begin;
    ++first_begin;
    ++i;
  }
  while (second_begin != second_end) {
    result[i] = *second_begin;
    ++second_begin;
    ++i;
  }
  return result;
}
template <typename Container>
Container MergeSort(typename Container::iterator first_begin,typename  Container::iterator first_end) {
  if (first_begin == first_end) {
    return Container();
  }
  if (first_end - first_begin == 1) {
    return Container(1, *first_begin);
  }
  typename Container::iterator mid = first_begin + (first_end - first_begin) / 2;
  Container first = MergeSort<Container>(first_begin, mid);
  Container second = MergeSort<Container>(mid, first_end);
  return Merge<Container>(first.begin(), first.end(),
                          second.begin(), second.end());
}
