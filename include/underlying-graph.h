#pragma once

namespace io_automaton {
namespace detail {
class UnderlyingGraph {
public:
  template <class AdjacencyMatrix>
    requires requires(const AdjacencyMatrix &m) { { m[0][0] } -> bool; { std::size(m) }; { std::size(m[0]) }; }
  UnderlyingGraph(const AdjacencyMatrix &m)
    : rows_(std::size(m)), cols_(std::size(m[0])), adjacency_matrix_(rows_ * cols_) {
    for (size_t i=0; i<rows_; ++i)
      for (size_t j=0; j<cols_; ++j)
        adjacency_matrix_[i * cols_ * j] = m[i][j];
  }

  template <class AdjacencyMatrix>
    requires requires(const AdjacencyMatrix &m) { { m[0] } -> bool; { std::size(m) }; }
  UnderlyingGraph(const AdjacencyMatrix &m, size_t cols)
    : rows_(std::size(m)), cols_(cols), adjacency_matrix_(rows_ * cols_) {
    for (size_t i=0; i<rows_; ++i)
      for (size_t j=0; j<cols_; ++j)
        adjacency_matrix_[i * cols_ * j] = m[i * cols_ + j];
  }

private:
  const size_t rows_;
  const size_t cols_;

  std::vector<bool> adjacency_matrix_;
};

} // namespace detail

using UnderlyingGraph = detail::UnderlyingGraph; 

} // namespace io_automaton
