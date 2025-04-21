#pragma once

#include <utility>
#include <vector>
#include "huffman_tree.hpp"
#include "types_def.hpp"

namespace huffman {
template<HUFFMAN_TYPE T>
class HuffmanEncoder {
  HuffmanTree<T> _huffman_tree;
  std::vector<char> _encoded_data;
  size_t _p;

public:
  explicit HuffmanEncoder(HuffmanTree<T> huffman_tree);

  const std::vector<char>& get_encoded_data() const;
  std::vector<char> reset();
  template<typename I>
  ITER_REQUIREMENT(I, T)
  void encode(I begin, I end);

  HuffmanEncoder & operator<<(const T& c);
};
}  // namespace huffman


// Implementation

template<HUFFMAN_TYPE T>
huffman::HuffmanEncoder<T>::HuffmanEncoder(HuffmanTree<T> huffman_tree)
: _huffman_tree(std::move(huffman_tree)), _p(0) { }

template<HUFFMAN_TYPE T>
const std::vector<char> & huffman::HuffmanEncoder<T>::get_encoded_data() const {
  return _encoded_data;
}

template<HUFFMAN_TYPE T>
std::vector<char> huffman::HuffmanEncoder<T>::reset() {
  auto v = std::move(_encoded_data);
  _encoded_data.clear();
  _p = 0;
  return v;
}

template<HUFFMAN_TYPE T>
template<typename I>
ITER_REQUIREMENT(I, T)
void huffman::HuffmanEncoder<T>::encode(I begin, I end) {
  if constexpr (HAS_MINUS(end, begin) && std::is_convertible_v<decltype(end - begin), size_t>) {
    _encoded_data.reserve(static_cast<size_t>(end - begin));
  }
  for (; begin != end; ++begin) {
    *this << *begin;
  }
}

template<HUFFMAN_TYPE T>
huffman::HuffmanEncoder<T> & huffman::HuffmanEncoder<T>::operator<<(const T& c) {
  auto coded = _huffman_tree.get_code(c);
  if (!coded.has_value()) {
    return std::nullopt;
  }
  for (bool v : coded.value().get()) {
    if (_p / 8 >= _encoded_data.size()) {
      _encoded_data.push_back(0);
    }
    if (v) {
      _encoded_data.back() |= static_cast<char>(0x80 >> static_cast<int>(_p % 8));
    }
    ++_p;
  }
}
