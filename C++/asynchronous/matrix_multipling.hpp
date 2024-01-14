#include <iostream>
#include <atomic>
#include <vector>
#include <thread>
#include <cassert>
template <typename Value>
class Matrix {
 public:
  Matrix(size_t rows, size_t columns) : rows_(rows), columns_(columns) {
    matrix_ = new Value*[rows];
    for (size_t i = 0; i < rows; ++i) {
      matrix_[i] = new Value[columns];
    }
  }
  size_t rows_;
  size_t columns_;
  Value ** matrix_;
};
template <typename Value>
Matrix<Value> operator * (const Matrix<Value>& left, const Matrix<Value>& right) {
  assert(left.columns_ == right.rows_);
  std::mutex write;
  Matrix<Value> ans(left.rows_, right.columns_);
  std::vector<std::jthread> procces(left.rows_ * right.columns_);
  for (size_t i = 0; i < left.rows_; ++i) {
    for (size_t j = 0; j < right.columns_; ++j) {
      procces.template emplace_back([&write, &left, &right, &ans, i, j](){
        Value val = 0;
        for (size_t k = 0; k < left.columns_; ++k) {
            val += left.matrix_[i][k] * right.matrix_[k][j];
        }
        std::lock_guard block(write);
        ans.matrix_[i][j] = val;
      });
    }
  }
  return ans;

}
