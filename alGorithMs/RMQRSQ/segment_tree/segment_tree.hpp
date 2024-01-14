template <typename Value, Value neutral_value>
class Sum {
 public:
  using Result = Value;
  Value GetNeutral() {return neutral_value;}
  Value operator () (const Value& first, const Value& second) { return first + second; }
 private:
  const static Value neutral_value_ = neutral_value;
};

template <typename Value>
class IncreaseIn {
 public:
  Value operator () (const Value& first, const Value& delta, const size_t& amount = 1) { return first * delta; }
  Value neutral = 1;
};

template <typename Value>
class IncreaseOn {
 public:
  Value operator () (const Value& first, const Value& delta) { return first + delta; }
  Value neutral = 0;
};

template <typename Value>
class IncreaseOnInconsistency {
 public:
  Value operator () (const Value& first, const Value& delta, const size_t& amount) { return first + amount * delta; }
  Value neutral = 0;
};


template <typename Value, typename Operation,
          typename ChangingOperation = IncreaseIn<Value>,
          typename InconsistencyChangingOperation = ChangingOperation>
class SegmentTree {
 public:
  SegmentTree (const std::vector<Value>& elements) : elements_amount_(elements.size()) {
    int64_t aimed_size = GetNearestPowOf2(elements.size());
    tree_.resize(aimed_size * 2 - 1, operation_.GetNeutral());
    left_broad_.resize(tree_.size());
    right_broad_.resize(tree_.size());
    inconsistency_.resize(tree_.size(), change_inconsistency_on_.neutral);
    for (size_t i = 0; i < elements_amount_; ++i) {
      tree_[aimed_size - 1 + i] = elements[i];
    }
    for (size_t i = 0; i < aimed_size; ++i) {
      left_broad_[aimed_size - 1 + i] = i;
      right_broad_[aimed_size - 1 + i] = i + 1;
    }
    for (int i = aimed_size - 2; i >= 0; --i) {
      tree_[i] = operation_(tree_[GetLeftSon(i)], tree_[GetRightSon(i)]);
      left_broad_[i] = left_broad_[GetLeftSon(i)];
      right_broad_[i] = right_broad_[GetRightSon(i)];
    }
  }
  typename Operation::Result GetResultOn(size_t begin, size_t end, size_t current_node = 0) {
    size_t left = left_broad_[current_node];
    size_t right = right_broad_[current_node];
    if (right <= begin || left >= end) {
      return operation_.GetNeutral();
    }
    if (left >= begin && right <= end) {
      tree_[current_node] = change_value_on_(tree_[current_node], inconsistency_[current_node]);
      inconsistency_[current_node] = change_inconsistency_on_.neutral;
      return tree_[current_node];
    }
    if (IsLeaf(current_node)) {
      return tree_[current_node];
    }
    inconsistency_[GetLeftSon(current_node)] =
        change_inconsistency_on_(inconsistency_[GetLeftSon(current_node)],
                        inconsistency_[current_node]);
    inconsistency_[GetRightSon(current_node)] =
        change_inconsistency_on_(inconsistency_[GetRightSon(current_node)],
                        inconsistency_[current_node]);
    return operation_(GetResultOn(begin, end, GetLeftSon(current_node)),
                      GetResultOn(begin, end, GetRightSon(current_node)));
  }
  void Print() {
    for (size_t i = 0; i < tree_.size(); ++i) {
      std::cout << tree_[i] << " ";
    }
    std::cout << '\n';
    for (size_t i = 0; i < tree_.size(); ++i) {
      std::cout << left_broad_[i] << " ";
    }
    std::cout << '\n';
    for (size_t i = 0; i < tree_.size(); ++i) {
      std::cout << right_broad_[i] << " ";
    }
  }
  void ChangeOnBeam(size_t begin, size_t end, Value delta) {
    ChangeFromNode(0, begin, end, delta);
  }
 private:
  size_t elements_amount_;
  Operation operation_;
  std::vector<Value> tree_;
  std::vector<size_t> left_broad_;
  std::vector<size_t> right_broad_;
  std::vector<size_t> inconsistency_;
  InconsistencyChangingOperation change_inconsistency_on_;
  ChangingOperation change_value_on_;
  void ChangeFromNode(size_t current_node, size_t begin, size_t end, Value delta) {
    size_t left = left_broad_[current_node];
    size_t right = right_broad_[current_node];
    if (right <= begin || left > end) {
      return;
    }
    if (left >= begin && right <= end) {
      inconsistency_[current_node] =
          change_inconsistency_on_(inconsistency_[current_node], delta);
      return;
    }
    if (IsLeaf(current_node)) {
      tree_[current_node] = change_value_on_(tree_[current_node],
                                             inconsistency_[current_node]);
      inconsistency_[current_node] = change_inconsistency_on_.neutral;
      return;
    }
    ChangeFromNode(GetLeftSon(current_node), begin, end, delta);
    ChangeFromNode(GetRightSon(current_node), begin, end, delta);
    tree_[current_node] =
        operation_(change_value_on_(tree_[GetLeftSon(current_node)],
                                 inconsistency_[GetLeftSon(current_node)]),
                   change_value_on_(tree_[GetRightSon(current_node)],
                                 inconsistency_[GetRightSon(current_node)]));
    inconsistency_[current_node] = change_inconsistency_on_.neutral;
  }
  bool IsLeaf(size_t current_node) {
    return left_broad_[current_node] == current_node - tree_.size() / 2;
  }
  size_t GetLeftSon(const size_t& cur_node) const {
    return 2 * cur_node + 1;
  }
  size_t GetRightSon(const size_t& cur_node) const {
    return 2 * cur_node + 2;
  }
  size_t GetNearestPowOf2(size_t value) const {
    size_t start = 1;
    while (value > start) {
      start *= 2;
    }
    return start;
  }
};
