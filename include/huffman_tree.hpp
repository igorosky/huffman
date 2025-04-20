#pragma once

#include <optional>
#include <unordered_map>
#include <variant>
#include <vector>
#include "types_def.hpp"

namespace huffman {
template<HUFFMAN_TYPE T>
class HuffmanCoder;
template<HUFFMAN_TYPE T>
class HuffmanTreeBuilder;
template<HUFFMAN_TYPE T>
class HuffmanTree {
  std::vector<std::variant<std::pair<size_t, size_t>, T>> _tree;
  std::unordered_map<T, std::vector<bool>> _codes;

  friend HuffmanTreeBuilder<T>;
  friend HuffmanCoder<T>;

  explicit HuffmanTree(decltype(_tree), decltype(_codes));

public:
  HuffmanTree() = delete;
  std::optional<T> get_value(const std::vector<bool> &) const;
  std::optional<std::reference_wrapper<const std::vector<bool>>> get_code(T) const;
};
}  // namespace huffman


// Implementation

template<HUFFMAN_TYPE T>
huffman::HuffmanTree<T>::HuffmanTree(decltype(_tree) tree, decltype(_codes) codes)
: _tree(std::move(tree)),
  _codes(std::move(codes)) { }

template<HUFFMAN_TYPE T>
std::optional<T> huffman::HuffmanTree<T>::get_value(const std::vector<bool>& code) const {
  size_t p = 0;
  if (_tree.empty()) {
    return std::nullopt;
  }
  for (auto v : code) {
    if (std::holds_alternative<T>(_tree[p])) {
      return std::nullopt;
    }
    auto [left, right] = std::get<std::pair<size_t, size_t>>(_tree[p]);
    p = v ? right : left;
  }
  if (auto ans = std::get_if<T>(&_tree[p])) {
    return *ans;
  }
  return std::nullopt;
}

template<HUFFMAN_TYPE T>
std::optional<std::reference_wrapper<const std::vector<bool>>> huffman::HuffmanTree<T>::get_code(T c) const {
  auto ans = _codes.find(c);
  return ans != _codes.end() ? std::make_optional(std::cref(ans->second)) : std::nullopt;
}
