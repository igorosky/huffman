#pragma once

#include <optional>
#include <utility>
#include <vector>
#include "huffman_tree.hpp"
#include "types_def.hpp"

namespace huffman {
template<HUFFMAN_TYPE T>
class HuffmanCoder {
  HuffmanTree<T> _huffman_tree;

public:
  explicit HuffmanCoder(HuffmanTree<T>);

  template<typename I>
  ITER_REQUIREMENT(I, T)
  std::optional<std::vector<char>> encode(I, I) const;
  std::optional<std::vector<T>> decode(const std::vector<char>& data, size_t len) const;
};
}  // namespace huffman


// Implementation

template<HUFFMAN_TYPE T>
huffman::HuffmanCoder<T>::HuffmanCoder(HuffmanTree<T> huffman_tree)
: _huffman_tree(std::move(huffman_tree)) { }

template<HUFFMAN_TYPE T>
template<typename I>
ITER_REQUIREMENT(I, T)
std::optional<std::vector<char>> huffman::HuffmanCoder<T>::encode(I begin, I end) const {
  size_t p = 0;
  std::vector<T> ans;
  if constexpr (HAS_MINUS(end, begin) && std::is_convertible_v<decltype(end - begin), size_t>) {
    ans.reserve(static_cast<size_t>(end - begin));
  }
  for (; begin != end; ++begin) {
    auto coded = _huffman_tree.get_code(*begin);
    if (!coded.has_value()) {
      return std::nullopt;
    }
    for (bool v : coded.value().get()) {
      if (p / 8 >= ans.size()) {
        ans.push_back(0);
      }
      if (v) {
        ans.back() |= static_cast<char>(0x80 >> static_cast<int>(p % 8));
      }
      ++p;
    }
  }
  return ans;
}

template<HUFFMAN_TYPE T>
std::optional<std::vector<T>> huffman::HuffmanCoder<T>::decode(const std::vector<char>& data, size_t len) const {
  size_t p = 0;
  size_t q = 0;
  if (auto c = std::get_if<T>(&_huffman_tree._tree[0])) {
    return std::vector(len, *c);
  }
  std::vector<T> ans;
  ans.reserve(len);
  while (ans.size() < len) {
    auto [left, right] = std::get<std::pair<size_t, size_t>>(_huffman_tree._tree[q]);
    q = ((data[p / 8] & (0x80 >> (p % 8))) != 0) ? right : left;
    if (auto c = std::get_if<T>(&_huffman_tree._tree[q])) {
      ans.push_back(*c);
      q = 0;
    }
    ++p;
  }
  return ans;
}
