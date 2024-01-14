template <typename type,
          typename Operation,
          typename Allocator = std::allocator<type>,
          typename ExternalAllocator = std::allocator<type*>>
class SparseTable {
 public:
  SparseTable(const std::vector<type>& base, Operation operation) : operation_(operation) {
   CalculateLog(base.size() + 1);
   tablesize_ = base.size();
   max_height_ = log_[tablesize_] + 1;
   layers_ = external_allocator_.allocate(max_height_);
   layers_[0] = allocator_.allocate(tablesize_);
   for (size_t i = 0; i < tablesize_; ++i) {
       layers_[0][i] = base[i];
       std::cout << layers_[0][i] << ' ';
   }
   std::cout << '\n';
   size_t previous_length = tablesize_;
   for (size_t layer_number = 1; layer_number < max_height_; ++layer_number) {
     size_t new_length = previous_length - GetPowOf2(layer_number - 1);
     layers_[layer_number] = allocator_.allocate(new_length);
     for (size_t i = 0; i < new_length; ++i) {
       layers_[layer_number][i] = operation_(layers_[layer_number - 1][i],
                                             layers_[layer_number - 1][i +
                                             GetPowOf2(layer_number - 1)]);
       std::cout << layers_[layer_number][i] << " ";
     }
     std::cout << '\n';
     previous_length = new_length;
   }
  }
  type GetResultOn(size_t begin, size_t end) const {
    size_t layer_number = log_[end - begin];
    return operation_(layers_[layer_number][begin], layers_[layer_number][end - GetPowOf2(layer_number)]);
  }
  ~SparseTable() {
    size_t previous_length = tablesize_;
    allocator_.deallocate(layers_[0], previous_length);
    for (size_t layer_number = 1; layer_number < max_height_; ++layer_number) {
      previous_length -= GetPowOf2(layer_number - 1);
      allocator_.deallocate(layers_[layer_number], previous_length);
    }
    external_allocator_.deallocate(layers_, max_height_);
  }
 protected:
  Allocator allocator_;
  ExternalAllocator external_allocator_;
  Operation operation_;
 private:
  size_t max_height_;
  size_t GetPowOf2(size_t x) const {return 1 << x;}
  void CalculateLog(size_t amount) {
    log_.reserve(amount);
    log_[0] = 0;
    log_[1] = 0;
    for (size_t i = 2; i < amount; ++i) {
      log_[i] = log_[i / 2] + 1;
    }
  }
  std::vector<size_t> log_;
  size_t tablesize_;
  type** layers_;
};
