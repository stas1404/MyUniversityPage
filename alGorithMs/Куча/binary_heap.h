#include <iostream>
template <typename U, typename Comparator, typename Allocator = std::allocator<U>>
class Heap {
 public:
  Heap() : allocator_(), buffer_size(1), current_size(0), elements(allocator_.allocate(1)) {};
  Heap(U* elems, size_t amount) : allocator_(), current_size(amount) {
    buffer_size = 1;
    while (buffer_size < amount) {
      buffer_size *= 2;
    }
    elements = allocator_.allocate(buffer_size);
    for (int i = amount - 1; i >= 0; --i) {
      allocator_.template construct(elements + i, elems[i]);
      SiftDown(i);
    }
  }
  U MostlyUp () const {
    std::cout << buffer_size << " " << current_size << '\n';
    return elements[0];
  }
  U ExtractMin() {
    U result = elements[0];
    Swap(current_size - 1, 0);
    --current_size;
    SiftDown(0);
    return result;
  }
  void Insert(const U& value) {
    if (current_size + 1 == buffer_size) {
      U* new_elements = allocator_.allocate(buffer_size * 2);
      for (int i = 0; i < current_size; ++i) {
        allocator_.template construct(new_elements + i, std::move(elements[i]));
      }
      allocator_.deallocate(elements, buffer_size);
      elements = new_elements;
      buffer_size *= 2;
    }
    allocator_.template construct(elements + current_size, std::move(value));
    SiftUp(current_size);
    ++current_size;
  }
  ~Heap() {
    for (int i = 0; i < buffer_size; ++i) {
      allocator_.template destroy(elements + i);
    }
    allocator_.deallocate(elements, buffer_size);
  }
 protected:
  Allocator allocator_;
 private:
  void SiftUp(size_t number) const {
    size_t par;
    try {
      par = GetParentsNumber(number);
    } catch (std::out_of_range& expr) {
      if (expr.what() == std::string("has no parent")) {
        return;
      } else {
        throw;
      }
    }
   if (is_upper_(elements[par], elements[number])) {
     return;
   } else {
     Swap(par, number);
     SiftUp(par);
   }
  }
  void SiftDown(size_t number) const {
    size_t left, right;
    try {
      auto pair = GetChildrenNumbers(number);
      left = pair.first;
      right = pair.second;
    } catch (std::out_of_range& expr) {
      if (expr.what() == std::string("has no children")) {
        return;
      } else {
        throw;
      }
    }
    size_t upper_children;
    if (right == 0) {
      upper_children = left;
    } else {
      if (is_upper_(elements[left], elements[right])) {
        upper_children = left;
      } else {
        upper_children = right;
      }
    }
    if (is_upper_(elements[number], elements[upper_children])) {
      return;
    } else {
      Swap(upper_children, number);
      SiftDown(upper_children);
    }
  }
  void Swap (size_t first, size_t second) const {
    U tmp(std::move(elements[second]));
    elements[second] = std::move(elements[first]);
    elements[first] = std::move(tmp);
  }
  size_t GetParentsNumber(size_t number) const {
    if (number == 0) {
      throw std::out_of_range("has no parent");
    }
    return (number - 1) / 2;
  }
  std::pair<size_t, size_t> GetChildrenNumbers (size_t number) const {
    if (2 * number + 1 >= current_size) {
      throw std::out_of_range("has no children");
    }
    if (2 * number + 2 >= current_size) {
      return {2 * number + 1, 0};
    }
    return {2 * number + 1, 2 * number + 2};
  }

  U* elements;
  size_t current_size;
  size_t buffer_size;
  Comparator is_upper_;
};

template <typename U>
using MinimalHeap = Heap<U, std::less<U> >;

template <typename U>
using MaximalHeap = Heap<U, std::greater<U>>;
