#include <iostream>
#include <vector>
template <typename String>
class ImagingString {
 public:
  ImagingString(const String& pattern, const String& text) : text(text), pattern(pattern) {
  }
  char operator[](size_t i) const {
    if (i < pattern.length()) {
      return pattern[i];
    }
    if (i > pattern.length()) {
      return text[i - pattern.length() - 1];
    }
    return '\0';
  }
  size_t length() const {
    return pattern.length() + text.length() + 1;
  }
  size_t size() const {
    return length();
  }
 private:
  const String& text;
  const String& pattern;
};
template <typename String>
std::vector<size_t> CountingPrefficsFunction(const String& mystring) {
  std::vector<size_t> pref_func(mystring.size());
  pref_func[0] = 0;
  for (size_t i = 1; i < mystring.size(); ++i) {
    //std::cout <<  "P" << mystring[i]; // << '\n';
    pref_func[i] = 0;
    size_t k = pref_func[i - 1];
    while (k > 0 && mystring[i] != mystring[k]) {
      k = pref_func[k - 1];
    }
    if (mystring[i] == mystring[k]) {
      ++k;
    }
    pref_func[i] = k;
  }
  return pref_func;
}
template <typename String>
std::vector<size_t> FindEntries(const String& pattern, const String& text) {
  std::vector<size_t> ans;
  ImagingString<String> mystring(pattern, text);
  std::vector<size_t> pref_func = CountingPrefficsFunction(mystring);
  for (size_t i = 0; i < pref_func.size(); ++i) {
    if (pref_func[i] == pattern.size()) {
      ans.push_back(i - pattern.size() * 2);
    }
  }
  return ans;
}