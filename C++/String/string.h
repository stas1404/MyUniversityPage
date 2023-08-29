#include <cstring>
#include <ostream>
#include <istream>
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
class String {
 public:
  size_t length() const {
    return length_ - 1;
  }
  size_t size() const {
    return length();
  }
  size_t capacity() const {
    return size_ - 1;
  }
  bool empty() {
    return length() == 0;
  }
  String() : length_(1), size_(1) {
    letters_ = new char[1];
    letters_[0] = '\0';
  }
  String(const char* other) {
    length_ = strlen(other) + 1;
    EncreaseSize(0);
    std::copy(other, other + length(), letters_);
    letters_[length_ - 1] = '\0';
  }
  String(size_t number, char symbol) {
    length_ = number + 1;
    EncreaseSize(0);
    std::fill(letters_, letters_ + number, symbol);
    letters_[number] = '\0';
  }
  String(char symbol) {
    *this = String(1, symbol);
  }
  String(const String& existing) {
    length_ = existing.length_;
    EncreaseSize(0);
    std::copy(existing.letters_, existing.letters_ + length_, letters_);
  }
  size_t find(const String& patern) const {
    std::vector<size_t> ans = FindEntries<String>(*this, patern);
    if (!ans.empty()) {
      return ans[0];
    }
    return length();
  }
  size_t rfind(const String& patern) const {
    std::vector<size_t> ans = FindEntries<String>(*this, patern);
    if (!ans.empty()) {
      return ans[ans.size() - 1];
    }
    return length();
  }
  String& operator = (const String& existng) {
    if (&existng == this) {
      return *this;
    }
    length_ = existng.length_;
    EncreaseSize(0);
    std::copy(existng.letters_, existng.letters_ + length_, letters_);
    return *this;
  }
  String substr(size_t start, size_t count) const {
    String substring(count, '\0');
    std::copy(letters_ + start, letters_ + start + count, substring.letters_);
    return substring;
  }
  String substr(size_t start) const {
    return substr(start, length_ - start);
  }
  char& operator [] (size_t pos) {
    return letters_[pos];
  }
  const char& operator [] (size_t pos) const {
    return letters_[pos];
  }
  void shrink_to_fit() {
    size_ = length_;
    EncreaseSize(0, true);
  }
  void pop_back() {
    --length_;
    letters_[length_ - 1] = '\0';
  }
  void push_back(const char& symbol) {
    EncreaseSize(1);
    ++length_;
    letters_[length_ - 2] = symbol;
    letters_[length_ - 1] = '\0';
  }
  void clear() {
    length_ = 1;
    size_ = length_;
    letters_[0] = '\0';
  }
  String& operator += (const String& right) {
    EncreaseSize(right.length_);
    std::copy(right.letters_, right.letters_ + right.length_,letters_ + length());
    length_ += right.length();
    return *this;
  }
  char& front() {
    return this->operator[](0);
  }
  const char& front() const {
    return this->operator[](0);
  }
  char& back() {
    return this->operator[](this->length() - 1);
  }
  const char& back() const {
    return this->operator[](this->length() - 1);
  }
  char* data() {
    return &letters_[0];
  }
  const char* data() const {
    return &letters_[0];
  }
  ~String() {
    delete[] letters_;
  }
 private:
  size_t length_ = 1;
  size_t size_ = 1;
  char* letters_ = nullptr;
  void EncreaseSize(size_t delta, bool shrink_to_fit = false) {
    if (length_ + delta <= size_ && !shrink_to_fit) {
      return;
    }
    while (length_ + delta > size_) {
      size_ *= 2;
    }
    char* newletters = new char [size_];
    if (letters_ != nullptr) {
      std::copy(letters_, letters_ + length_, newletters);
      delete[] letters_;
    }
    letters_ = newletters;
  }
  friend std::ostream& operator << (std::ostream &os, const String &string);
  friend std::istream& operator >> (std::istream& in, String& string);
  friend bool operator == (const String& left, const String& right);
  friend bool operator < (const String& left, const String& right);
};
std::ostream& operator << (std::ostream &os, const String &string) {
  for (size_t i = 0; i < string.length(); ++i) {
    os << string.letters_[i];
  }
  return os;
}
std::istream& operator >> (std::istream& in, String& string) {
  char* bufer = new char[255];
  in.getline(bufer, 255);
  string = String(bufer);
  delete[] bufer;
  return in;
}
bool operator == (const String& left, const String& right) {
  if (left.length_ != right.length_) {
    return false;
  }
  for (size_t i = 0; i < right.length_; ++i) {
    if (left.letters_[i] != right.letters_[i]) {
      return false;
    }
  }
  return true;
}
bool operator != (const String& left, const String& right) {
  return !(left == right);
}
bool operator < (const String& left, const String& right) {
  bool is_left_shorter = false;
  size_t minleng = right.length_;
  if (left.length_ < right.length_) {
    minleng = left.length_;
    is_left_shorter = true;
  }
  size_t maxleng = left.length_ + right.length_ - minleng - 1;
  for (size_t i = 0; i < maxleng; ++i) {
    if (i == minleng) {
      return is_left_shorter;
    }
    if (left.letters_[i] < right.letters_[i]) {
      return true;
    }
    if (left.letters_[i] > right.letters_[i]) {
      return false;
    }
  }
  return false;
}
bool operator >= (const String& left, const String& right) {
  return !(left < right);
}
bool operator > (const String& left, const String& right) {
  return  left >= right && left != right;
}
bool operator <= (const String& left, const String& right) {
  return !(left > right);
}
String& operator + (String left, const String& right) {
  return left.operator+=(right);
}
