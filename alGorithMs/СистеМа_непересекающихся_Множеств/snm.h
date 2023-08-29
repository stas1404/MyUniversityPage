#include <iostream>
#include <unordered_map>
template <typename Child = int>
class SystemIntersectedSet {
 public:
  SystemIntersectedSet(size_t number)
      : pred_(new int[number]),
        weighteight_(new int[number]),
        children_(new int[number]) {
    for (size_t i = 0; i < number; ++i) {
      pred_[i] = i;
      children_[i] = 0;
      weighteight_[i] = 0;
    }
  }
  void GetPred(Child child) { GetPredPrive(GetValPrive(child)); }
  int GetPredWeight(Child child) {
    return GetPredWeigthPrive(GetValPrive(child));
  }
  void Unite(Child fir, Child sec, int weight) {
    size_t firind = GetValPrive(fir);
    size_t secind = GetValPrive(sec);
    UnitePrive(firind, secind, weight);
  }
  ~SystemIntersectedSet() {
    delete[] pred_;
    delete[] weighteight_;
    delete[] children_;
  }

 private:
  std::unordered_map<Child, size_t> ind_;
  int numbforadd_ = 0;
  int* pred_;
  int* weighteight_;
  int* children_;
  int GetPredPrive(int ver) {
    if (pred_[ver] != ver) {
      ver = GetPredPrive(pred_[ver]);
    }
    return ver;
  }
  size_t GetValPrive(Child elem) {
    auto iter = ind_.find(elem);
    if (iter == ind_.end()) {
      ind_[elem] = numbforadd_;
      ++numbforadd_;
    }
    // std::cout << "M" << ind[elem] << '\n';
    return ind_[elem];
  }
  int GetPredWeigthPrive(int ver) { return weighteight_[GetPredPrive(ver)]; }
  void UnitePrive(int fir, int sec, int weighteight) {
    fir = GetPredPrive(fir);
    sec = GetPredPrive(sec);
    if (fir != sec) {
      Swapping(fir, sec);
      pred_[sec] = fir;
      weighteight_[fir] += weighteight_[sec];
    }
    weighteight_[fir] += weighteight;
  }
  void Swapping(int& fir, int& sec);
};
template <typename T>
void SystemIntersectedSet<T>::Swapping(int& fir, int& sec) {
  if (children_[fir] < children_[sec]) {
    std::swap(fir, sec);
  }
  if (children_[fir] == children_[sec]) {
    ++children_[fir];
  }
}
