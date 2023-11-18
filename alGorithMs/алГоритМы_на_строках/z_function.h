#include <iostream>
#include <vector>
template <typename String>
std::vector<size_t> zFunction(const String& string) {
  size_t left = 0;
  size_t right = 0;
  size_t i = 1;
  std::vector<size_t> z_func(string.size());
  z_func[0] = 0;
  while (i != string.size()) {
    if (i > right) {
      size_t j = 0;
      while (i + j < string.size() && string[j] == string[i + j]) {
        ++j;
      }
      left = i;
      right = left + j - 1;
      z_func[i] = j;
    } else {
      if (z_func[i - left] > right - i) {
        size_t j = right - i;
        while (i + j < string.size() && string[j] == string[i + j]) {
          ++j;
        }
        z_func[i] = j;
      } else {
        z_func[i] = z_func[i - left];
      }
    }
    ++i;
  }
  return z_func;
}
