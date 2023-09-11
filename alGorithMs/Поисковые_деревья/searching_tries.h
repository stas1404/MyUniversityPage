 #include <iostream>
#include <fstream>
#include <tuple>
#include <vector>
#include <unordered_map>
template <typename Kernel>
struct Node : public Kernel {
  Node* left = nullptr;
  Node* right = nullptr;
  template<typename ...Args>
  explicit Node<Kernel>(const Args&... args) : Kernel(args...) {}
  bool HaveNoChild () {
    if ((left == right) && (right == nullptr)) {
      return true;
    } else {
      return false;
    }
  }
  size_t GetAmountLeft() {
    if (this->left != nullptr) {
      return this->left->tree_size;
    } else {
      return 0;
    }
  }
  size_t GetAmountRight() {
    if (this->right != nullptr) {
      return this->right->tree_size;
    } else {
      return 0;
    }
  }
  void RecounttreeSize() {
    this->tree_size = 1;
    if (this->left != nullptr) {
      this->tree_size += this->left->tree_size;
    }
    if (this->right != nullptr) {
      this->tree_size += this->right->tree_size;
    }
  }
  void RecountResult() {
    if (this->HaveNoChild()) {
      this -> result = this->information;
      return;
    }
    if (this->left == nullptr) {
      this -> result = this->operation(this->information, right->GetResult());
      return;
    }
    if (this->right == nullptr) {
      this -> result = this->operation(this->information, left->GetResult());
      return;
    }
    this -> result = this->operation(left->result, this->information, right->result);
  }
  void RecountResultRecursivly() {
    if (this->HaveNoChild()) {
      this -> result = this->information;
      return;
    }
    if (this->left == nullptr) {
      this->right->RecountResultRecursivly();
      this -> result = this->operation(this->information, right->GetResult());
      return;
    }
    if (this->right == nullptr) {
      this->left->RecountResultRecursivly();
      this -> result = this->operation(this->information, left->GetResult());
      return;
    }
    this->right->RecountResultRecursivly();
    this->left->RecountResultRecursivly();
    this -> result = this->operation(left->result, this->information, right->result);
  }
};
template <typename Value>
struct BaseKernel {
  Value value;
  BaseKernel() {};
  BaseKernel(Value new_value) : value(new_value) {}
  BaseKernel(const BaseKernel& other) : value(other.value) {}
};
template <typename Value, typename Priority = int32_t>
struct DekartKernel : public BaseKernel<Value> {
  Priority priority;
  Priority GenPriority() {
    return rand();
  }
  using BaseKernel<Value>::BaseKernel;
  DekartKernel(Value value) : BaseKernel<Value>(value),
                              priority(GenPriority()) {}
  DekartKernel(const DekartKernel<Value, Priority>& other) : BaseKernel<Value>(other.value),
                                                             priority(other.priority) {}
};
template <typename Value, typename Priority = int32_t>
struct DekartKernelMultiple : public DekartKernel<Value, Priority> {
  using DekartKernel<Value, Priority>::DekartKernel;
  size_t tree_size = 1;
};
template <typename Information>
struct max {
  Information operator() (const Information& left, const Information& right) {
    return std::max(left, right);
  }
  Information operator() (const Information& left, const Information& middle,
                          const Information& right) {
    return std::max(middle, std::max(left, right));
  }
};
template <typename Information>
struct sum {
  Information operator() (const Information& left, const Information& right) {
    return left + right;
  }
  Information operator() (const Information& left, const Information& middle,
                          const Information& right) {
    return left + middle + right;
  }
};
template <typename Information, typename Result, typename Operation = sum<Information>>
struct MegaArrayKernel : public DekartKernelMultiple<size_t> {
  const static Information neutral_delta = 0;
  Information information;
  Information delta = neutral_delta;
  Operation operation;
  MegaArrayKernel() {}
  MegaArrayKernel(const Information& informa) : DekartKernelMultiple<size_t>(),
                                                information(informa), result(informa){}
  MegaArrayKernel(const Information& informa, const size_t& number) : DekartKernelMultiple<size_t>(number),
                                                                      information(informa), result(informa){}
  void ChangeResultAndInformation() {
    information += delta;
    result += delta;
  }
  void MakeDeltaNeutral() {
    delta = neutral_delta;
  }
  void ChangeInformation() {
    information += delta;
  }
  Result GetResult() {
    if (delta != neutral_delta) {
      return result + tree_size * delta;
    }
    return result;
  }
  Result result;
};
template <typename Value>
using BaseNode = Node<BaseKernel<Value>>;

template <typename Value, typename Priority = int32_t>
using DekartNode = Node<DekartKernel<Value, Priority>>;

template <typename Value, typename Priority = int32_t>
using DekartMultipleNode = Node<DekartKernelMultiple<Value, Priority>>;

template <typename Information, typename Result>
using MegaArrayNode = Node<MegaArrayKernel<Information, Result>>;

template <typename Value, typename NodeType = BaseNode<Value>>
class BaseTree {
 public:
  BaseTree() {}
  explicit BaseTree(NodeType* peak) {
    peak_ = peak;
  }
  BaseTree(BaseTree&& other) {
    Delete(peak_);
    peak_ = other.peak_;
    other.peak_ = nullptr;
  }
  BaseTree& operator = (BaseTree&& other) {
    Delete(peak_);
    peak_ = other.peak_;
    other.peak_ = nullptr;
    return *this;
  }
  bool empty() const {
    return peak_ == nullptr;
  }
  void Insert(const Value& value) {
    if (empty()) {
      peak_ = new NodeType(value);
      return;
    }
    NodeType* parent = FindParent(value);
    if (at_left_(value, parent->value)) {
      parent->left = new NodeType(value);
    } else {
      parent->right = new NodeType(value);
    }
  }
  bool ConsistValue(const Value& value) const {
    NodeType *node = Find(value);
    if (node == nullptr) {
      return false;
    }
    return true;
  }
  Value FindMaximalValue() const {
    NodeType* ans = FindMaximal();
    if (ans != nullptr) {
      return ans->value;
    }
    throw std::runtime_error("tree is empty");
  }
  Value FindMinimalValue() const {
    NodeType* ans = FindMinimal();
    if (ans != nullptr) {
      return ans->value;
    }
    throw std::runtime_error("tree is empty");
  }
  void PreOrder() {
    PreOrder_(peak_);
  }
  ~BaseTree() {
    Delete(peak_);
  }
 protected:
  NodeType* peak_ = nullptr;
  std::less<Value> at_left_;
  using Comparator = decltype(at_left_);
  std::allocator<NodeType> allocator_;
  std::tuple<NodeType*, NodeType*, bool> FindParentAnsChild (Value value) const {
    Finder find_prev(value, peak_);
    return std::make_tuple(find_prev.parent, find_prev.current, find_prev.is_left_son);
  }
  NodeType* FindMaximal() const {
    if (peak_ != nullptr) {
      return GetMostlyRightSon(peak_);
    }
    return nullptr;
  }
  NodeType* FindMinimal() const {
    if (peak_ != nullptr) {
      return GetMostlyLeftSon(peak_);
    }
    return nullptr;
  }
  NodeType* GetMostlyRightSon(NodeType* cur) const {
    if (cur->right != nullptr) {
      return GetMostlyRightSon(cur->right);
    }
    return cur;
  }
  NodeType* GetMostlyLeftSon(NodeType* cur) const {
    if (cur->left != nullptr) {
      return GetMostlyLeftSon(cur->left);
    }
    return cur;
  }
  NodeType* FindParent (Value value) const {
    Finder find_prev(value, peak_);
    return find_prev.parent;
  }
  NodeType* Find (Value value) const {
    Finder find_prev(value, peak_);
    return find_prev.current;
  }
  void PreOrder_(NodeType* node) const {
    if (node == nullptr) {
      return;
    }
    std::cout << "M" << node->tree_size << '/' << node->information << "/" << node->result << "/" << node->delta << " ";
    std::cout << "  left_son:  ";
    PreOrder_(node->left);
    std::cout << "\n  right_son:  ";
    PreOrder_(node->right);
  }
  void InOrder_(NodeType* node) const {
    if (node == nullptr) {
      return;
    }
    InOrder_(node->left);
    std::cout << node->value;
  }
  void CutDown() {
    peak_ = nullptr;
  }
  class Finder {
   public:
    NodeType* current = nullptr;
    NodeType* parent = nullptr;
    bool is_left_son = false;
    Finder(Value value, NodeType* peak) {
      current = peak;
      while (current != nullptr) {
        if (current->value == value) {
          return;
        }
        parent = current;
        if (at_left(value, current->value)) {
          current = current->left;
          is_left_son = true;
        } else {
          current = current->right;
          is_left_son = false;
        }
      }
    }
   protected:
    decltype(at_left_) at_left;
  };
 private:
  void Delete(NodeType* node) const {
    if (node == nullptr) {
      return;
    }
    Delete(node->left);
    Delete(node->right);
    delete node;
  }
};
template <typename Value, typename NodeType = DekartMultipleNode<Value>>
class DekartSearchingTree : public BaseTree<Value, NodeType> {
 public:
  using BaseTree<Value, NodeType>::BaseTree;
  using tree_type = DekartSearchingTree<Value, NodeType>;
  using Comparator = typename BaseTree<Value, NodeType>::Comparator;
  std::pair<tree_type, tree_type> Split(const Value& key) {
    if (this->empty()) {
      return {tree_type(nullptr), tree_type(nullptr)};
    }
    tree_type tmp;
    if (this->at_left_(key, this->peak_->value) || (key == this->peak_->value)) {
      tmp = tree_type(this->peak_->left);
    } else {
      tmp = tree_type(this->peak_->right);
    }
    auto [left, right] = tmp.Split(key);
    if (this->at_left_(key, this->peak_->value) || (key == this->peak_->value)) {
      this->peak_->left = right.peak_;
      this->peak_->RecounttreeSize();
      right.CutDown();
      return {std::move(left), std::move(*this)};
    } else {
      this->peak_->right = left.peak_;
      this->peak_->RecounttreeSize();
      left.CutDown();
      return {std::move(*this), std::move(right)};
    }
  }
  void InsertIfNotConsist(const Value& value) {
    if (this->ConsistValue(value)) {
      return;
    }
    Insert(value);
  }
  void Insert(const Value& value) {
    auto [left, right] = std::move(this->Split(value));
    NodeType* new_node = new NodeType(value);
    *this = tree_type(std::move(left), tree_type(new_node));
    *this = tree_type(std::move(*this), std::move(right));
  }
  void Remove(const Value& value) {
    if (this->empty()) {
      return;
    }
    auto [par, ch, is_left] = this->FindParentAnsChild(value);
    if (ch == nullptr) {
      return;
    }
    tree_type new_tree(tree_type(ch->left), tree_type(ch->right));
    if (par == nullptr) { // It's mean we should remove peak_
      delete this->peak_;
      this->peak_ = new_tree.peak_;
      new_tree.CutDown();
      return;
    }
    if (is_left) {
      par->left = new_tree.peak_;
    } else {
      par->right = new_tree.peak_;
    }
    delete ch;
    new_tree.CutDown();
    auto [left, right] = this->Split(value);
    *this = tree_type(std::move(left), std::move(right));
  }
  DekartSearchingTree(tree_type&& left, tree_type&& right) { // Analog of Merge operation
    if (left.empty()) {
      *this = std::move(right);
      return;
    }
    if (right.empty()) {
      *this = std::move(left);
      return;
    }
    if (left.peak_->priority > right.peak_->priority) {
      tree_type tmp(tree_type(left.peak_->right), std::move(right));
      left.peak_->right = tmp.peak_;
      tmp.CutDown();
      *this = std::move(left);
    } else {
      tree_type tmp(std::move(left), tree_type(right.peak_->left));
      right.peak_->left = tmp.peak_;
      tmp.CutDown();
      *this = std::move(right);
    }
    left.CutDown();
    right.CutDown();
    this->peak_->RecounttreeSize();
  }
  Value FindPreviousFor(const Value& value) {
    auto [left, right] = std::move(this->Split(value));
    if (left.empty()) {
      *this = tree_type(std::move(left), std::move(right));
      throw std::out_of_range("don't consist element less then given");
    }
    Value ans = left.FindMaximalValue();
    *this = tree_type(std::move(left), std::move(right));
    return ans;
  }
  Value FindNextAfter(const Value& value) {
    auto [left, right] = std::move(this->Split(value));
    if (right.empty()) {
      *this = tree_type(std::move(left), std::move(right));
      throw std::out_of_range("don't consist element more then given");
    }
    auto [par, ch, is_left] = right.FindParentAnsChild(value);
    *this = tree_type(std::move(left), std::move(right));
    if (ch == nullptr) {
      return par->value;
    }
    if (ch->value == value) {
      return par->value;
    }
    return ch->value;
  }
  void FindOrderStatisticsNumber(size_t number) {
    if (this->empty()) {
      throw std::out_of_range("tree is empty");
    }
    if (this->peak_->tree_size <= number) {
      throw std::out_of_range("tree doesn't consist " + std::to_string(number) + "th element");
    }
    FindOrderStatisticsNumber_(this->peak_, number);
  }
 protected:

  void FindOrderStatisticsNumber_(NodeType* node, size_t& number) {
    if (node->GetAmountLeft() > number) {
      FindOrderStatisticsNumber_(node->left, number);
      return;
    }
    if (node->GetAmountLeft() == number) {
      std::cout << node->value << '\n';
      return;
    }
    number -= (1 + node->GetAmountLeft());
    FindOrderStatisticsNumber_(node->right, number);
  }
};
template <typename NodeType>
size_t RecountChildAmountRecursivly(NodeType* peak) {
  if (peak == nullptr) {
    return 0;
  }
  peak->tree_size = RecountChildAmountRecursivly(peak->left) +
      RecountChildAmountRecursivly(peak->right) + 1;
  return peak->tree_size;
}
template <typename Information, typename Result = Information>
class MegaArray : public DekartSearchingTree<short, MegaArrayNode<Information, Result>> {
 public:
  using DekartSearchingTree<short, MegaArrayNode<Information, Result>>::DekartSearchingTree;
  using tree_type = MegaArray<Information, Result>;
  using NodeType = MegaArrayNode<Information, Result>;
  MegaArray(const std::vector<Information>& nodes) {
    srand(time(0));
    this->peak_ = new NodeType(nodes[0], value_for_insert);
    ++value_for_insert;
    NodeType* last_added = this->peak_;
    std::unordered_map<NodeType*, NodeType*> parents;
    parents[last_added] = nullptr;
    for (size_t i = 1; i < nodes.size(); ++i) {
      NodeType* cur = last_added;
      NodeType* new_node = new NodeType(nodes[i], value_for_insert);
      ++value_for_insert;
      while (cur != nullptr) {
        if (cur->priority > new_node->priority) {
          new_node->left = cur->right;
          cur->right = new_node;
          break;
        } else {
          cur = parents[cur];
        }
      }
      if (cur == nullptr) {
        new_node->left = this->peak_;
        this->peak_ = new_node;
      }
      last_added = new_node;
    }
    RecountChildAmountRecursivly(this->peak_);
    this->peak_->RecountResultRecursivly();
  }
  std::pair<tree_type, tree_type> Split(size_t key) {
    if (this->empty()) {
      return {tree_type(nullptr), tree_type(nullptr)};
    }
    //std::cout << "FROM SPLIT       ";
    //std::cout << this->peak_->GetAmountLeft() << '\n';
    tree_type tmp;
    this->peak_->ChangeInformation();
    this->throwDeltaToChildren();
    if (this->at_left_(key, this->peak_->GetAmountLeft()) || (key == this->peak_->GetAmountLeft())) {
      //std::cout << "FIRST     " << this->peak_->information << '\n';
      tmp = tree_type(this->peak_->left);
      auto [left, right] = tmp.Split(key);
      this->peak_->MakeDeltaNeutral();
      this->peak_->left = right.peak_;
      this->peak_->RecounttreeSize();
      this->peak_->RecountResult();
      right.CutDown();
      return {std::move(left), std::move(*this)};
    } else {
      //std::cout << "SECONd     " << this->peak_->information << '\n';
      tmp = tree_type(this->peak_->right);
      auto [left, right] = tmp.Split(key -= (this->peak_->GetAmountLeft() + 1));
      this->peak_->MakeDeltaNeutral();
      this->peak_->right = left.peak_;
      this->peak_->RecounttreeSize();
      this->peak_->RecountResult();
      left.CutDown();
      return {std::move(*this), std::move(right)};
    }
  }
  Information operator[] (size_t number) {
    return GetResultOnBeam(number, number + 1);
  }
  MegaArray(tree_type&& left, tree_type&& right) { // Analog of Merge operation
    if (left.empty()) {
      *this = std::move(right);
      return;
    }
    if (right.empty()) {
      *this = std::move(left);
      return;
    }
    if (left.peak_->priority > right.peak_->priority) {
      left.throwDeltaToChildren();
      left.peak_->ChangeInformation();
      left.peak_->MakeDeltaNeutral();
      tree_type tmp(tree_type(left.peak_->right), std::move(right));
      left.peak_->right = tmp.peak_;
      tmp.CutDown();
      left.peak_->RecountResult();
      *this = std::move(left);
    } else {
      right.throwDeltaToChildren();
      right.peak_->ChangeInformation();
      right.peak_->MakeDeltaNeutral();
      tree_type tmp(std::move(left), tree_type(right.peak_->left));
      right.peak_->left = tmp.peak_;
      tmp.CutDown();
      right.peak_->RecountResult();
      *this = std::move(right);
    }
    left.CutDown();
    right.CutDown();
    this->peak_->RecounttreeSize();
    this->peak_->RecountResult();
  }
  void Insert(const Information& value) {
    size_t ins_value = value_for_insert;
    auto [left, right] = std::move(this->Split(value_for_insert));
    NodeType* new_node = new NodeType(value, ins_value);
    *this = tree_type(std::move(left), tree_type(new_node));
    *this = tree_type(std::move(*this), std::move(right));
    //this->PreOrder();
    value_for_insert = ins_value + 1;
  }
  Result GetResultOnBeam(size_t start, size_t end) {
    if (start == end) {
      throw std::out_of_range("you should pass [start, end)");
    }
    auto [left_remain, right] = this->Split(start);
    //left_remain.PreOrder();
    //right.PreOrder();
    auto [target, right_remain] = right.Split(end - start);
    //target.PreOrder();
    //right_remain.PreOrder();
    Result ans = target.peak_->GetResult();
    target = tree_type(std::move(target), std::move(right_remain));
    *this = tree_type(std::move(left_remain), std::move(target));
    return ans;
  }
  void ChangeBeamOnDelta(size_t start, size_t end, const Information& delta) {
    if (start == end) {
      throw std::out_of_range("you should pass a set like [start, end)");
    }
    auto [left_remain, right] = this->Split(start);
    auto [target, right_remain] = right.Split(end - start);
    target.peak_->delta = delta;
    target = tree_type(std::move(target), std::move(right_remain));
    *this = tree_type(std::move(left_remain), std::move(target));
  }
 protected:
  void throwDeltaToChildren() {
    if (this->empty()) {
      return;
    }
    if (this->peak_->delta == NodeType::neutral_delta) {
      return;
    }
    if (this->peak_->left != nullptr) {
      this->peak_->left->delta += this->peak_->delta;
    }
    if (this->peak_->right != nullptr) {
      this->peak_->right->delta += this->peak_->delta;
    }
  }
  size_t value_for_insert = 0;
};
